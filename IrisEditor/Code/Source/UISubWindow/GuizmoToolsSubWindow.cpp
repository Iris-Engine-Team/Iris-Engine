#include "UISubWindow/GuizmoToolsSubWindow.h"

//UIWINDOW
#include "UIWindow/SceneViewportWindow.h"

//UITOOLS
#include "UITools/ImGuiWidgets.h"

namespace Iris
{
	namespace Editor
	{
		GuizmoToolsSubWindow::GuizmoToolsSubWindow(SceneViewportWindow* _sceneWindow)
		{
			m_SceneWindow = _sceneWindow;
			p_Name = "GuizmoTools";
		}

		void GuizmoToolsSubWindow::StartSubWindow()
		{
			ImGuiWindowFlags flags = 0;
			flags |= ImGuiWindowFlags_NoCollapse;
			flags |= ImGuiWindowFlags_NoDecoration;
			flags |= ImGuiWindowFlags_NoDocking;
			flags |= ImGuiWindowFlags_NoResize;
			flags |= ImGuiWindowFlags_NoTitleBar;
			flags |= ImGuiWindowFlags_AlwaysAutoResize;
			flags |= ImGuiWindowFlags_NoMove;
			flags |= ImGuiWindowFlags_NoSavedSettings;
			flags |= ImGuiWindowFlags_NoFocusOnAppearing;
			ImGui::SetNextWindowPos(ImVec2(pos.GetX(), pos.GetY()), ImGuiCond_Always);
			ImGui::SetNextWindowBgAlpha(0.5f);
			if (ImGui::Begin(p_Name.c_str(), NULL, flags))
			{
				m_SubWindowOpened = true;
			}
		}

		void GuizmoToolsSubWindow::CloseSubWindow()
		{
			if (m_SubWindowOpened)
			{
				ImGui::End();
				m_SubWindowOpened = false;
			}
		}

		bool GuizmoToolsSubWindow::DrawSubWindow()
		{
			if (m_SubWindowOpened)
			{
				bool setOperation = false;
				bool setMode = false;
				if (ImGui::RadioButton("Translate", m_CurrentOperation == ImGuizmo::TRANSLATE))
				{
					m_CurrentOperation = ImGuizmo::TRANSLATE;
					setOperation = true;
				}
				else if (ImGui::RadioButton("Rotate", m_CurrentOperation == ImGuizmo::ROTATE))
				{
					m_CurrentOperation = ImGuizmo::ROTATE;
					setOperation = true;
				}
				else if (ImGui::RadioButton("Scale", m_CurrentOperation == ImGuizmo::SCALE))
				{
					m_CurrentOperation = ImGuizmo::SCALE;
					setOperation = true;
				}

				if (ImGui::RadioButton("Local", m_CurrentModicationMode == ImGuizmo::LOCAL))
				{
					m_CurrentModicationMode = ImGuizmo::LOCAL;
					setMode = true;
				}
				else if (ImGui::RadioButton("World", m_CurrentModicationMode == ImGuizmo::WORLD))
				{
					m_CurrentModicationMode = ImGuizmo::WORLD;
					setMode = true;
				}

				if(setOperation)
					m_SceneWindow->SetOperation(m_CurrentOperation);
				if (setMode)
					m_SceneWindow->SetMode(m_CurrentModicationMode);

				std::string string = std::to_string(ImGui::GetIO().Framerate);
				ImGui::Text(string.c_str());
			}
			return p_ModifiedValue;
		}

		void GuizmoToolsSubWindow::ChangeWindowPos(Vector2 _pos)
		{
			pos = _pos;
		}
	}
}