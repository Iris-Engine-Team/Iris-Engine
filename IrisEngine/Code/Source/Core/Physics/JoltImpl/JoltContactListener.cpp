#include "Core/Physics/JoltImpl/JoltContactListener.h"
#include "Core/Physics/JoltImpl/JoltPhysicsSysteme.h"
#include "SECS/Components/RigidBody.h"
#include "Core/Physics/JoltImpl/JoltPhysicsLayer.h"
namespace Iris
{
	namespace Engine
	{
		JoltContactListener::JoltContactListener(IPhysicsSystem* _system)
		{
			p_System = _system;
		}

		JPH::ValidateResult	JoltContactListener::OnContactValidate(const JPH::Body& _body1, const JPH::Body& _body2, JPH::RVec3Arg _inBaseOffset, const JPH::CollideShapeResult& _inCollisionResult)
		{
			_inBaseOffset; _inCollisionResult;

			if (_body1.IsSensor() && _body2.IsSensor())
				return JPH::ValidateResult::RejectAllContactsForThisBodyPair;

			return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
		}

		void JoltContactListener::OnContactAdded(const JPH::Body& _body1, const JPH::Body& _body2, const JPH::ContactManifold& _inManifold, JPH::ContactSettings& ioSettings)
		{
			_inManifold; ioSettings;

			std::array<RigidBody*, 2> rbs = p_System->CastToJolt()->FindPairCollision(_body1.GetID(), _body2.GetID());

			if (_body1.IsSensor())
				IContactListener::OnTriggerAdded(rbs[1], rbs[0]);
			else if (_body2.IsSensor())
				IContactListener::OnTriggerAdded(rbs[0], rbs[1]);
			else
				IContactListener::OnContactAdded(rbs[0], rbs[1]);
		}

		void JoltContactListener::OnContactPersisted(const JPH::Body& _body1, const JPH::Body& _body2, const JPH::ContactManifold& _inManifold, JPH::ContactSettings& ioSettings)
		{
			_inManifold; ioSettings;

			std::array<RigidBody*, 2> rbs = p_System->CastToJolt()->FindPairCollision(_body1.GetID(), _body2.GetID());

			if (_body1.IsSensor())
				IContactListener::OnTriggerPersisted(rbs[1], rbs[0]);
			else if (_body2.IsSensor())
				IContactListener::OnTriggerPersisted(rbs[0], rbs[1]);
			else
				IContactListener::OnContactPersisted(rbs[0], rbs[1]);
		}

		void JoltContactListener::OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair)
		{
			std::array<RigidBody*, 2> rbs = p_System->CastToJolt()->FindPairCollision(inSubShapePair.GetBody1ID(), inSubShapePair.GetBody2ID());

			if (rbs[0] && rbs[1])
			{
				if (rbs[0]->IsTrigger())
					IContactListener::OnTriggerRemoved(rbs[1], rbs[0]);
				else if (rbs[1]->IsTrigger())
					IContactListener::OnTriggerRemoved(rbs[0], rbs[1]);
				else
					IContactListener::OnContactRemoved(rbs[0], rbs[1]);
			}
		}
	}
}
