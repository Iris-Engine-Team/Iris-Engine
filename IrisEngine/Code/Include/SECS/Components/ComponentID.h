#pragma once 

namespace Iris
{
	namespace Engine
	{
		enum class ComponentID : int
		{
			IE_TRANSFORM = 0,
			IE_COLLIDER = 1,
			IE_RIGIDBODY = 2,
			IE_MODEL_RENDERER = 3,
			IE_LIGHT = 4,
			IE_CAMERA = 5,
			IE_SCRIPT = 6,
			IE_NUM_COMPONENT = 7
		};
	}
}