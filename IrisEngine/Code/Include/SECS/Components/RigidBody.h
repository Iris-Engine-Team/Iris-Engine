#pragma once
#include "SECS/Components/IComponent.h"

#include "Vector/Vector3.h"
#include "Quaternion/Quaternion.h"
#include "Core/Physics/PhysicsLayer.h"

namespace Iris
{
	namespace Engine
	{
		class IPhysicsSystem;
		class Collider;
		class JoltPhysicBody;

		struct ContactInfo;

		class ITransform;


		enum class MotionType : uint16_t
		{
			IE_Static = 0,						///< Non movable
			IE_Kinematic = 1,					///< Movable using velocities only, does not respond to forces
			IE_Dynamic = 2,						///< Responds to forces as a normal physics object
		};

		class Constraint
		{
		public:
			Constraint() { }

			IRIS_ENGINE_API IE_FORCE_INLINE Vector3 GetRotationConstraint() const { return m_RotationConstraint; }
			IRIS_ENGINE_API IE_FORCE_INLINE void SetRotationConstraint(bool _x, bool _y, bool _z) { m_RotationConstraint = Vector3((float)_x, (float)_y, (float)_z); }
			IRIS_ENGINE_API IE_FORCE_INLINE void SetRotationConstraint(const Vector3& _rotationConstraint) { m_RotationConstraint = _rotationConstraint; }
		
		private:
			//1 = lock axis, 0 unlock axis
			Vector3 m_RotationConstraint = Vector3::s_Zero;
		};

		class IPhysicBody
		{
		public:
			virtual ~IPhysicBody() = default;

			/// <summary>
			/// Casts an IPhysicBody into a JoltPhysicBody
			/// </summary>
			virtual JoltPhysicBody* CastToJolt() { return nullptr; }

			/// <summary>
			/// Init function
			/// </summary>
			/// <param name="_physicsSystem">: need it to add the body i the physical scene</param>
			/// <param name="_collider">: shape of the body</param>
			/// <param name="_transform"></param>
			/// <param name="_motionType">: describe the motion of the body like Dynamic, Static , ...</param>
			/// <param name="_layer">: collision layer of the body</param>
			virtual void Init(IPhysicsSystem* _physicsSystem, Collider* _collider, ITransform* _transform, MotionType _motionType, PhysicLayer _layer) = 0;

			virtual void Destroy() = 0;
			
			/// <summary>
			/// Adds a body if its already created
			/// </summary>
			virtual void AddBodyToPhysicsSystem(IPhysicsSystem* _physicsSystem) = 0;

			/// <summary>
			/// Rmoves a body from the physical scene
			/// </summary>
			virtual void RemoveFromPhysicsSystem(IPhysicsSystem* _physicsSystem) = 0;

			/// <summary>
			/// Removes and destroy a body from the physical scene
			/// </summary>
			virtual void PermanentlyRemoveFromPhysicsSystem(IPhysicsSystem* _physicsSystem) = 0;

			IE_FORCE_INLINE void ActivateBody() { p_UpdateAwake = true; p_Awake = true; }
			IE_FORCE_INLINE void DeactivateBody() { p_UpdateAwake = true; p_Awake = false; }
			
			/// <summary>
			/// Sets active or deactivate a body juste before the physics update
			/// </summary>
			IE_FORCE_INLINE virtual void ApplyActivation(IPhysicsSystem* _physicsSystem) = 0;

			/// <returns>The linked collider ref</returns>
			Collider* GetCollider();

			/// <summary>
			/// Links the collider with the rigidbody
			/// </summary>
			/// <param name="_collider">: collider to link</param>
			void SetCollider(Collider* _collider);

			/// <summary>
			/// Sets the physical position
			/// </summary>
			/// <param name="_physicsSystem"></param>
			/// <param name="_position">: world position</param>
			virtual void SetPosition(IPhysicsSystem* _physicsSystem, const Vector3& _position) = 0;

			/// <returns>The world physical position</returns>
			virtual Vector3 GetPosition() const = 0;

			/// <summary>
			/// Sets the physical rotation
			/// </summary>
			/// <param name="_rotation">: world rotation</param>
			virtual void SetRotation(IPhysicsSystem* _physicsSystem, const Quaternion& _rotation) = 0;

			/// <returns>The world physical rotation</returns>
			virtual Quaternion GetRotation() const = 0;

			/// <returns>Velocity of the physical body</returns>
			virtual Vector3 GetVelocity() const = 0;

