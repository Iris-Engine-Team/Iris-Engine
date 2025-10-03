#include "UIWindow/GameViewportWindow.h"

//RHI
#include "Core/Graphics/Context/RenderingContext.h"
#include "Core/Graphics/RHI/IDevice.h"

//INPUT
#include "Core/Input/InputEnum.h"
#include "Core/Input/IMGUIInput.h"

//SECS
#include "SECS/Systems/CameraSystem.h"
#include "SECS/Components/CameraComponent.h"
#include "SECS/IrisObject.h"
#include "SECS/Components/Transform.h"
#include "SECS/SceneManager.h"

//MATH
#include "Matrix/Matrix3.h"
#include "Vector/Vector3.h"
#include "Quaternion/Quaternion.h"

#include "Core/EditorLayer.h"

#include "UITools/ImGuiWrapper.h"

//STD
#include "tuple"

namespace Iris
{
	namespace Editor
	{

		GameViewportWindow::GameViewportWindow(std::string _hiddenID, Engine::SceneManager* _sceneManager, EditorLayer* _layerEditor)
		{
			p_Name = "Game##" + _hiddenID;

			m_SceneManager = _sceneManager;
			m_LayerEditor = _layerEditor;
		}

		GameViewportWindow::~GameViewportWindow()
		{
			ImGuiEditorWindow::~ImGuiEditorWindow();
		}

		void GameViewportWindow::StartWindow(bool _lightMode)
		{
			ImGuiEditorWindow::StartWindow(_lightMode);

			Engine::ComponentHandle<Engine::CameraComponent>* camHandle = m_SceneManager->GetCameraSystem()->GetMainCamera();
			Engine::CameraComponent* cam = nullptr;
			if (camHandle)
			{
				cam = camHandle->UseComponent();
				if (cam)
					cam->SetEnable(p_WindowOpened);
			}

			if (ImGui::IsWindowFocused(ImGuiFocusedFlags_::ImGuiFocusedFlags_None) && m_LayerEditor->GetEditorPlayMode())
			{
				if (cam)
				{
					Vector2 posLock = Vector2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
					posLock += Vector2((ImGui::GetWindowContentRegionMin().x + ImGui::GetWindowContentRegionMax().x) * 0.5f
						, (ImGui::GetWindowContentRegionMin().y + ImGui::GetWindowContentRegionMax().y) * 0.5f);

					cam->SetLockPositionInWindow(posLock);
				}

				m_LayerEditor->SetGameState(true);
			}
		}

		void GameViewportWindow::CloseWindow()
		{
			ImGuiEditorWindow::CloseWindow();
		}

		bool GameViewportWindow::DrawWindow(unsigned int _cFrame)
		{
			if (p_WindowOpened)
			{
				DrawMenuBar();

				if (m_ViewportIDs.size() > 0)
				{
					ImGui::Image(m_ViewportIDs[_cFrame], ImVec2(p_WindowSize[0], p_WindowSize[1]));
				}
			}
			return p_UnSavedDocument;
		}

		void GameViewportWindow::UpdateSize(Engine::IRenderer* _renderer, EditorRenderer* _editorRenderer, Engine::RenderingContext* _renderingContext, Engine::CameraComponent* _camera)
		{
			if (p_OldWindowSize == p_WindowSize)
				return;

			_renderingContext->GetDevice()->WaitIdle();

			if (p_WindowSize.GetX() <= 0.f)
				p_WindowSize.SetX(1.f);

			if (p_WindowSize.GetY() <= 0.f)
				p_WindowSize.SetY(1.f);

			_camera->SetAspect(p_WindowSize[0] / p_WindowSize[1]);

			_camera->RecreateFramebuffers(_renderer, _renderingContext, static_cast<unsigned int>(p_WindowSize.GetX()), static_cast<unsigned int>(p_WindowSize.GetY()));

			if (m_ViewportIDs.size() > 0)
			{
				ImGuiWrapper::DeleteFramebufferIDs(m_ViewportIDs);
			}

			m_ViewportIDs = ImGuiWrapper::CreateFramebufferIDs(_editorRenderer, _camera->GetCameraFramebuffers());
		}

		void GameViewportWindow::Recreate(Engine::IRenderer* _renderer, EditorRenderer* _editorRenderer, Engine::RenderingContext* _renderingContext, Engine::CameraComponent* _camera)
		{
			_renderingContext->GetDevice()->WaitIdle();

			_camera->SetAspect(p_WindowSize[0] / p_WindowSize[1]);

			_camera->RecreateFramebuffers(_renderer, _renderingContext, static_cast<unsigned int>(p_WindowSize.GetX()), static_cast<unsigned int>(p_WindowSize.GetY()));

			if (m_ViewportIDs.size() > 0)
			{
				ImGuiWrapper::DeleteFramebufferIDs(m_ViewportIDs);
			}

			m_ViewportIDs = ImGuiWrapper::CreateFramebufferIDs(_editorRenderer, _camera->GetCameraFramebuffers());
		}

		void GameViewportWindow::DrawMenuBar()
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMin().x, ImGui::GetCursorPosY()));
				float offsetY = 2.f;
				float maxPosButton = 56.f;
				float offset = 15.f;
				ImVec2 p1 = ImVec2(ImGui::GetCursorScreenPos().x - offset, maxPosButton - offsetY);
				ImVec2 p2 = ImVec2(p1.x + ImGui::GetWindowSize().x + offset, p1.y + 2.0f);
				ImGui::GetWindowDrawList()->AddRectFilled(p1, p2, IM_COL32(0, 0, 0, 255));
				ImGui::EndMenuBar();
			}
		}
	}
}