#pragma once
#include "SECS/Components/RigidBody.h"
#include "SECS/Components/Collider.h"

namespace Iris
{
	namespace Engine
	{
		class Scene;
		class JoltPhysicsSystem;
		class IContactListener;
		class IActivationListener;

		template<class T>
		class ComponentTable;

		enum class PhysicShape : int;
		enum class PhysicType : int
		{
			IE_TYPE_JOLT = 0
		};

		class IPhysicsSystem
		{
		public:
			IPhysicsSystem(Scene* _scene);
			virtual ~IPhysicsSystem() = default;

			void SetScene(Scene* _scene);

			virtual void Init(size_t _nbThread) = 0;
			virtual void Destroy();

			virtual void Clear();

			/// <summary>
			/// Casts an IPhysicsSystem into a JoltPhysicsSystem
			/// </summary>
			virtual JoltPhysicsSystem* CastToJolt() { return nullptr; }

			/// <summary>
			/// Updates the physics of all rigidbody link to a collider
			/// </summary>
			virtual void Update(double _fixedDeltaTime) = 0;

			/// <summary>
			/// Interpolates the position of all physical objects that have the interpolate flag ON
			/// </summary>
			/// <param name="_timerInterpolate">: timer between (0 - 1)</param>
			void Interpolate(float _timerInterpolate);

			/// <summary>
			/// Instantiates a shape 
			/// </summary>
			/// <param name="_shapeType">: type of the wanted shape</param>
			virtual IShape* CreateShape(PhysicShape _shapeType) = 0;

			/// <summary>
			/// Instantiates a physical body in the physic scene
			/// </summary>
			virtual IPhysicBody* CreateBody() = 0;

			/// <summary>
			/// Modifies the internal collider shape of rigidbody in the list 
			/// </summary>
			void UpdateModifiedShape();

			/// <summary>
			/// Adds rigidbody in the list of shape to update
			/// </summary>
			/// <param name="_rigidbody"></param>
			void AddShapeToModidy(RigidBody* _rigidbody);

		protected:
			
			Scene* p_SceneRef = nullptr;

			/// <summary>
			/// List of all rigidbody components
			/// </summary>
			ComponentTable<RigidBody>* p_RigidBodies = nullptr;

			/// <summary>
			/// List of all collider components
			/// </summary>
			ComponentTable<Collider>* p_Colliders = nullptr;

			/// <summary>
			/// List of rigidbodies that need to be updated
			/// </summary>
			std::vector<ComponentHandle<RigidBody>> p_ShapeToUpdate;

			/// <summary>
			/// Listner that invokes on Collision/Trigger ENTER/STAY/EXIT events 
			/// </summary>
			IContactListener* p_ContactListener = nullptr;
			
			/// <summary>
			/// Listener that invokes on Activated/Deactivated events
			/// <para>(The first on activated event is call by the rigidbody himself because is not already in the rigidbody component table)</para>
			/// </summary>
			IActivationListener* m_ActivationListener = nullptr;
		};
	}
}