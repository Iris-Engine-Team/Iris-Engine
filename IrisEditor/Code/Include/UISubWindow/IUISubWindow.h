#pragma once

namespace Iris
{
	namespace Editor
	{
		class ImGuiSubWindow;

		class IUISubWindow
		{
		public:
			virtual ~IUISubWindow() = default;

			virtual void StartSubWindow() = 0;
			virtual void CloseSubWindow() = 0;
			virtual bool DrawSubWindow() = 0;

			virtual IE_FORCE_INLINE ImGuiSubWindow* CastToImGui() { return nullptr; }
		};
	}
}