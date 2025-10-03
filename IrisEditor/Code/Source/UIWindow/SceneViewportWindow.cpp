#include "UIWindow/SceneViewportWindow.h"

#include "Core/EditorLayer.h"

//RHI
#include "Core/Graphics/RHI/IDevice.h"

//Input
#include "Core/Input/InputManager.h"
#include "Core/Input/InputEnum.h"
#include "Core/Input/IMGUIInput.h"

//SECS
#include "SECS/Components/CameraComponent.h"
#include "SECS/IrisObject.h"
#include "SECS/Components/Transform.h"
#include "SECS/SceneManager.h"
#include "SECS/Systems/CameraSystem.h"

//SubWindow
#include "UISubWindow/GuizmoToolsSubWindow.h"
#include "UISubWindow/ImGuiSubWindow.h"

//Math
#include "Matrix/Matrix3.h"
#include "Quaternion/Quaternion.h"

//STD
#include "tuple"


#include "UITools/ImGuiWrapper.h"

namespace Iris
{
	namespace Editor
	{
		Quaternion RotationMatrixToQuaternion(Matrix3 m)
		{
			Quaternion q;
			float trace = m[0][0] + m[1][1] + m[2][2];

			if (trace > 0.0f) {
				float s = 0.5f / sqrtf(trace + 1.0f);
				q.SetW(0.25f / s);
				q.SetX((m[2][1] - m[1][2]) * s);
				q.SetY((m[0][2] - m[2][0]) * s);
				q.SetZ((m[1][0] - m[0][1]) * s);
			}
			else {
				if (m[0][0] > m[1][1] && m[0][0] > m[2][2]) {
					float s = 2.0f * sqrtf(1.0f + m[0][0] - m[1][1] - m[2][2]);
					q.SetW((m[2][1] - m[1][2]) / s);
					q.SetX(0.25f * s);
					q.SetY((m[0][1] + m[1][0]) / s);
					q.SetZ((m[0][2] + m[2][0]) / s);
				}
				else if (m[1][1] > m[2][2]) {
					float s = 2.0f * sqrtf(1.0f + m[1][1] - m[0][0] - m[2][2]);
					q.SetW((m[0][2] - m[2][0]) / s);
					q.SetX((m[0][1] + m[1][0]) / s);
					q.SetY(0.25f * s);
					q.SetZ((m[1][2] + m[2][1]) / s);
				}
				else {
					float s = 2.0f * sqrtf(1.0f + m[2][2] - m[0][0] - m[1][1]);
					q.SetW((m[1][0] - m[0][1]) / s);
					q.SetX((m[0][2] + m[2][0]) / s);
					q.SetY((m[1][2] + m[2][1]) / s);
					q.SetZ(0.25f * s);
				}
			}
			return q;
		}

		float RadToDeg(float _r)
		{
			return _r * (180.0f / PI);
		}

		SceneViewportWindow::SceneViewportWindow(std::string _hiddenID, Engine::InputManager* _inputManager, Engine::InitEditorContext* _initializer, EditorLayer* _layerEditor)
		{
			p_Name = "Scene Viewport##" + _hiddenID;
			m_LayerEditor = _layerEditor;
			m_InputManager = _inputManager;
			m_Initializer = _initializer;
			m_SubWindow = new GuizmoToolsSubWindow(this);
		}

		SceneViewportWindow::~SceneViewportWindow()
		{
			ImGuiEditorWindow::~ImGuiEditorWindow();
			m_InputManager = nullptr;
			m_CurrentObject = nullptr;
			delete(m_SubWindow);
		}

		void SceneViewportWindow::StartWindow(bool _lightMode)
		{
			ImGuiEditorWindow::StartWindow(_lightMode);
			if (m_SubWindowPos != Vector2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y))
			{
				m_SubWindowPos = Vector2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y);
				dynamic_cast<GuizmoToolsSubWindow*>(m_SubWindow)->ChangeWindowPos(m_SubWindowPos);
			}
			
			Engine::ComponentHandle<Engine::CameraComponent>* camHandle = m_Initializer->SceneManager->GetCameraSystem()->GetEditorCamera();
			Engine::CameraComponent* cam = nullptr;
			if (camHandle)
			{
				cam = camHandle->UseComponent();
				cam->SetEnable(p_WindowOpened);
			}

