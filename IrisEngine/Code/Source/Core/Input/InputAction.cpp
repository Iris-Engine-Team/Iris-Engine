#pragma once
#include "Core/Input/InputAction.h"

#include "Vector/Vector2.h"
#include "Core/Input/InputEnum.h"

namespace Iris
{
	namespace Engine
	{
		InputAction::~InputAction()
		{
			m_PressedKeyActions.clear();
			m_ReleasedKeyActions.clear();
			m_RepeatedKeyActions.clear();

			m_PressedMouseButtonActions.clear();
			m_ReleasedMouseButtonActions.clear();
			m_RepeatedMouseButtonActions.clear();
		}

		void InputAction::AddPressedAction(std::function<void()> _function)
		{
			m_PressedKeyActions.push_back(_function);
			keyAction = true;
		}

		void InputAction::DeletePressedAction(std::function<void()> _function)
		{
			for (std::vector<std::function<void()>>::iterator function = m_PressedKeyActions.begin(); function != m_PressedKeyActions.end() + 1; function++)
			{
				if (function->target_type() == _function.target_type())
				{
					m_PressedKeyActions.erase(function);
				}
			}
		}

		void InputAction::AddReleasedAction(std::function<void()> _function)
		{
			m_ReleasedKeyActions.push_back(_function);
			keyAction = true;
		}

		void InputAction::DeleteReleasedAction(std::function<void()> _function)
		{
			for (std::vector<std::function<void()>>::iterator function = m_ReleasedKeyActions.begin(); function != m_ReleasedKeyActions.end() + 1; function++)
			{
				if (function->target_type() == _function.target_type())
				{
					m_ReleasedKeyActions.erase(function);
				}
			}
		}

		void InputAction::AddRepeatedAction(std::function<void()> _function)
		{
			m_RepeatedKeyActions.push_back(_function);
			keyAction = true;
		}

		void InputAction::DeleteRepeatedAction(std::function<void()> _function)
		{
			for (std::vector<std::function<void()>>::iterator function = m_RepeatedKeyActions.begin(); function != m_RepeatedKeyActions.end() + 1; function++)
			{
				if (function->target_type() == _function.target_type())
				{
					m_RepeatedKeyActions.erase(function);
				}
			}
		}

		void InputAction::AddPressedAction(std::function<void(Vector2 _mousePos)> _function)
		{
			m_PressedMouseButtonActions.push_back(_function);
			keyAction = false;
		}
		void InputAction::DeletePressedAction(std::function<void(Vector2 _mousePos)> _function)
		{
			for (std::vector<std::function<void(Vector2 _mousePos)>>::iterator function = m_PressedMouseButtonActions.begin(); function != m_PressedMouseButtonActions.end() + 1; function++)
			{
				if (function->target_type() == _function.target_type())
				{
					m_PressedMouseButtonActions.erase(function);
				}
			}
		}

		void InputAction::AddReleasedAction(std::function<void(Vector2 _mousePos)> _function)
		{
			m_ReleasedMouseButtonActions.push_back(_function);
			keyAction = false;
		}
		void InputAction::DeleteReleasedAction(std::function<void(Vector2 _mousePos)> _function)
		{
			for (std::vector<std::function<void(Vector2 _mousePos)>>::iterator function = m_ReleasedMouseButtonActions.begin(); function != m_ReleasedMouseButtonActions.end() + 1; function++)
			{
				if (function->target_type() == _function.target_type())
				{
					m_ReleasedMouseButtonActions.erase(function);
				}
			}
		}

		void InputAction::AddRepeatedAction(std::function<void(Vector2 _mousePos)> _function)
		{
			m_RepeatedMouseButtonActions.push_back(_function);
			keyAction = false;
		}
		void InputAction::DeleteRepeatedAction(std::function<void(Vector2 _mousePos)> _function)
		{
			for (std::vector<std::function<void(Vector2 _mousePos)>>::iterator function = m_RepeatedMouseButtonActions.begin(); function != m_RepeatedMouseButtonActions.end() + 1; function++)
			{
				if (function->target_type() == _function.target_type())
				{
					m_RepeatedMouseButtonActions.erase(function);
				}
			}
		}

		void InputAction::StartAction(InputState _state)
		{
			size_t vectorSize = 0;
			switch (_state) 
			{
			case InputState::PRESS:
				vectorSize = m_PressedKeyActions.size();
				for (size_t i = 0; i < vectorSize; i++)
				{
					m_PressedKeyActions[i]();
				}
				break;
			case InputState::RELEASE:
				vectorSize = m_ReleasedKeyActions.size();
				for (size_t i = 0; i < vectorSize; i++)
				{
					m_ReleasedKeyActions[i]();
				}
				break;
			case InputState::REPEAT:
				vectorSize = m_RepeatedKeyActions.size();
				for (size_t i = 0; i < vectorSize; i++)
				{
					m_RepeatedKeyActions[i]();
				}
				break;
			}
		}
		void InputAction::StartAction(InputState _state, Vector2 _mousePos)
		{
			size_t vectorSize = 0;
			switch (_state)
			{
			case InputState::PRESS:
				vectorSize = m_PressedMouseButtonActions.size();
				for (size_t i = 0; i < vectorSize; i++)
				{
					m_PressedMouseButtonActions[i](_mousePos);
				}
				break;
			case InputState::RELEASE:
				vectorSize = m_ReleasedMouseButtonActions.size();
				for (size_t i = 0; i < vectorSize; i++)
				{
					m_ReleasedMouseButtonActions[i](_mousePos);
				}
				break;
			case InputState::REPEAT:
				vectorSize = m_RepeatedMouseButtonActions.size();
				for (size_t i = 0; i < vectorSize; i++)
				{
					m_RepeatedMouseButtonActions[i](_mousePos);
				}
				break;
			}
		}
	}
}