#pragma once
#include "Core/Tools/Macros.h"
#include "Vector/Vector2.h"

// STD
#include <vector>
#include <unordered_map>

namespace Iris
{
	namespace Engine
	{
		class IInput;
		class InputMappingContext;
		class IWindow;

		class InputManager
		{
		public:
			IRIS_ENGINE_API InputManager(IWindow* _window);
			IRIS_ENGINE_API ~InputManager();

			IRIS_ENGINE_API void AddInputMappingContext(std::string _name, InputMappingContext* _newMap);
			IRIS_ENGINE_API InputMappingContext* GetInputMappingContext(std::string _name);

			IRIS_ENGINE_API void AddInputInUpdate(IInput _input);

			IRIS_ENGINE_API void Update();

		public:
			std::unordered_map<std::string, InputMappingContext*> m_InputMappingContext;

			std::vector<IInput> InputUpdateList;
			IWindow* m_Window;
		private:
		};
	}
}