			/// <summary>
			/// Sets velocity, it will be applied just before the physics update
			/// </summary>
			/// <param name="_velocity">: velocity to set</param>
			IE_FORCE_INLINE void SetVelocity(const Vector3& _velocity) { p_UpdateVelocity = true; p_VelocityToSet = _velocity; }

			/// <summary>
			/// Sets velocity, it will be applied just before the physics update
			/// </summary>
			/// <param name="_velocity">: velocity to set</param>
			IE_FORCE_INLINE void SetAngularVelocity(const Vector3& _angularVelocity) { p_UpdateAngularVelocity = true; p_AngularVelocityToSet = _angularVelocity; }

			/// <summary>
			/// Adds force to the current "add force to set", it will be applied just before the physics update 
			/// <para>Only for dynamic motion</para>
			/// </summary>
			/// <param name="_force">: force to add</param>
			IE_FORCE_INLINE void AddForce(const Vector3& _force) { p_UpdateAddForce = true; p_AddForceToSet += _force; }

			/// <summary>
			/// Apply velocity and angular velocity just before the physics update  
			/// </summary>
			virtual void ApplyVelocity(IPhysicsSystem* _physicsSystem) = 0;
			
			/// <summary>
			/// Apply force just before the physics update  
			/// </summary>
			virtual void ApplyAddForce(IPhysicsSystem* _physicsSystem) = 0;

			/// <summary>
			/// Sets motion type
			/// </summary>
			/// <param name="_motionType">: wanted motion</param>
			virtual void SetMotionType(IPhysicsSystem* _physicsSystem, MotionType _motionType) = 0;
			
			/// <summary>
			/// Sets collision layer of the physical body
			/// </summary>
			/// <param name="_layer">: wanted layer</param>
			virtual void SetLayer(IPhysicsSystem* _physicsSystem, PhysicLayer _layer) = 0;
			
			/// <returns>The current collison layer of the body</returns>
			virtual PhysicLayer GetLayer() const = 0;
			
			/// <summary>
			/// Changes the internal shape of the physical body
			/// </summary>
			virtual void ChangeShape(IPhysicsSystem* _physicsSystem) = 0;

			/// <summary>
			/// Sets body as a trigger or a collider
			/// </summary>
			virtual void SetIsTrigger(bool _isTrigger) = 0;

			/// <summary>
			/// Sets the friction of the body
			/// </summary>
			/// <param name="_friction">: friction to set (can be negative)</param>
			virtual void SetFriction(float _friction) = 0;

			/// <summary>
			/// Sets mass
			/// </summary>
			/// <param name="_mass">: mass in kg</param>
			virtual void SetMass(float _mass) = 0;

			/// <summary>
			/// Sets rotation constraint between this body and an other (or dummy)
			/// </summary>
			/// <param name="_constraint">: constraint that defines the locked axes</param>
			virtual void SetRotationConstraint(IPhysicsSystem* _physicsSystem, const Constraint& _constraint) = 0;

			/// <summary>
			/// Removes the constraint
			/// </summary>
			virtual void RemoveConstraint(IPhysicsSystem* _physicsSystem) = 0;

		protected:

			/// <summary>
			/// Collider ref
			/// </summary>
			ComponentHandle<Collider>* p_Collider = nullptr;

			/// <summary>
			/// Velocity to apply
			/// </summary>
			Vector3 p_VelocityToSet = Vector3::s_Zero;
			
			/// <summary>
			/// Dirty flag that allows to know if we need to apply velocity
			/// </summary>
			bool p_UpdateVelocity = false;

			/// <summary>
			/// Angular velocity to apply
			/// </summary>
			Vector3 p_AngularVelocityToSet = Vector3::s_Zero;

			/// <summary>
			/// Dirty flag that allows to know if we need to apply angular velocity
			/// </summary>
			bool p_UpdateAngularVelocity = false;

			/// <summary>
			/// Force to add
			/// </summary>
			Vector3 p_AddForceToSet = Vector3::s_Zero;
			
			/// <summary>
			/// Dirty flag that allows to know if we need to add force
			/// </summary>
			bool p_UpdateAddForce = false;

			/// <summary>
			/// Activation state to set
			/// </summary>
			bool p_Awake = true;

			/// <summary>
			/// Dirty flag that allows to know if we need to modify the activation state
			/// </summary>
			bool p_UpdateAwake = true;
		};


