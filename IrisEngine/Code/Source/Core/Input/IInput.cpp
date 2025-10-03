#include "Core/Input/IInput.h"

namespace Iris
{
	namespace Engine
	{
		void IInput::SetInputState(InputState _newInputState)
		{
			m_CurrentInputState = _newInputState;
		}

		const InputState& IInput::GetInputState()
		{
			return m_CurrentInputState;
		}

		unsigned int IInput::ConvertToIris(unsigned int _key)
		{
			_key;
			return 0;
		}

		unsigned int IInput::ConvertToGLFW()
		{
			return 0;
		}

		unsigned int IInput::ConvertToImGui()
		{
			return 0;
		}

		void IInput::SetInput(IrisInput _newInput)
		{
			m_Input = _newInput;
		}

		const IrisInput& IInput::GetInput()
		{
			return m_Input;
		}
	}
}