#pragma once
#include "SECS/Components/IComponent.h"
#include "UISubWindow/IUISubWindow.h"

namespace Iris
{
	namespace Editor
	{
		class ImGuiSubWindow : public IUISubWindow
		{
		public:
			virtual ~ImGuiSubWindow() override = default;

			virtual void StartSubWindow() override = 0;
			virtual void CloseSubWindow() override = 0;
			virtual bool DrawSubWindow() override = 0;

			IE_FORCE_INLINE ImGuiSubWindow* CastToImGui() override { return this; }

			IE_FORCE_INLINE void SetSave(bool _saved) { p_ModifiedValue = _saved; }

		protected:
			std::string p_Name = "ImGuiSubWindow";

			bool p_ModifiedValue = false;
		};
	}
}