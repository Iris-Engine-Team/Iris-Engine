#pragma once
#include "Core/Physics/ContactListener.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ContactListener.h>

#include"Core/Tools/Log.h"

namespace Iris
{
	namespace Engine
	{
		class JoltContactListener : public JPH::ContactListener , public IContactListener
		{
		public:
			JoltContactListener(IPhysicsSystem* _system);
			~JoltContactListener() override = default;

			/// <summary>
			/// Accept or reject a contact between 2 rigibodies in addition to layer and broad phase validation
			/// </summary>
			/// <param name="_inCollisionResult">: info on the contact</param>
			/// <returns>The result of the contact (accepte or reject)</returns>
			JPH::ValidateResult OnContactValidate(const JPH::Body& _body1, const JPH::Body& _body2, JPH::RVec3Arg _inBaseOffset, const JPH::CollideShapeResult& _inCollisionResult) final override;

			/// <summary>
			/// Calls the function On Collision/Trigger Enter of 2 rigibodies
			/// </summary>
			void OnContactAdded(const JPH::Body& _body1, const JPH::Body& _body2, const JPH::ContactManifold& _inManifold, JPH::ContactSettings& ioSettings) final override;

			/// <summary>
			/// Calls the function On Collision/Trigger Stay of 2 rigibodies and keeps them awake
			/// </summary>
			void OnContactPersisted(const JPH::Body& _body1, const JPH::Body& _body2, const JPH::ContactManifold& _inManifold, JPH::ContactSettings& ioSettings) final override;

			/// <summary>
			/// Calls the function On Collision/Trigger Exit of 2 rigibodies
			/// </summary>
			void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) final override;
		};
	}
}
