#pragma once

#include "imgui.h"
#include "ImGuizmo.h"

#include <string>

namespace Iris
{
	namespace Editor
	{
		class ImGuiWidgets
		{
		public:
			static void ImGuiVector3(std::string _name, std::string _hiddenTag, float& _x, float& _y, float& _z, float _step, float _minValue, float _maxValue);
		};
	}
}