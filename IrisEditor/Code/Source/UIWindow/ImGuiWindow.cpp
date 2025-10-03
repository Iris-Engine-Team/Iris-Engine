#include "UIWindow/ImGuiWindow.h"

#include "imgui.h"

namespace Iris
{
	namespace Editor
	{
		void ImGuiEditorWindow::StartWindow(bool _lightMode)
		{
			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_NoCollapse;
			window_flags |= ImGuiWindowFlags_MenuBar;
			if(p_UnSavedDocument)
				window_flags |= ImGuiWindowFlags_UnsavedDocument;
			p_Open = false;

			if (p_Focused && !p_OverrideCol)
			{
				ImGui::PushStyleColor(ImGuiCol_WindowBg, _lightMode ? ImVec4(127.f / 255.f, 56.f / 255.f, 234.f / 255.f, 1) : ImVec4(42.f / 255.f, 19.f / 255.f, 47.f / 255.f, 0.8f));
			}

			if(p_Open)
				p_WindowOpened = ImGui::Begin(p_Name.c_str(), &p_Open, window_flags);
			else
				p_WindowOpened = ImGui::Begin(p_Name.c_str(), NULL, window_flags);

			if (p_Focused && !p_OverrideCol)
				ImGui::PopStyleColor();

			p_OldWindowSize = p_WindowSize;
			ImVec2 curSize = ImGui::GetContentRegionAvail();
			p_WindowSize = Vector2(curSize.x, curSize.y);
		}

		void ImGuiEditorWindow::CloseWindow()
		{
			p_Focused = ImGui::IsWindowFocused();
			ImGui::End();
			p_WindowOpened = false;
		}
	}
}