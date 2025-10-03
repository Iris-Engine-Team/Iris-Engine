#include "Core/Physics/JoltImpl/JoltPhysicsSysteme.h"

#include "Core/Physics/JoltImpl/JoltContactListener.h"
#include "Core/Physics/JoltImpl/JoltActivationListener.h"

#include "Jolt/Jolt.h"
#include <Jolt/Physics/PhysicsSystem.h>
#include "Core/Physics/JoltImpl/JoltPhysicsLayer.h"

#include "SECS/Components/ComponentTable.h"

// Jolt includes
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Constraints/SixDOFConstraint.h>

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

// Disable common warnings triggered by Jolt, you can use JPH_SUPPRESS_WARNING_PUSH / JPH_SUPPRESS_WARNING_POP to store and restore the warning state
JPH_SUPPRESS_WARNINGS

static void TraceImpl(const char* inFMT, ...)
{
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);

	DEBUG_LOG(buffer);
}

#ifdef JPH_ENABLE_ASSERTS
static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, JPH::uint inLine)
{
	std::string expr = inExpression;
	std::string infile = inFile;
	std::string line = std::to_string(inLine);
	std::string message = infile + ":" + line + ": (" + expr + ") " + (inMessage != nullptr ? inMessage : "");
	DEBUG_LOG(message.c_str());
	
	return true;
};

#endif // JPH_ENABLE_ASSERTS

namespace Iris
{
	namespace Engine
	{

		JoltPhysicsSystem::JoltPhysicsSystem(Scene* _scene)
			:IPhysicsSystem::IPhysicsSystem(_scene)
		{
		}

		void JoltPhysicsSystem::Init(size_t _nbThread)
		{
			RegisterDefaultAllocator();

			Trace = TraceImpl;
			JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;)

			Factory::sInstance = new Factory();

			RegisterTypes();

			m_TempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);

