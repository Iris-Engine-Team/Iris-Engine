#include "Core/Input/InputMappingContext.h"

#include "Core/Input/InputEnum.h"
#include "Core/Input/InputAction.h"

namespace Iris
{
	namespace Engine
	{
		InputMappingContext::InputMappingContext()
		{
			for (unsigned int i = 0; i <= (unsigned int)IrisInput::IE_KEY_RIGHT_SUPER; i++)
				AddInputWithAction(nullptr, (IrisInput)i);
		}

		InputMappingContext::~InputMappingContext()
		{
			for (std::pair<IrisInput, InputAction*> pair : m_InputMapping)
			{
				if (pair.second)
				{
					delete(pair.second);
					pair.second = nullptr;
				}
			}
			m_InputMapping.clear();
		}

		void InputMappingContext::AddInputWithAction(InputAction* _inputAction, IrisInput _input)
		{
			std::pair<IrisInput, InputAction*> pair = { _input , _inputAction };
			m_InputMapping.insert(pair);
		}

		void InputMappingContext::AddActionToInput(InputAction* _inputAction, IrisInput _input)
		{
			m_InputMapping[_input] = _inputAction;
		}
		void InputMappingContext::InitiateAction(IrisInput _input, InputState _inputState, Vector2 _mousePos)
		{
			if (m_InputMapping[_input])
			{
				if (_mousePos == Vector2(-1, -1))
					m_InputMapping[_input]->StartAction(_inputState);
				else
					m_InputMapping[_input]->StartAction(_inputState, _mousePos);
			}
		}
	}
}