		class RigidBody : public IComponent
		{
		public:
			
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
			//							Component DEF							 //
			DEFINE_COMPONENT(RigidBody);
			static IE_FORCE_INLINE std::string GetName() { return "RigidBody"; }
			static IE_FORCE_INLINE ComponentID GetID() { return ComponentID::IE_RIGIDBODY; }
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

			~RigidBody() override = default;

			/// <summary>
			/// Writes data in the given metadata
			/// </summary>
			/// <param name="_metadata"></param>
			void Serialize(IMetadata* _metadata) final override;

			/// <summary>
			/// Init the components with the given data
			/// </summary>
			void Deserialization(IMetadata* _metadata, InitEditorContext& _initializer) final override;

			IRIS_ENGINE_API void Init(InitEditorContext& _initializer) override;
			IRIS_ENGINE_API void Init(IPhysicsSystem* _physicsSystem);
			IRIS_ENGINE_API virtual void Init(IPhysicsSystem* _physicsSystem, MotionType _motionType, Collider* _collider = nullptr);
			void Copy(IComponent* _componentToCopy) override;
			IRIS_ENGINE_API void Duplicate(IComponent* _componentToDuplicate, InitEditorContext* _initializer) final override;
			void Destroy() override;

			/// <summary>
			/// Adds the body in the physics system
			/// </summary>
			void AddToPhysicsSystem();

			/// <summary>
			/// Removes the body from the physics system
			/// </summary>
			void RemoveFromPhysicsSystem();

			/// <summary>
			/// Removes and destroy the body from the physics system
			/// </summary>
			void PermanentlyRemoveFromPhysicsSystem();

			/// <summary>
			/// Sets enable or disable the body
			/// <para>Adds or removes from the physics system</para>
			/// </summary>
			IRIS_ENGINE_API void SetEnable(bool _enable) final override;

			/// <summary>
			/// Apply activation before system update
			/// </summary>
			void ApplyActivation();
			
			/// <param name="_position">: Set world position</param>
			IRIS_ENGINE_API void SetPosition(const Vector3& _position);

			/// <summary>
			///  Sets physic position
			/// </summary>
			IRIS_ENGINE_API void SetPhysiquePosition();

			/// <param name="_position">: target position interpolation</param>
			IRIS_ENGINE_API void SetTargetPosition(const Vector3& _position);

			/// <returns>The physique position of the object (!= position transform)</returns>
			IRIS_ENGINE_API Vector3 GetPosition();
			IE_FORCE_INLINE Vector3* GetPositionPtr() { GetPosition(); return &m_Position; }

			/// <param name="_rotation">: the physic rotation</param>
			IRIS_ENGINE_API void SetRotation(const Quaternion& _rotation);

			/// <returns>The physic rotation</returns>
			IRIS_ENGINE_API Quaternion GetRotation();
			IE_FORCE_INLINE Quaternion* GetRotationPtr() { GetRotation(); return &m_Rotation; }

			/// <returns>The velocity</returns>
			IRIS_ENGINE_API Vector3 GetVelocity();
			IE_FORCE_INLINE Vector3* GetVelocityPtr() { GetVelocity(); return &m_Velocity; }

			/// <summary>
			/// Sets velocity
			/// <para>(IE_Kinematic and IE_Dynamic)</para>
			/// </summary>
			IRIS_ENGINE_API void SetVelocity(Vector3 _velocity);

			/// <summary>
			/// Sets angular velocity
			/// <para>(IE_Kinematic and IE_Dynamic)</para>
			/// </summary>
			IRIS_ENGINE_API void SetAngularVelocity(Vector3 _velocity);

			/// <summary>
			/// Adds force to the body
			/// <para>(IE_Dynamic)</para>
			/// </summary>
			IRIS_ENGINE_API void AddForce(Vector3 _force);

			/// <summary>
			/// Apply the velocity stored before the physic update
			/// </summary>
			void ApplyVelocity();

			/// <summary>
			/// Adds the force stored before the physic update
			/// </summary>
			void ApplyAddForce();

			/// <summary>
			/// Interpolate the position (if m_Interpolate)
			/// </summary>
			void InterpolatePosition(float _timeLerp, ITransform* _transform);

			IRIS_ENGINE_API IE_FORCE_INLINE void SetInterpolatePosition(bool _interpolate) { m_Interpolate = _interpolate; }
			IRIS_ENGINE_API IE_FORCE_INLINE bool IsInterpolatePosition() const { return m_Interpolate; }

			IRIS_ENGINE_API void SetMotionType(MotionType _motionType);
			IRIS_ENGINE_API IE_FORCE_INLINE MotionType GetMotionType() const { return m_MotionType; }

