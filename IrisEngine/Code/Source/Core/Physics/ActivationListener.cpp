#include "Core/Physics/ActivationListener.h"
#include "SECS/Components/RigidBody.h"

namespace Iris
{
	namespace Engine
	{
		void IActivationListener::OnBodyActivated(RigidBody* _body)
		{
			if (!_body)
				return;

			//std::string message = "\"" + _body->GetIrisObject()->GetName() + "\" is activated" ;
		    //DEBUG_LOG(message.c_str());
			_body->OnActivated();
		}
		void IActivationListener::OnBodyDeactivated(RigidBody* _body)
		{
			if (!_body)
				return;

			//std::string message = "\"" + _body->GetIrisObject()->GetName() + "\" is deactivated";
			//DEBUG_LOG(message.c_str());
			_body->OnDeactivated();
		}
	}
}