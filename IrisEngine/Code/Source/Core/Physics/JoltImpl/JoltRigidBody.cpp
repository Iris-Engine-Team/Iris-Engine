#include "Core/Physics/JoltImpl/JoltPhysicsSysteme.h"
#include "Core/Physics/JoltImpl/JoltRigidBody.h"
#include "Core/Physics//JoltImpl/JoltPhysicsLayer.h"
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Constraints/SixDOFConstraint.h>

JPH_SUPPRESS_WARNINGS

namespace Iris
{
	namespace Engine
	{
		void JoltPhysicBody::Init(IPhysicsSystem* _physicsSystem, Collider* _collider, ITransform* _transform, MotionType _motionType, PhysicLayer _layer)
		{
			p_Collider = new ComponentHandle<Collider>(_collider);

			IShape* shape = _collider->GetShape();
			
			Vector3 gPos = _collider->GetPhysicPositionWithOffSet(_transform);

			Quaternion gRota = _transform->GetGlobalRotation();

			JPH::EMotionType joltMotionType = JPH::EMotionType::Dynamic;
			switch (_motionType)
			{
			case MotionType::IE_Static: 
			{
				joltMotionType = JPH::EMotionType::Static;
				break;
			}
			case MotionType::IE_Kinematic:
			{
				joltMotionType = JPH::EMotionType::Kinematic;
				break;
			}
			default:
				break;
			}

			JPH::BodyCreationSettings settings(shape->GetShapeSettings()->CastJolt()->GetSettings()
				, JPH::RVec3(gPos[0], gPos[1], gPos[2]), JPH::Quat(gRota[1], gRota[2], gRota[3], gRota[0]), joltMotionType, _layer);

			settings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
			settings.mMassPropertiesOverride.mMass = 1.f;

			JPH::BodyInterface& bi = _physicsSystem->CastToJolt()->GetBodyInterface();
			m_JoltBody = bi.CreateBody(settings);
			bi.AddBody(m_JoltBody->GetID(), JPH::EActivation::Activate);
		}

		void JoltPhysicBody::Destroy()
		{
			if (p_Collider)
			{
				p_Collider->UseComponent()->LinkRigidBody(nullptr);
				delete p_Collider;
			}
		}

		void JoltPhysicBody::AddBodyToPhysicsSystem(IPhysicsSystem* _physicsSystem)
		{
			JPH::BodyInterface& bi = _physicsSystem->CastToJolt()->GetBodyInterface();
			if(!m_JoltBody->IsInBroadPhase())
				bi.AddBody(m_JoltBody->GetID(), JPH::EActivation::Activate);
		}

		void JoltPhysicBody::RemoveFromPhysicsSystem(IPhysicsSystem* _physicsSystem)
		{
			JPH::BodyInterface& bi = _physicsSystem->CastToJolt()->GetBodyInterface();
			bi.RemoveBody(m_JoltBody->GetID());
		}

		void JoltPhysicBody::PermanentlyRemoveFromPhysicsSystem(IPhysicsSystem* _physicsSystem)
		{
			JPH::BodyInterface& bi = _physicsSystem->CastToJolt()->GetBodyInterface();
			if(m_JoltBody->IsInBroadPhase())
				bi.RemoveBody(m_JoltBody->GetID());
			bi.DestroyBody(m_JoltBody->GetID());
		}

		void JoltPhysicBody::ApplyActivation(IPhysicsSystem* _physicsSystem)
		{
			if (!p_UpdateAwake)
				return;

			p_UpdateAwake = false;
			JPH::BodyInterface& bi = _physicsSystem->CastToJolt()->GetBodyInterface();
			if (p_Awake)
				bi.ActivateBody(m_JoltBody->GetID());
			else
				bi.DeactivateBody(m_JoltBody->GetID());
		}

		void JoltPhysicBody::SetPosition(IPhysicsSystem* _physicsSystem, const Vector3& _position)
		{
			_physicsSystem->CastToJolt()->GetBodyInterface().SetPosition(m_JoltBody->GetID(), JPH::Vec3(_position[0], _position[1], _position[2]), JPH::EActivation::Activate);
		}

		Vector3 JoltPhysicBody::GetPosition() const
		{
			JPH::RVec3 pos = m_JoltBody->GetPosition();
			return Vector3(pos.GetX(), pos.GetY(), pos.GetZ());
		}

		Quaternion JoltPhysicBody::GetRotation() const
		{
			JPH::Quat q = m_JoltBody->GetRotation();
			return Quaternion(q.GetW(), q.GetX(), q.GetY(), q.GetZ());
		}

		void JoltPhysicBody::SetRotation(IPhysicsSystem* _physicsSystem, const Quaternion& _rotation)
		{
			_physicsSystem->CastToJolt()->GetBodyInterface().SetRotation(m_JoltBody->GetID(), JPH::QuatArg(_rotation[1], _rotation[2], _rotation[3], _rotation[0]), JPH::EActivation::Activate);
		}

		Vector3 JoltPhysicBody::GetVelocity() const
		{
			JPH::Vec3 v = m_JoltBody->GetLinearVelocity();
			return Vector3(v.GetX(), v.GetY(), v.GetZ());
		}

