#include "UISubWindow/CameraComponentSubWindow.h"

#include "SECS/Components/CameraComponent.h"

#include "SECS/SceneManager.h"

#include "SECS/Systems/CameraSystem.h"

//UITools
#include "UITools/ImGuiWrapper.h"
#include "UITools/FileExtension.h"

namespace Iris
{
	namespace Editor
	{
		CameraComponentSubWindow::CameraComponentSubWindow(Engine::SceneManager* _sceneManager)
		{
			p_Name = "Camera";
			m_SceneManager = _sceneManager;
		}

		void CameraComponentSubWindow::StartSubWindow()
		{
			ImGuiTreeNodeFlags flags = 0;
			//flags |= ImGuiTreeNodeFlags_OpenOnArrow;
			flags |= ImGuiTreeNodeFlags_Selected;
			flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
			if (m_CurrentCamera)
			{
				if (ImGui::TreeNodeEx(p_Name.c_str(), flags))
				{
					if (!m_CurrentCamera->IsEnable())
						ImGui::BeginDisabled();
					m_SubWindowOpened = true;
				}
			}
		}

		void CameraComponentSubWindow::CloseSubWindow()
		{
			if (m_SubWindowOpened)
			{
				if (!m_CurrentCamera->IsEnable())
					ImGui::EndDisabled();
				ImGui::TreePop();
				m_SubWindowOpened = false;
			}
		}

		bool CameraComponentSubWindow::DrawSubWindow()
		{
			if (m_SubWindowOpened)
			{
				float camNear = m_CurrentCamera->GetNear();
				float camFar = m_CurrentCamera->GetFar();
				float camFOV = m_CurrentCamera->GetFov();
				bool isOrtho = m_CurrentCamera->GetIsOrtho();

				ImGui::DragFloat("Near", &camNear, 0.005f, 0.f, camFar, "%.3f");
				ImGui::DragFloat("Far", &camFar, 0.05f, camNear, FLT_MAX, "%.3f");
				ImGui::DragFloat("FOV", &camFOV, 0.005f, 0.f, PI, "%.3f");
				ImGui::Checkbox("Ortho", &isOrtho);

				m_CurrentCamera->SetNear(camNear);
				m_CurrentCamera->SetFar(camFar);
				m_CurrentCamera->SetFov(camFOV);
				m_CurrentCamera->SetIsOrtho(isOrtho);

				bool mainCam = m_CurrentCamera->IsMainGameCam();
				if(ImGui::Checkbox("Main Camera##IsMainCamera", &mainCam))
				{
					if (mainCam)
						m_SceneManager->GetCameraSystem()->SetMainCamera(m_CurrentCamera);
					else
					{
						Engine::CameraComponent* cam = m_SceneManager->GetCameraSystem()->GetMainCamera()->UseComponent();
						m_SceneManager->GetCameraSystem()->SetMainCamera(cam == m_CurrentCamera ? nullptr : cam);
					}
				}
			}
			return p_ModifiedValue;
		}

		void CameraComponentSubWindow::SetCameraComponent(Engine::CameraComponent* _selectedCamera)
		{
			m_CurrentCamera = _selectedCamera;
		}
	}
}