			m_JobSystem = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, static_cast<int>(_nbThread));

			const uint cMaxBodies = 1024;

			const uint cNumBodyMutexes = 0;

			const uint cMaxBodyPairs = 1024;

			const uint cMaxContactConstraints = 1024;

			m_LayerInterface = new BPLayerInterfaceImpl();

			m_ObjectVSBroadLayerFilter = new ObjectVsBroadPhaseLayerFilterImpl();

			m_ObjectVSObjectLayerFilter = new ObjectLayerPairFilterImpl();

			m_PhysicsSystem = new JPH::PhysicsSystem();
			m_PhysicsSystem->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, *m_LayerInterface, *m_ObjectVSBroadLayerFilter, *m_ObjectVSObjectLayerFilter);

			JoltActivationListener* activationListener = new JoltActivationListener(this);
			m_ActivationListener = activationListener;
			m_PhysicsSystem->SetBodyActivationListener(activationListener);

			JoltContactListener* contactListener = new JoltContactListener(this);
			p_ContactListener = contactListener;
			m_PhysicsSystem->SetContactListener(contactListener);


			BodyCreationSettings dummySettings(
				new JPH::BoxShape(JPH::Vec3(0.1f, 0.1f, 0.1f)),
				JPH::Vec3::sZero(),
				JPH::Quat::sIdentity(),
				JPH::EMotionType::Static, Layers::IE_DISABLE);
			m_Dummy = m_PhysicsSystem->GetBodyInterface().CreateBody(dummySettings);
			m_PhysicsSystem->GetBodyInterface().AddBody(m_Dummy->GetID(), EActivation::DontActivate);
		}

		void JoltPhysicsSystem::Destroy()
		{
			if (p_RigidBodies)
			{
				std::vector<RigidBody>& rbs = p_RigidBodies->GetAllComponents();

				size_t size = rbs.size();
				for (size_t i = 0; i < size; ++i)
					rbs[i].PermanentlyRemoveFromPhysicsSystem();
			}

			JPH::BodyInterface& bi = m_PhysicsSystem->GetBodyInterface();
			bi.RemoveBody(m_Dummy->GetID());
			bi.DestroyBody(m_Dummy->GetID());

			IPhysicsSystem::Destroy();

			delete m_ObjectVSBroadLayerFilter;
			delete m_LayerInterface;
			delete m_ObjectVSObjectLayerFilter;
			delete m_PhysicsSystem;

			UnregisterTypes();

			delete Factory::sInstance;
			Factory::sInstance = nullptr;

			delete m_TempAllocator;
			delete m_JobSystem;
		}

		void JoltPhysicsSystem::Clear()
		{
			std::vector<RigidBody>& rbs = p_RigidBodies->GetAllComponents();

			size_t size = rbs.size();
			for (size_t i = 0; i < size; ++i)
				rbs[i].PermanentlyRemoveFromPhysicsSystem();

			IPhysicsSystem::Clear();
		}

		void JoltPhysicsSystem::Update(double _fixedDeltaTime)
		{
			ComponentTable<ITransform>* trsTable = p_SceneRef->GetComponentTable<ITransform>();
			std::vector<ITransform>& trs = trsTable->GetAllComponents();

			ComponentTable<Collider>* colTable = p_SceneRef->GetComponentTable<Collider>();
			std::vector<Collider>& cols = colTable->GetAllComponents();

			std::vector<RigidBody>& rbs = p_RigidBodies->GetAllComponents();

			std::vector<Vector3> offsets;

			size_t size = p_RigidBodies->ComponentCount();
			for (size_t i = 0; i < size; ++i)
			{
				rbs[i].ApplyActivation();

				if (!rbs[i].GetPhysicalBody() || !rbs[i].IsEnable())
				{
					offsets.push_back(Vector3::s_Zero);
					continue;
				}


				ITransform* t = &trs[trsTable->ContainsEntity(rbs[i].GetIrisObject())];

				Collider* col = &cols[colTable->ContainsEntity(rbs[i].GetIrisObject())];

				Vector3 pos = t->GetGlobalPosition();
				Vector3 offsetPos = col->GetPhysicPositionWithOffSet(t);
				offsets.push_back(offsetPos - pos);

				rbs[i].SetPosition(offsetPos);
				rbs[i].SetRotation(t->GetGlobalRotation());
				rbs[i].SetPhysiquePosition();

				rbs[i].ApplyVelocity();
				rbs[i].ApplyAddForce();
			}

			int step = static_cast<int>(_fixedDeltaTime / (1.f / 60.f));
			if (step == 0)
				step = 1;
			const int cCollisionSteps = step;

			m_PhysicsSystem->Update(static_cast<float>(_fixedDeltaTime), cCollisionSteps, m_TempAllocator, m_JobSystem);

			for (size_t i = 0; i < size; ++i)
			{
				if (!rbs[i].GetPhysicalBody() || !rbs[i].IsEnable())
					continue;

				Vector3 rbNewPos = rbs[i].GetPosition();
				rbs[i].SetTargetPosition(rbNewPos);

				ITransform* t = &trs[trsTable->ContainsEntity(rbs[i].GetIrisObject())];

				if (!rbs[i].IsInterpolatePosition())
					t->SetGlobalPosition(rbNewPos - offsets[i]);
				t->SetGlobalRotation(rbs[i].GetRotation());
			}
		}

		IShape* JoltPhysicsSystem::CreateShape(PhysicShape _shapeType)
		{
			switch (_shapeType)
			{
			case PhysicShape::IE_SHAPE_BOX:
			{
				return new JoltBoxCollider();
			}
			case PhysicShape::IE_SHAPE_SPHERE:
			{
				return new JoltSphereCollider();
			}
			case PhysicShape::IE_SHAPE_CAPSULE:
			{
				return new JoltCapsuleCollider();
			}
			default:
			{
				return nullptr;
			}
			}
		}

		IPhysicBody* JoltPhysicsSystem::CreateBody()
		{
			return new JoltPhysicBody();
		}

		JPH::BodyInterface& JoltPhysicsSystem::GetBodyInterface()
		{
			return m_PhysicsSystem->GetBodyInterface();
		}

		std::array<RigidBody*, 2> JoltPhysicsSystem::FindPairCollision(const JPH::BodyID& _body1, const JPH::BodyID& _body2)
		{
			std::array<RigidBody*, 2> pairRb{ nullptr,nullptr };
			
			bool nFound1 = true;
			bool nFound2 = true;

			std::vector<RigidBody>& rbs = p_RigidBodies->GetAllComponents();
			size_t size = p_RigidBodies->ComponentCount();
			for (size_t i = 0; i < size; ++i)
			{
				const JPH::BodyID& body = rbs[i].GetPhysicalBody()->CastToJolt()->GetJoltBody()->GetID();

				if (nFound1 && body == _body1)
				{
					nFound1 = false;
					pairRb[0] = &rbs[i];
				}
				else if (nFound2 && body == _body2)
				{
					nFound2 = false;
					pairRb[1] = &rbs[i];
				}

				if (!nFound1 && !nFound2)
					return pairRb;
			}

			return pairRb;
		}

		RigidBody* JoltPhysicsSystem::FindBody(const JPH::BodyID& _body)
		{
			std::vector<RigidBody>& rbs = p_RigidBodies->GetAllComponents();
			size_t size = p_RigidBodies->ComponentCount();
			for (size_t i = 0; i < size; ++i)
			{
				if (!rbs[i].GetPhysicalBody())
					continue;

				const JPH::BodyID& body = rbs[i].GetPhysicalBody()->CastToJolt()->GetJoltBody()->GetID();
				if (body == _body)
					return &rbs[i];
			}
			return nullptr;
		}

		void JoltPhysicsSystem::AddConstraint(JPH::SixDOFConstraint* _constraint)
		{
			m_PhysicsSystem->AddConstraint(_constraint);
		}

		void JoltPhysicsSystem::RemoveConstraint(JPH::SixDOFConstraint* _constraint)
		{
			m_PhysicsSystem->RemoveConstraint(_constraint);
		}
	}
}