#pragma once
#include "Vector/Vector3.h"

namespace Iris
{
	namespace Engine
	{
		class IPhysicsSystem;
		class RigidBody;

		struct ContactInfo
		{
		public:
			Vector3 ContactPoint;
			Vector3 Normal;
			RigidBody* Other;
		};

		class IContactListener
		{
		public:
			virtual ~IContactListener() = default;
		protected:

			/// <summary>
			/// Calls the function On Collision Enter of 2 rigibodies
			/// </summary>
			void OnContactAdded(RigidBody* _body1, RigidBody* _body2);

			/// <summary>
			/// Calls the function On Collision Stay of 2 rigibodies and keeps them awake
			/// </summary>
			void OnContactPersisted(RigidBody* _body1, RigidBody* _body2);
			
			/// <summary>
			/// Calls the function On Collision Exit of 2 rigibodies
			/// </summary>
			void OnContactRemoved(RigidBody* _body1, RigidBody* _body2);

			/// <summary>
			/// Calls the function On Trigger Enter for the trigger type rigibody
			/// </summary>
			void OnTriggerAdded(RigidBody* _body1, RigidBody* _triggerCollider);

			/// <summary>
			/// Calls the function On Trigger Stay for the trigger type rigibody
			/// </summary>
			void OnTriggerPersisted(RigidBody* _body1, RigidBody* _triggerCollider);

			/// <summary>
			/// Calls the function On Trigger Exit for the trigger type rigibody
			/// </summary>
			void OnTriggerRemoved(RigidBody* _body1, RigidBody* _triggerCollider);

		protected:
			IPhysicsSystem* p_System = nullptr;
		};
	}
}