			if (ImGui::IsWindowHovered())
			{
				if (cam)
				{
					Vector2 posLock = Vector2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
					posLock += Vector2((ImGui::GetWindowContentRegionMin().x + ImGui::GetWindowContentRegionMax().x) * 0.5f, (ImGui::GetWindowContentRegionMin().y + ImGui::GetWindowContentRegionMax().y) * 0.5f);
					cam->SetLockPositionInWindow(posLock);
				}
				m_LayerEditor->SetGameState(false);
			}
		}

		void SceneViewportWindow::CloseWindow()
		{
			ImGuiEditorWindow::CloseWindow();
		}

		bool SceneViewportWindow::DrawWindow(unsigned int _cFrame)
		{
			if (p_WindowOpened)
			{
				DrawMenuBar();

				if (m_ViewportIDs.size() > 0)
				{
					ImGui::Image(m_ViewportIDs[_cFrame], ImVec2(p_WindowSize[0], p_WindowSize[1]));
				}

				m_SubWindow->StartSubWindow();
				m_SubWindow->DrawSubWindow();
				m_SubWindow->CloseSubWindow();

				if (m_CurrentObject)
				{
					DrawGuizmo();
				}

				ImGuiSceneCallback();
			}
			return p_UnSavedDocument;
		}

		void SceneViewportWindow::DrawMenuBar()
		{
			if (ImGui::BeginMenuBar())
			{
				DrawCameraParameters();

				ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMin().x, ImGui::GetCursorPosY()));
				float offsetY = 2.f;
				float maxPosButton = ImGui::GetItemRectMax().y;
				float offset = 15.f;
				ImVec2 p1 = ImVec2(ImGui::GetCursorScreenPos().x - offset, maxPosButton - offsetY);
				ImVec2 p2 = ImVec2(p1.x + ImGui::GetWindowSize().x + offset, p1.y + 2.0f);
				ImGui::GetWindowDrawList()->AddRectFilled(p1, p2, IM_COL32(0, 0, 0, 255));
				ImGui::EndMenuBar();
			}
			m_MenuOffset = 0;
		}

		void SceneViewportWindow::DrawCameraParameters()
		{
			float spacing = ImGui::GetStyle().ItemSpacing.x;
			float width = ImGui::CalcTextSize("Camera").x + ImGui::GetStyle().FramePadding.x * 2.0f;

			ImVec2 newPos = ImVec2(ImGui::GetWindowContentRegionMin().x + ImGui::GetWindowWidth() - (width + spacing) - m_MenuOffset, ImGui::GetCursorPosY());
			ImGui::SetCursorPos(newPos);

			m_MenuOffset += (width + spacing);

			if (ImGui::BeginMenu("Camera##Scene"))
			{
				Engine::CameraComponent* cam = m_Initializer->SceneManager->GetCameraSystem()->GetEditorCamera()->UseComponent();

				float camNear = cam->GetNear();
				float camFar = cam->GetFar();
				float camFOV = cam->GetFov();
				bool isOrtho = cam->GetIsOrtho();
				bool drawCollider = cam->GetDrawColliders();
				bool drawGrid = cam->GetDrawGrid();
				bool drawSkybox = cam->GetDrawSkybox();

				ImGui::DragFloat("Near", &camNear, 0.005f, 0.f, camFar, "%.3f");
				ImGui::DragFloat("Far", &camFar, 0.05f, camNear, FLT_MAX, "%.3f");
				ImGui::DragFloat("FOV", &camFOV, 0.005f, 0.f, PI, "%.3f");
				ImGui::Checkbox("Ortho", &isOrtho);
				ImGui::Checkbox("Draw Collider", &drawCollider);
				ImGui::Checkbox("Draw Grid", &drawGrid);
				ImGui::Checkbox("Draw Skybox", &drawSkybox);

				cam->SetNear(camNear);
				cam->SetFar(camFar);
				cam->SetFov(camFOV);
				cam->SetIsOrtho(isOrtho);
				cam->SetDrawColliders(drawCollider);
				cam->SetDrawGrid(drawGrid);
				cam->SetDrawSkybox(drawSkybox);

				ImGui::EndMenu();
			}
		}

		void SceneViewportWindow::DrawGuizmo()
		{
			ImGuizmo::SetDrawlist();
			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			Matrix4 newTRS = m_CurrentObject->GetTransform()->GetGlobalTRS();
			Engine::CameraComponent* camera = m_Initializer->SceneManager->GetCameraSystem()->GetEditorCamera()->UseComponent();
			ImGuizmo::Manipulate(camera->GetViewMatrix().GetData(), camera->GetProjectionMatrix().GetData(), m_CurrentOperation, m_CurrentModicationMode, newTRS.GetData(), NULL, NULL, NULL, NULL);

			Vector3 position = Vector3(newTRS[3].GetX(), newTRS[3].GetY(), newTRS[3].GetZ());

			Vector3 xAxis = Vector3(newTRS[0][0], newTRS[0][1], newTRS[0][2]);
			Vector3 yAxis = Vector3(newTRS[1][0], newTRS[1][1], newTRS[1][2]);
			Vector3 zAxis = Vector3(newTRS[2][0], newTRS[2][1], newTRS[2][2]);

			Vector3 scaling = Vector3(xAxis.Norm(), yAxis.Norm(), zAxis.Norm());

			xAxis = xAxis.Normalize();
			yAxis = yAxis.Normalize();
			zAxis = zAxis.Normalize();

			Matrix3 rot = Matrix3(xAxis, yAxis, zAxis);

			Quaternion rotation = RotationMatrixToQuaternion(rot);

			switch (m_CurrentOperation)
			{
			case ImGuizmo::OPERATION::TRANSLATE:
				m_CurrentObject->GetTransform()->SetGlobalPosition(position);
				break;
			case ImGuizmo::OPERATION::ROTATE:
				m_CurrentObject->GetTransform()->SetGlobalRotation(rotation);
				break;
			case ImGuizmo::OPERATION::SCALE:
				m_CurrentObject->GetTransform()->SetGlobalScale(scaling);
				break;
			default:
				break;
			}
		}

		void SceneViewportWindow::ImGuiSceneCallback()
		{
			for (unsigned int i = (unsigned int)Engine::IrisInput::IE_MOUSEBUTTON_1; i <= (unsigned int)Engine::IrisInput::IE_MOUSEBUTTON_8; i++)
			{
				IMGUIInput input;
				input.SetInput((Engine::IrisInput)i);
				if (input.ConvertToImGui() < (unsigned int)-1)
				{
					if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked((ImGuiKey)input.ConvertToImGui()))
					{
						input.SetInputState(Engine::InputState::PRESS);
						m_InputManager->AddInputInUpdate(input);
					}
					else if (ImGui::IsMouseDown((ImGuiKey)input.ConvertToImGui()))
					{
						input.SetInputState(Engine::InputState::REPEAT);
						m_InputManager->AddInputInUpdate(input);
					}
					else if (ImGui::IsMouseReleased((ImGuiKey)input.ConvertToImGui()))
					{
						input.SetInputState(Engine::InputState::RELEASE);
						m_InputManager->AddInputInUpdate(input);
					}
				}
			}

			for (unsigned int i = (unsigned int)Engine::IrisInput::IE_KEY_A; i <= (unsigned int)Engine::IrisInput::IE_KEY_RIGHT_SUPER; i++)
			{
				IMGUIInput input;
				input.SetInput((Engine::IrisInput)i);
				if (input.ConvertToImGui() < (unsigned int)-1)
				{
					if (ImGui::IsWindowHovered() && ImGui::IsKeyPressed((ImGuiKey)input.ConvertToImGui()))
					{
						input.SetInputState(Engine::InputState::PRESS);
						m_InputManager->AddInputInUpdate(input);
					}
					else if (ImGui::IsKeyDown((ImGuiKey)input.ConvertToImGui()))
					{
						input.SetInputState(Engine::InputState::REPEAT);
						m_InputManager->AddInputInUpdate(input);
					}
					else if (ImGui::IsKeyReleased((ImGuiKey)input.ConvertToImGui()))
					{
						input.SetInputState(Engine::InputState::RELEASE);
						m_InputManager->AddInputInUpdate(input);
					}
				}
			}
		}
		void SceneViewportWindow::SetCurrentObject(Engine::IrisObject* _selectedObject)
		{
			m_CurrentObject = _selectedObject;
		}

		void SceneViewportWindow::UpdateSize(Engine::IRenderer* _renderer, EditorRenderer* _editorRenderer, Engine::RenderingContext* _renderingContext, Engine::CameraComponent* _camera)
		{
			if (p_OldWindowSize == p_WindowSize)
				return;

			_renderingContext->GetDevice()->WaitIdle();

			if (p_WindowSize.GetX() <= 0.f)
				p_WindowSize.SetX(1.f);

			if (p_WindowSize.GetY() <= 0.f)
				p_WindowSize.SetY(1.f);

			_camera->SetAspect(p_WindowSize[0]/ p_WindowSize[1]);

			_camera->RecreateFramebuffers(_renderer, _renderingContext, static_cast<unsigned int>(p_WindowSize[0]), static_cast<unsigned int>(p_WindowSize[1]));

			if (m_ViewportIDs.size() > 0)
			{
				ImGuiWrapper::DeleteFramebufferIDs(m_ViewportIDs);
			}

			m_ViewportIDs = ImGuiWrapper::CreateFramebufferIDs(_editorRenderer, _camera->GetCameraFramebuffers());
		}

		ImGuizmo::MODE SceneViewportWindow::GetMode()
		{
			return m_CurrentModicationMode;
		}

		ImGuizmo::OPERATION SceneViewportWindow::GetOperation()
		{
			return m_CurrentOperation;
		}

		void SceneViewportWindow::SetMode(ImGuizmo::MODE _mode)
		{
			m_CurrentModicationMode = _mode;
		}

		void SceneViewportWindow::SetOperation(ImGuizmo::OPERATION _operation)
		{
			m_CurrentOperation = _operation;
		}
	}
}