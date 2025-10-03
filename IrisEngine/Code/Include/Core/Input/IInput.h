#pragma once
#include "Core/Tools/Macros.h"
#include "Core/Input/InputEnum.h"
#include "Vector/Vector2.h"

#include <array>

namespace Iris
{
	namespace Engine
	{
		class IInput
		{
		public:

			IRIS_ENGINE_API void SetInputState(InputState _newInputState);
			IRIS_ENGINE_API const InputState& GetInputState();

			IRIS_ENGINE_API virtual  unsigned int ConvertToIris(unsigned int _key);
			IRIS_ENGINE_API virtual unsigned int ConvertToGLFW();
			IRIS_ENGINE_API virtual unsigned int ConvertToImGui();

			IRIS_ENGINE_API void SetInput(IrisInput _newInput);
			IRIS_ENGINE_API const IrisInput& GetInput();
		
			//Vector2 MousePos;
		protected:
			InputState m_CurrentInputState;
			IrisInput m_Input;
		};
	}
}