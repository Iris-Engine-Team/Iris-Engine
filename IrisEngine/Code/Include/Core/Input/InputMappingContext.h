#pragma once

#include <unordered_map>
#include "Core/Tools/Macros.h"
#include "Vector/Vector2.h"

namespace Iris
{
	namespace Engine
	{
		class InputAction;
		enum class IrisInput : unsigned int;
		enum class InputState : unsigned int;

		class InputMappingContext
		{
		public:
			IRIS_ENGINE_API InputMappingContext();
			~InputMappingContext();

			IRIS_ENGINE_API void AddInputWithAction(InputAction* _inputAction, IrisInput _input);

			IRIS_ENGINE_API void AddActionToInput(InputAction* _inputAction, IrisInput _input);

			IRIS_ENGINE_API void InitiateAction(IrisInput _input, InputState _inputState, Vector2 _mousePos = Vector2(-1, -1));

			IRIS_ENGINE_API IE_FORCE_INLINE void SetActive(bool _enable) { m_Enable = _enable; }
			IRIS_ENGINE_API IE_FORCE_INLINE bool IsActive() const { return m_Enable; }

		private:
			bool m_Enable = true;

			std::unordered_map<IrisInput, InputAction*> m_InputMapping;
		};
	}
}