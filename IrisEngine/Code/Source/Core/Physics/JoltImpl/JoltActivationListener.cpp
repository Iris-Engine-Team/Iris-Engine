#include "Core/Physics/JoltImpl/JoltActivationListener.h"
#include "Core/Physics/JoltImpl/JoltPhysicsSysteme.h"

namespace Iris
{
	namespace Engine
	{
		JoltActivationListener::JoltActivationListener(IPhysicsSystem* _system)
		{
			p_System = _system;
		}

		void JoltActivationListener::OnBodyActivated(const JPH::BodyID& _bodyId, JPH::uint64 _bodyUserData)
		{
			_bodyUserData;
			IActivationListener::OnBodyActivated(p_System->CastToJolt()->FindBody(_bodyId));
		}

		void JoltActivationListener::OnBodyDeactivated(const JPH::BodyID& _bodyId, JPH::uint64 _bodyUserData)
		{
			_bodyUserData;
			IActivationListener::OnBodyDeactivated(p_System->CastToJolt()->FindBody(_bodyId));
		}
	}
}