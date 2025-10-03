#pragma once
#include "../PhysicsLayer.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Collision/ObjectLayer.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhase.h"

namespace Iris
{
	namespace Engine
	{

		JPH_SUPPRESS_WARNINGS

		using namespace JPH;
		using namespace JPH::literals;
		using namespace std;

		class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter
		{
		public:
			virtual bool					ShouldCollide(PhysicLayer inObject1, PhysicLayer inObject2) const override
			{
				switch (inObject1)
				{
				case Layers::IE_NON_MOVING:
					return inObject2 == Layers::IE_MOVING;
				case Layers::IE_MOVING:
					return inObject2 != Layers::IE_DISABLE;
				case Layers::IE_TRIGGER:
					return inObject2 == Layers::IE_MOVING;
				case Layers::IE_DISABLE:
					return false;
				default:
					JPH_ASSERT(false);
					return false;
				}
			}
		};

		namespace BroadPhaseLayers
		{
			static constexpr BroadPhaseLayer NON_MOVING(0);
			static constexpr BroadPhaseLayer MOVING(1);
			static constexpr BroadPhaseLayer TRIGGER(2);
			static constexpr BroadPhaseLayer DISABLE(3);
			static constexpr uint NUM_LAYERS(4);
		};

		class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface
		{
		public:
			BPLayerInterfaceImpl()
			{
				// Create a mapping table from object to broad phase layer
				mObjectToBroadPhase[Layers::IE_NON_MOVING] = BroadPhaseLayers::NON_MOVING;
				mObjectToBroadPhase[Layers::IE_MOVING] = BroadPhaseLayers::MOVING;
				mObjectToBroadPhase[Layers::IE_TRIGGER] = BroadPhaseLayers::TRIGGER;
				mObjectToBroadPhase[Layers::IE_DISABLE] = BroadPhaseLayers::DISABLE;
			}

			virtual uint					GetNumBroadPhaseLayers() const override
			{
				return BroadPhaseLayers::NUM_LAYERS;
			}

			virtual BroadPhaseLayer			GetBroadPhaseLayer(ObjectLayer inLayer) const override
			{
				JPH_ASSERT(inLayer < Layers::IE_NUM_LAYERS);
				return mObjectToBroadPhase[inLayer];
			}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
			virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
			{
				switch ((BroadPhaseLayer::Type)inLayer)
				{
				case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
				case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
				case (BroadPhaseLayer::Type)BroadPhaseLayers::TRIGGER:		return "TRIGGER";
				case (BroadPhaseLayer::Type)BroadPhaseLayers::DISABLE:		return "DISABLE";
				default:													JPH_ASSERT(false); return "INVALID";
				}
			}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

		private:
			BroadPhaseLayer					mObjectToBroadPhase[Layers::IE_NUM_LAYERS];
		};

		/// Class that determines if an object layer can collide with a broadphase layer
		class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter
		{
		public:
			virtual bool				ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override
			{
				switch (inLayer1)
				{
				case Layers::IE_NON_MOVING:
					return inLayer2 == BroadPhaseLayers::MOVING;
				case Layers::IE_MOVING:
					return inLayer2 != BroadPhaseLayers::DISABLE;
				case Layers::IE_TRIGGER:
					return inLayer2 == BroadPhaseLayers::MOVING;
				case Layers::IE_DISABLE:
					return false;
				default:
					JPH_ASSERT(false);
					return false;
				}
			}
		};

	}
}