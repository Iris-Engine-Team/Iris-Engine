#include "UISubWindow/ScriptComponentSubWindow.h"

#include "SECS/Components/ScriptComponent.h"

#include "SECS/SceneManager.h"

//UITools
#include "UITools/ImGuiWrapper.h"
#include "UITools/FileExtension.h"

namespace Iris
{
	namespace Editor
	{
		ScriptComponentSubWindow::ScriptComponentSubWindow()
		{
			p_Name = "Script";
			//m_SceneManager = _sceneManager;
		}

		void ScriptComponentSubWindow::StartSubWindow()
		{
			ImGuiTreeNodeFlags flags = 0;
			//flags |= ImGuiTreeNodeFlags_OpenOnArrow;
			flags |= ImGuiTreeNodeFlags_Selected;
			flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
			if (m_CurrentScript)
			{
				if (ImGui::TreeNodeEx(p_Name.c_str(), flags))
				{
					if (!m_CurrentScript->IsEnable())
						ImGui::BeginDisabled();
					m_SubWindowOpened = true;
				}
			}
		}

		void ScriptComponentSubWindow::CloseSubWindow()
		{
			if (m_SubWindowOpened)
			{
				if (!m_CurrentScript->IsEnable())
					ImGui::EndDisabled();
				ImGui::TreePop();
				m_SubWindowOpened = false;
			}
		}

		bool ScriptComponentSubWindow::DrawSubWindow()
		{
			if (m_SubWindowOpened)
			{
				std::string scriptName = m_CurrentScript->GetScriptName() + " " + m_CurrentScript->GetName();
				ImGui::Text(scriptName.c_str());
			}
			return p_ModifiedValue;
		}

		void ScriptComponentSubWindow::SetScriptComponent(Engine::ScriptComponent* _selectedCamera)
		{
			m_CurrentScript = _selectedCamera;
		}
	}
}