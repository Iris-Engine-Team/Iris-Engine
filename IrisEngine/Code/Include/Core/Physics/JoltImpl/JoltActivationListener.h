#pragma once
#include "Core/Physics/ActivationListener.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include"Core/Tools/Log.h"

namespace Iris
{
	namespace Engine
	{
		
		class JoltActivationListener : public JPH::BodyActivationListener, public IActivationListener
		{
		public:
			JoltActivationListener(IPhysicsSystem* _system);
			~JoltActivationListener() override = default;

			/// <summary>
			/// Calls the function On body Activated of the rigidbody
			/// <para>If the rigidbody has just been created, the rigidbody will call its function itself</para>
			/// </summary>
			/// <param name="_bodyId">: id of the jolt body to find the corresponding rigidbody</param>
			void OnBodyActivated(const JPH::BodyID& _bodyId, JPH::uint64 _bodyUserData) override;

			/// <summary>
			/// Calls the function On body Deactivated of the rigidbody
			/// </summary>
			/// <param name="_bodyId">: id of the jolt body to find the corresponding rigidbody</param>
			void OnBodyDeactivated(const JPH::BodyID& _bodyId, JPH::uint64 _bodyUserData) override;
		};

	}
}