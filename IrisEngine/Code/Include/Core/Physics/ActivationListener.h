#pragma once

namespace Iris
{
	namespace Engine
	{
		class IPhysicsSystem;
		class RigidBody;

		class IActivationListener
		{
		public:
			virtual ~IActivationListener() = default;
		protected:

			/// <summary>
			/// Calls the function On body Activated of the rigidbody.
			/// <para>If the rigidbody has just been created, the rigidbody will call its function itself</para>
			/// </summary>
			/// <param name="_body">: rigidbody to call its function</param>
			void OnBodyActivated(RigidBody* _body);

			/// <summary>
			/// Calls the function On body Deactivated of the rigidbody.
			/// </summary>
			/// <param name="_body">: rigidbody to call its function</param>
			void OnBodyDeactivated(RigidBody* _body);

		protected:
			IPhysicsSystem* p_System = nullptr;
		};
	}
}