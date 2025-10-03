#include "UITools/ImGuiWidgets.h"

namespace Iris
{
	namespace Editor
	{
		void ImGuiWidgets::ImGuiVector3(std::string _name, std::string _hiddenTag, float& _x, float& _y, float& _z, float _step, float _minValue, float _maxValue)
		{
			ImGui::Text(_name.c_str());
			ImGui::SameLine();
			ImGui::Spacing();
			ImGui::SameLine();
			std::string newhiddenTag = "x##" + _hiddenTag;
			ImGui::DragFloat(newhiddenTag.c_str(), &_x, _step, _minValue, _maxValue, "%.3f");
			ImGui::SameLine();
			newhiddenTag = "y##" + _hiddenTag;
			ImGui::DragFloat(newhiddenTag.c_str(), &_y, _step, _minValue, _maxValue, "%.3f");
			ImGui::SameLine();
			newhiddenTag = "z##" + _hiddenTag;
			ImGui::DragFloat(newhiddenTag.c_str(), &_z, _step, _minValue, _maxValue, "%.3f");
		}
	}
}
