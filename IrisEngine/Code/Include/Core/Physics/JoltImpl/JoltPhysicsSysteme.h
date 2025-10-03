#include "SECS/Systems/PhysicsSystem.h"

#include "Core/Physics/JoltImpl/JoltCollider.h"
#include "Core/Physics/JoltImpl/JoltRigidBody.h"

#include <array>

namespace JPH
{
	class BodyInterface;
	class PhysicsSystem;
	class JobSystemThreadPool;
	class TempAllocatorImpl;
	class SixDOFConstraint;
}

namespace Iris
{
	namespace Engine
	{
		class BPLayerInterfaceImpl;
		class ObjectVsBroadPhaseLayerFilterImpl;
		class ObjectLayerPairFilterImpl;
		class MyBodyActivationListener;
		class IContactListener;

		class JoltPhysicsSystem : public IPhysicsSystem
		{
		public:
			JoltPhysicsSystem(Scene* _scene);
			~JoltPhysicsSystem() override = default;

			void Init(size_t _nbThread) final override;
			void Destroy() final override;

			void Clear() final override;

			/// <summary>
			/// Casts an IPhysicsSystem into a JoltPhysicsSystem
			/// </summary>
			JoltPhysicsSystem* CastToJolt() override { return this; }

			/// <summary>
			/// Updates the physics of all rigidbody link to a collider
			/// </summary>
			void Update(double _fixedDeltaTime) override;

			/// <summary>
			/// Instantiates jolt shape
			/// </summary>
			/// <param name="_shapeType">: type of the wanted shape</param>
			IShape* CreateShape(PhysicShape _shapeType) override;

			/// <summary>
			/// Instantiates jolt body
			/// </summary>
			IPhysicBody* CreateBody() override;

			/// <returns>Jolt body interface</returns>
			JPH::BodyInterface& GetBodyInterface();

			/// <summary>
			/// Find a pair of rigidbody thanks to their jolt body ID
			/// </summary>
			/// <returns>The pair of rigidbody (nullptr if not found)</returns>
			std::array<RigidBody*, 2> FindPairCollision(const JPH::BodyID& _body1, const JPH::BodyID& _body2);

			/// <summary>
			/// Find a single rigidbody thanks to its jolt body ID
			/// </summary>
			/// <returns>The found rigidbody or nullptr if not</returns>
			RigidBody* FindBody(const JPH::BodyID& _body);

			/// <summary>
			/// Adds a rotation constraint between 2 bodies or 1 body and the dummy
			/// </summary>
			/// <param name="_constraint">: rotation constraint</param>
			void AddConstraint(JPH::SixDOFConstraint* _constraint);

			/// <summary>
			/// Removes a constraint from the physics system
			/// </summary>
			/// <param name="_constraint"></param>
			void RemoveConstraint(JPH::SixDOFConstraint* _constraint);

			JPH::Body* GetDummy() { return m_Dummy; }
		private:

			/// <summary>
			/// Jolt physics system
			/// </summary>
			JPH::PhysicsSystem*  m_PhysicsSystem = nullptr;

			/// <summary>
			/// Interface that manage broad phase layer
			/// </summary>
			BPLayerInterfaceImpl* m_LayerInterface = nullptr;

			/// <summary>
			/// Class that determines if an object layer can collide with a broadphase layer
			/// </summary>
			ObjectVsBroadPhaseLayerFilterImpl* m_ObjectVSBroadLayerFilter = nullptr;

			/// <summary>
			/// Class that determines if an object layer can collide with an other object layer
			/// </summary>
			ObjectLayerPairFilterImpl* m_ObjectVSObjectLayerFilter = nullptr;

			/// <summary>
			/// Memory allocator
			/// </summary>
			JPH::TempAllocatorImpl* m_TempAllocator = nullptr;

			/// <summary>
			/// Jolt task manager
			/// </summary>
			JPH::JobSystemThreadPool* m_JobSystem = nullptr;

			JPH::Body* m_Dummy = nullptr;
		};
	}
}