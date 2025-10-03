#include "Core/Physics/ContactListener.h"
#include "SECS/Components/RigidBody.h"

namespace Iris
{
	namespace Engine
	{
		void IContactListener::OnContactAdded(RigidBody* _body1, RigidBody* _body2)
		{
			ContactInfo ci;
			ci.Other = _body2;
			_body1->OnCollisionEnter(ci);

			ci.Other = _body1;
			_body2->OnCollisionEnter(ci);
		}

		void IContactListener::OnContactPersisted(RigidBody* _body1, RigidBody* _body2)
		{
			ContactInfo ci;
			ci.Other = _body2;
			_body1->OnCollisionStay(ci);

			ci.Other = _body1;
			_body2->OnCollisionStay(ci);
		}

		void IContactListener::OnContactRemoved(RigidBody* _body1, RigidBody* _body2)
		{
			ContactInfo ci;
			ci.Other = _body2;
			_body1->OnCollisionExit(ci);

			ci.Other = _body1;
			_body2->OnCollisionExit(ci);
		}

		void IContactListener::OnTriggerAdded(RigidBody* _body1, RigidBody* _triggerCollider)
		{
			ContactInfo ci;
			ci.Other = _body1;
			_triggerCollider->OnTriggerEnter(ci);
		}

		void IContactListener::OnTriggerPersisted(RigidBody* _body1, RigidBody* _triggerCollider)
		{
			ContactInfo ci;
			ci.Other = _body1;
			_triggerCollider->OnTriggerStay(ci);
		}

		void IContactListener::OnTriggerRemoved(RigidBody* _body1, RigidBody* _triggerCollider)
		{
			ContactInfo ci;
			ci.Other = _body1;
			_triggerCollider->OnTriggerExit(ci);
		}
	}
}