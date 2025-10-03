#pragma once
#include "SECS/Components/RigidBody.h"

#include "Quaternion/Quaternion.h"
#include "Vector/Vector3.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/Body.h"

namespace JPH
{
	class SixDOFConstraint;
}

namespace Iris
{
	namespace Engine
	{
		class JoltPhysicBody :public IPhysicBody
		{
		public:
			JoltPhysicBody() = default;
			~JoltPhysicBody() final override = default;

			/// <summary>
			/// Casts an IPhysicBody into a JoltPhysicBody
			/// </summary>
			JoltPhysicBody* CastToJolt() final override { return this; }

			/// <summary>
			/// Init function
			/// </summary>
			/// <param name="_physicsSystem">: need it to add the body i the physical scene</param>
			/// <param name="_collider">: shape of the body</param>
			/// <param name="_transform"></param>
			/// <param name="_motionType">: describe the motion of the body like Dynamic, Static , ...</param>
			/// <param name="_layer">: collision layer of the body</param>
			void Init(IPhysicsSystem* _physicsSystem, Collider* _collider, ITransform* _transform, MotionType _motionType, PhysicLayer _layer) final  override;
			
			void Destroy() final override;

			/// <summary>
			/// Adds a body if its already created
			/// </summary>
			void AddBodyToPhysicsSystem(IPhysicsSystem* _physicsSystem) final override;
			
			/// <summary>
			/// Rmoves a body from the physical scene
			/// </summary>
			void RemoveFromPhysicsSystem(IPhysicsSystem* _physicsSystem) final override;
			
			/// <summary>
			/// Removes and destroy a body from the physical scene
			/// </summary>
			void PermanentlyRemoveFromPhysicsSystem(IPhysicsSystem* _physicsSystem) final override;

			/// <summary>
			/// Sets active or deactivate a body juste before the physics update
			/// </summary>
			void ApplyActivation(IPhysicsSystem* _physicsSystem) final override;

			/// <summary>
			/// Sets the physical position
			/// </summary>
			/// <param name="_physicsSystem"></param>
			/// <param name="_position">: world position</param>
			void SetPosition(IPhysicsSystem* _physicsSystem, const Vector3& _position) final override;
			
			/// <returns>The world physical position</returns>
			Vector3 GetPosition() const final override;
			
			/// <summary>
			/// Sets the physical rotation
			/// </summary>
			/// <param name="_rotation">: world rotation</param>
			void SetRotation(IPhysicsSystem* _physicsSystem, const Quaternion& _rotation) final override;
			
			/// <returns>The world physical rotation</returns>
			Quaternion GetRotation() const final override;

			/// <returns>Velocity of the physical body</returns>
			Vector3 GetVelocity() const final override;
			
			/// <summary>
			/// Apply velocity just before the physics update  
			/// </summary>
			void ApplyVelocity(IPhysicsSystem* _physicsSystem) override;
			
			/// <summary>
			/// Apply force just before the physics update  
			/// </summary>
			void ApplyAddForce(IPhysicsSystem* _physicsSystem) final override;

			/// <summary>
			/// Sets motion type
			/// </summary>
			/// <param name="_motionType">: wanted motion</param>
			void SetMotionType(IPhysicsSystem* _physicsSystem, MotionType _motionType) final override;
			
			/// <summary>
			/// Sets collision layer of the physical body
			/// </summary>
			/// <param name="_layer">: wanted layer</param>
			void SetLayer(IPhysicsSystem* _physicsSystem, PhysicLayer _layer) final override;
			
			/// <returns>The current collison layer of the body</returns>
			PhysicLayer GetLayer() const final override;
			
			/// <summary>
			/// Changes the internal shape of the physical body
			/// </summary>
			void ChangeShape(IPhysicsSystem* _physicsSystem) final override;

			
			/// <returns>Jolt body</returns>
			IE_FORCE_INLINE JPH::Body* GetJoltBody() const { return m_JoltBody; }

			/// <summary>
			/// Sets body as a trigger or a collider
			/// </summary>
			void SetIsTrigger(bool _isTrigger) final override;

			/// <summary>
			/// Sets the friction of the body
			/// </summary>
			/// <param name="_friction">: friction to set (can be negative)</param>
			void SetFriction(float _friction) final override;

			/// <summary>
			/// Sets mass
			/// </summary>
			/// <param name="_mass">: mass in kg</param>
			void SetMass(float _mass) final override;

			/// <summary>
			/// Sets rotation constraint between this body and an other (or dummy)
			/// </summary>
			/// <param name="_constraint">: constraint that defines the locked axes</param>
			void SetRotationConstraint(IPhysicsSystem* _physicsSystem, const Constraint& _constraint) final override;
			
			/// <summary>
			/// Removes the constraint
			/// </summary>
			void RemoveConstraint(IPhysicsSystem* _physicsSystem) final override;
		private:

			/// <summary>
			/// Jolt body ref
			/// </summary>
			JPH::Body* m_JoltBody = nullptr;

			/// <summary>
			/// Rotation constraint
			/// </summary>
			JPH::SixDOFConstraint* m_JoltConstraint = nullptr;
		};
	}
}