#include "UIWindow/ConsoleWindow.h"

//ENGINETOOLS
#include "Core/Tools/Log.h"
#include "Core/Tools/FilePath.h"

//STD
#include <iostream>
#include <fstream>
#include <string>

//UITOOLS
#include "UITools/ImGuiFontManager.h"

//IMGUI
#include "imgui.h"

namespace Iris
{
	namespace Editor
	{
		ConsoleWindow::ConsoleWindow(std::string _hiddenID)
		{
			p_Name = "Log##" + _hiddenID;
			m_ListenerID = Debug::Log::AddListener([this](const std::string& _msg, Vector3 _color) 
			{
				AddLog(_msg, _color);
			});
			p_OverrideCol = true;
		}

		ConsoleWindow::~ConsoleWindow()
		{
			Debug::Log::RemoveListener(m_ListenerID);
		}

		void ConsoleWindow::StartWindow(bool _lightMode)
		{
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::IsWindowFocused() ? ImVec4(0.f, 0.f, 0.f, 1.f) : ImVec4(0.f,0.f,0.f,0.8f));
			ImGuiEditorWindow::StartWindow(_lightMode);
			ImGui::PopStyleColor();
		}
		void ConsoleWindow::CloseWindow()
		{
			ImGuiEditorWindow::CloseWindow();
		}

		bool ConsoleWindow::DrawWindow(unsigned int _cFrame)
		{
			_cFrame;
			if (p_WindowOpened)
			{
				if (ImGui::BeginMenuBar())
				{
					if (ImGui::Button("Clear"))
					{
						m_LogLines.clear();
					}
					ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMin().x, ImGui::GetCursorPosY()));
					float offsetY = 2.f;
					float maxPosButton = ImGui::GetItemRectMax().y;
					float offset = 15.f;
					ImVec2 p1 = ImVec2(ImGui::GetCursorScreenPos().x - offset, maxPosButton - offsetY);
					ImVec2 p2 = ImVec2(p1.x + ImGui::GetWindowSize().x + offset, p1.y + 2.0f);
					ImGui::GetWindowDrawList()->AddRectFilled(p1, p2, IM_COL32(0, 0, 0, 255));
					ImGui::EndMenuBar();
				}
				ImGui::PushFont(FontManager::GetFontFromName("RobotoMono-BoldItalic"));
				for (auto line : m_LogLines)
				{
					Vector3 color = line.second;
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(color.GetX(), color.GetY(), color.GetZ(), 1.f));
					ImGui::Text(line.first.c_str());
					ImGui::PopStyleColor();
				}
				ImGui::PopFont();
			}
			return p_UnSavedDocument;
		}

		void ConsoleWindow::AddLog(const std::string& _log, Vector3 _color)
		{
			m_LogLines.push_back(std::pair(_log, _color));
		}
	}
}