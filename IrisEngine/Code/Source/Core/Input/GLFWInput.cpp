#include "Core/Input/GLFWInput.h"

#include "GLFW/glfw3.h"

#include <array>

namespace Iris
{
	namespace Engine
	{
		unsigned int GLFWInput::ConvertToIris(unsigned int _key)
		{
			size_t size = m_GlfwArray.size();
			for (size_t i = 0; i < size; ++i)
			{
				if (m_GlfwArray[i] == _key)
					return static_cast<unsigned int>(i);
			}

			return 0;
		}

		unsigned int GLFWInput::ConvertToGLFW()
		{
			return m_GlfwArray[static_cast<unsigned int>(m_Input)];
		}
	}
}