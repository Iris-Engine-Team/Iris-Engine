#pragma once

#include "Core/Tools/Macros.h"

#include "Vector/Vector3.h"
#include "Vector/Vector2.h"

namespace Iris
{
	namespace Engine
	{
		struct Vertex
		{
			Vector3 Position = Vector3(1.f);
			Vector3 Normal = Vector3(1.f);
			Vector2 TextCoord = Vector2(1.f);
			Vector3 Tangent = Vector3(1.f);
			Vector3 BiTangent = Vector3(1.f);
		};
	}
}