		void JoltPhysicBody::ApplyVelocity(IPhysicsSystem* _physicsSystem)
		{
			if (p_UpdateVelocity)
			{
				p_UpdateVelocity = false;

				if (!m_JoltBody->IsActive())
					_physicsSystem->CastToJolt()->GetBodyInterface().ActivateBody(m_JoltBody->GetID());

				JPH::Vec3 v = JPH::Vec3Arg(p_VelocityToSet[0], p_VelocityToSet[1], p_VelocityToSet[2]);
				m_JoltBody->SetLinearVelocity(v);
			}

			if (p_UpdateAngularVelocity)
			{
				p_UpdateAngularVelocity = false;

				if (!m_JoltBody->IsActive())
					_physicsSystem->CastToJolt()->GetBodyInterface().ActivateBody(m_JoltBody->GetID());

				JPH::Vec3 v = JPH::Vec3Arg(p_AngularVelocityToSet[0], p_AngularVelocityToSet[1], p_AngularVelocityToSet[2]);
				m_JoltBody->SetAngularVelocity(v);
			}
		}

		void JoltPhysicBody::ApplyAddForce(IPhysicsSystem* _physicsSystem)
		{
			if (p_UpdateAddForce)
			{
				p_UpdateAddForce = false;

				JPH::Vec3 v = JPH::Vec3Arg(p_AddForceToSet[0], p_AddForceToSet[1], p_AddForceToSet[2]);
				_physicsSystem->CastToJolt()->GetBodyInterface().AddForce(m_JoltBody->GetID(), v);

				p_AddForceToSet = Vector3::s_Zero;
			}
		}

		void JoltPhysicBody::SetMotionType(IPhysicsSystem* _physicsSystem, MotionType _motionType)
		{
			PhysicLayer layer = Layers::IE_MOVING;
			JPH::EMotionType joltMotionType = JPH::EMotionType::Dynamic;
			switch (_motionType)
			{
			case Iris::Engine::MotionType::IE_Static:
			{
				layer = Layers::IE_NON_MOVING;
				joltMotionType = JPH::EMotionType::Static;
				break;
			}
			case Iris::Engine::MotionType::IE_Kinematic:
			{
				joltMotionType = JPH::EMotionType::Kinematic;
				break;
			}
			default:
				break;
			}

			PhysicLayer layerBase = m_JoltBody->GetObjectLayer();
			if (layerBase == Layers::IE_DISABLE || layerBase == Layers::IE_TRIGGER)
				layer = layerBase;

			JPH::BodyInterface& bi = _physicsSystem->CastToJolt()->GetBodyInterface();
			bi.SetObjectLayer(m_JoltBody->GetID(), layer);
			bi.SetMotionType(m_JoltBody->GetID(), joltMotionType, JPH::EActivation::Activate);
		}

		void JoltPhysicBody::SetLayer(IPhysicsSystem* _physicsSystem, PhysicLayer _layer)
		{
			_physicsSystem->CastToJolt()->GetBodyInterface().SetObjectLayer(m_JoltBody->GetID(), _layer);
		}

		PhysicLayer JoltPhysicBody::GetLayer() const
		{
			return m_JoltBody->GetObjectLayer();
		}

		void JoltPhysicBody::ChangeShape(IPhysicsSystem* _physicsSystem)
		{
			Collider* col = p_Collider->UseComponent();
			if (col)
			{
				_physicsSystem->CastToJolt()->GetBodyInterface().SetShape(m_JoltBody->GetID()
					, col->GetShape()->GetShapeSettings()->CastJolt()->GetSettings()
					, false, JPH::EActivation::Activate);
			}
		}

		void JoltPhysicBody::SetIsTrigger(bool _isTrigger)
		{
			m_JoltBody->SetIsSensor(_isTrigger);
		}

		void JoltPhysicBody::SetFriction(float _friction)
		{
			m_JoltBody->SetFriction(_friction);
		}

		void JoltPhysicBody::SetMass(float _mass)
		{
			JPH::MotionProperties* t_motionProperties = m_JoltBody->GetMotionProperties();
			const JPH::Shape* t_shape = m_JoltBody->GetShape();
			JPH::MassProperties t_massProperties = t_shape->GetMassProperties();

			t_massProperties.ScaleToMass(_mass);
			t_massProperties.mMass = _mass;
			t_motionProperties->SetMassProperties(JPH::EAllowedDOFs::All, t_massProperties);
		}

		void JoltPhysicBody::SetRotationConstraint(IPhysicsSystem* _physicsSystem, const Constraint& _constraint)
		{
			JPH::SixDOFConstraintSettings t_constraintSettings;
			t_constraintSettings.mPosition1 = JPH::Vec3(0, 0, 0);
			t_constraintSettings.mPosition2 = JPH::Vec3(0, 0, 0);
			Vector3 rotaConstraint = _constraint.GetRotationConstraint();
			if(rotaConstraint.GetX() > 0)
				t_constraintSettings.SetLimitedAxis(JPH::SixDOFConstraintSettings::EAxis::RotationX, 1.f, 0.f);
			if (rotaConstraint.GetY() > 0)
				t_constraintSettings.SetLimitedAxis(JPH::SixDOFConstraintSettings::EAxis::RotationY, 1.f, 0.f);
			if (rotaConstraint.GetZ() > 0)
				t_constraintSettings.SetLimitedAxis(JPH::SixDOFConstraintSettings::EAxis::RotationZ, 1.f, 0.f);
			JoltPhysicsSystem* jSys = _physicsSystem->CastToJolt();

			RemoveConstraint(_physicsSystem);
			m_JoltConstraint = new JPH::SixDOFConstraint(*m_JoltBody, *jSys->GetDummy(), t_constraintSettings);
			jSys->AddConstraint(m_JoltConstraint);
		}

		void JoltPhysicBody::RemoveConstraint(IPhysicsSystem* _physicsSystem)
		{
			if (m_JoltConstraint)
			{
				_physicsSystem->CastToJolt()->RemoveConstraint(m_JoltConstraint);
				m_JoltConstraint = nullptr;
			}
		}
	}
}