			/// <summary>
			/// Called by the collider if the rigidBody is init before the collider
			/// </summary>
			void LinkCollider(Collider* _collider);

			/// <returns>The linked collider</returns>
			IRIS_ENGINE_API Collider* GetCollider();

			/// <summary>
			/// Adds to the ShapeToModidy list of the physics system
			/// </summary>
			void UpdateColliderShape();
			IE_FORCE_INLINE void ShapeUpdated() { m_ToUpdateShape = false; }

			IRIS_ENGINE_API IE_FORCE_INLINE IPhysicBody* GetPhysicalBody() { return m_PhysicBody; }

			IRIS_ENGINE_API void SetIsTrigger(bool _isTrigger);
			IRIS_ENGINE_API IE_FORCE_INLINE bool IsTrigger() const { return m_IsTrigger; }

			IRIS_ENGINE_API void SetFriction(float _friction);
			IRIS_ENGINE_API float GetFriction() const { return m_Friction; }

			/// <summary>
			/// Sets mass, (don't works IE_Static)
			/// </summary>
			IRIS_ENGINE_API void SetMass(float _mass);
			IRIS_ENGINE_API float GetMass() const { return m_Mass; }

			/// <summary>
			/// Sets rotational constraint, locks rotation axes
			/// </summary>
			IRIS_ENGINE_API void SetRotationConstraint(const Constraint& _constraint, bool _checkSame = true);

			/// <summary>
			/// Removes rotational constraint
			/// </summary>
			IRIS_ENGINE_API void RemoveConstraint();

			/// <returns>The current rotaional constraint</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE const Constraint& GetConstraint() const { return m_Constraint; }
	
			/// <summary>
			/// On collision enter event
			/// </summary>
			void OnCollisionEnter(const ContactInfo& _contactInfo);

			/// <summary>
			/// On collision stay event, keeps the body awake
			/// </summary>
			void OnCollisionStay(const ContactInfo& _contactInfo);

			/// <summary>
			/// On collision exit event
			/// </summary>
			void OnCollisionExit(const ContactInfo& _contactInfo);

			/// <summary>
			/// On trigger enter event
			/// </summary>
			void OnTriggerEnter(const ContactInfo& _contactInfo);

			/// <summary>
			/// On trigger stay event, keeps the body awake
			/// </summary>
			void OnTriggerStay(const ContactInfo& _contactInfo);

			/// <summary>
			/// On trigger exit event
			/// </summary>
			void OnTriggerExit(const ContactInfo& _contactInfo);

			/// <summary>
			/// On activate event
			/// </summary>
			void OnActivated();

			/// <summary>
			/// On deactivate event
			/// </summary>
			void OnDeactivated();

		private:

			Vector3 m_Position = Vector3::s_Zero;
			Quaternion m_Rotation = Quaternion::s_Identity;
			Vector3 m_Velocity = Vector3::s_Zero;

			/// <summary>
			/// Current Rotational constraint
			/// </summary>
			Constraint m_Constraint;

			/// <summary>
			/// Collision layer
			/// </summary>
			PhysicLayer m_Layer;

			/// <summary>
			/// Physic body simulated
			/// </summary>
			IPhysicBody* m_PhysicBody = nullptr;

			/// <summary>
			/// Physics system ref
			/// </summary>
			IPhysicsSystem* m_PhysicsSystem = nullptr;

			/// <summary>
			/// Motion type
			/// </summary>
			MotionType m_MotionType = MotionType::IE_Dynamic;

			bool m_ToUpdateShape = false;

			/// <summary>
			/// Interpolation start position
			/// </summary>
			Vector3 m_StartPos;

			/// <summary>
			/// Interpolation current position
			/// </summary>
			Vector3 m_CurrentPos;

			/// <summary>
			/// Interpolation target position
			/// </summary>
			Vector3 m_TargetPos;

			/// <summary>
			/// If system must interpolate the position
			/// </summary>
			bool m_Interpolate = false;

			/// <summary>
			/// Mass in kg
			/// </summary>
			float m_Mass = 1.f;

			/// <summary>
			/// Friction
			/// </summary>
			float m_Friction = 0.5f;

			/// <summary>
			/// Is trigger body
			/// </summary>
			bool m_IsTrigger = false;


			/// <summary>
			/// Activation state to set
			/// </summary>
			bool p_Activate = true;

			/// <summary>
			/// Dirty flag that allows to know if we need to modify the activation state
			/// </summary>
			bool p_UpdateActivation = true;
		};
	}
}