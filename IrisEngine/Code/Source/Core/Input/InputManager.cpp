#include "Core/Input/InputManager.h"

#include "Core/GLFWWindow.h"
#include "Core/Input/IInput.h"
#include "Core/Input/InputMappingContext.h"

#include "Core/Input/InputEnum.h"

#include <GLFW/glfw3.h>

#include "Core/Tools/Log.h"

namespace Iris
{
	namespace Engine
	{
		InputManager::InputManager(IWindow* _window)
		{
			m_Window = _window;
		}

		InputManager::~InputManager()
		{
			for (auto it = m_InputMappingContext.begin(); it != m_InputMappingContext.end(); ++it)
				delete it->second;

			m_InputMappingContext.clear();
		}

		void InputManager::AddInputMappingContext(std::string _name, InputMappingContext* _newMap)
		{
			auto imc = m_InputMappingContext.find(_name);
			if (imc == m_InputMappingContext.end())
				m_InputMappingContext[_name] = _newMap;
		}

		InputMappingContext* InputManager::GetInputMappingContext(std::string _name)
		{
			auto imc = m_InputMappingContext.find(_name);
			if (imc == m_InputMappingContext.end())
				return nullptr;

			return imc->second;
		}

		void InputManager::AddInputInUpdate(IInput _input)
		{
			size_t vectorSize = InputUpdateList.size();
			for (size_t i = 0; i < vectorSize; i++)
			{
				if (InputUpdateList[i].GetInput() == _input.GetInput())
				{
					InputUpdateList[i].SetInputState(_input.GetInputState());
					return;
				}
			}
			InputUpdateList.push_back(_input);
		}

		void InputManager::Update()
		{
			if (InputUpdateList.size() > 0)
			{
				std::vector<size_t> m_DeleteInputUpdateList;

				size_t vectorLength = InputUpdateList.size();


				double x, y = 0.0;
				glfwGetCursorPos(m_Window->CastToGLFW()->GetRaw(), &x, &y);
				Vector2 mousePos = Vector2((float)x, (float)y);

				for (size_t i = 0; i < vectorLength; i++)
				{
					IInput inpp = static_cast<IInput>(InputUpdateList[i]);
					

					for (auto imc = m_InputMappingContext.begin(); imc != m_InputMappingContext.end(); ++imc)
					{
						if (!imc->second->IsActive())
							continue;

						if ((unsigned int)InputUpdateList[i].GetInput() < 8)
							imc->second->InitiateAction(InputUpdateList[i].GetInput(), InputUpdateList[i].GetInputState(), mousePos);
						else
							imc->second->InitiateAction(InputUpdateList[i].GetInput(), InputUpdateList[i].GetInputState());

					}

					if (InputUpdateList[i].GetInputState() == InputState::PRESS)
					{
						InputUpdateList[i].SetInputState(InputState::REPEAT);
					}
					if (InputUpdateList[i].GetInputState() == InputState::RELEASE)
					{
						m_DeleteInputUpdateList.push_back(i);
					}
				}

				vectorLength = m_DeleteInputUpdateList.size();
				int decr = 0;

				for (size_t i = 0; i < vectorLength; i++)
				{
					InputUpdateList.erase(InputUpdateList.begin() + (m_DeleteInputUpdateList[i] - decr));
					++decr;
				}
				m_DeleteInputUpdateList.clear();
			}
		}
	}
}