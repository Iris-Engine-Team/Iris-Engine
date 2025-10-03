#pragma once

namespace Iris
{
	namespace Editor
	{
		class ImGuiEditorWindow;

		class IUIWindow
		{
		public:
			virtual ~IUIWindow() = default;

			virtual void StartWindow(bool _lightMode) = 0;
			virtual void CloseWindow() = 0;
			virtual bool DrawWindow(unsigned int _cFrame) = 0;

			IE_FORCE_INLINE std::string GetName() { return p_Name; };

			std::string p_Name = "Window";

			virtual IE_FORCE_INLINE ImGuiEditorWindow* CastToImGui() { return nullptr; }
		};
	}
}