#pragma once
#include <vector>
#include <functional>
#include "Core/Tools/Macros.h"

class Vector2;
namespace Iris
{
	namespace Engine
	{
		enum class InputState : unsigned int;

		class InputAction
		{
		public:
			~InputAction();

			//Key
			IRIS_ENGINE_API void AddPressedAction(std::function<void()> _function);
			IRIS_ENGINE_API void DeletePressedAction(std::function<void()> _function);

			IRIS_ENGINE_API void AddReleasedAction(std::function<void()> _function);
			IRIS_ENGINE_API void DeleteReleasedAction(std::function<void()> _function);

			IRIS_ENGINE_API void AddRepeatedAction(std::function<void()> _function);
			IRIS_ENGINE_API void DeleteRepeatedAction(std::function<void()> _function);

			//MouseButton
			IRIS_ENGINE_API void AddPressedAction(std::function<void(Vector2 _mousePos)> _function);
			IRIS_ENGINE_API void DeletePressedAction(std::function<void(Vector2 _mousePos)> _function);

			IRIS_ENGINE_API void AddReleasedAction(std::function<void(Vector2 _mousePos)> _function);
			IRIS_ENGINE_API void DeleteReleasedAction(std::function<void(Vector2 _mousePos)> _function);

			IRIS_ENGINE_API void AddRepeatedAction(std::function<void(Vector2 _mousePos)> _function);
			IRIS_ENGINE_API void DeleteRepeatedAction(std::function<void(Vector2 _mousePos)> _function);

			IRIS_ENGINE_API void StartAction(InputState _state);
			IRIS_ENGINE_API void StartAction(InputState _state, Vector2 _mousePos);
		private:
			//Key
			std::vector<std::function<void()>> m_PressedKeyActions;
			std::vector<std::function<void()>> m_ReleasedKeyActions;
			std::vector<std::function<void()>> m_RepeatedKeyActions;

			//MouseButton
			std::vector<std::function<void(Vector2 _mousePos)>> m_PressedMouseButtonActions;
			std::vector<std::function<void(Vector2 _mousePos)>> m_ReleasedMouseButtonActions;
			std::vector<std::function<void(Vector2 _mousePos)>> m_RepeatedMouseButtonActions;

			bool keyAction = true;
		};
	}
}