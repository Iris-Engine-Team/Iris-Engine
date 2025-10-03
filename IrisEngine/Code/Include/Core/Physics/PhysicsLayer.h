#pragma once
namespace Iris
{
	namespace Engine
	{
		using PhysicLayer = uint16_t;
		namespace Layers
		{
			static constexpr PhysicLayer IE_NON_MOVING = 0;
			static constexpr PhysicLayer IE_MOVING = 1;
			static constexpr PhysicLayer IE_TRIGGER = 2;
			static constexpr PhysicLayer IE_DISABLE = 3;
			static constexpr PhysicLayer IE_NUM_LAYERS = 4;
		};
	}
}