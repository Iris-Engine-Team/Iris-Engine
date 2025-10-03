#include "Core/Input/IMGUIInput.h"

namespace Iris
{
	namespace Editor
	{
		unsigned int IMGUIInput::ConvertToImGui()
		{
			return m_ImguiArray[(unsigned int)m_Input];
		}
	}
}
