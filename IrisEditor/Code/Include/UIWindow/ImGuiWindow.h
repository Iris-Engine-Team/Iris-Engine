#pragma once

#include "Core/Tools/Macros.h"
#include "Vector/Vector2.h"
#include "UIWindow/IUIWindow.h"

namespace Iris
{
	namespace Editor
	{
		class ImGuiEditorWindow : public IUIWindow
		{
		public:
			virtual IE_FORCE_INLINE ~ImGuiEditorWindow() override = default;

			virtual void StartWindow(bool _lightMode) = 0;
			virtual void CloseWindow() = 0;
			virtual bool DrawWindow(unsigned int _cFrame) = 0;

			IE_FORCE_INLINE Vector2 GetWindowSize() { return p_WindowSize; }
			IE_FORCE_INLINE Vector2 GetOldWindowSize() { return p_OldWindowSize; }

			IE_FORCE_INLINE ImGuiEditorWindow* CastToImGui() override { return this; }

			virtual IE_FORCE_INLINE void SceneSaved() { p_UnSavedDocument = false; }
		protected:
			Vector2 p_WindowSize = Vector2(100.f);
			Vector2 p_OldWindowSize = Vector2(10.f);

			bool p_Open = false;

			bool p_Focused = false;

			bool p_OverrideCol = false;

			bool p_WindowOpened = false;

			bool p_UnSavedDocument = false;

		private:

		};
	}
}