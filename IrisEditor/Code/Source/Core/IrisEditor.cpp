#include "Core/IrisEditor.h"

#include "Resources/ResourceManager.h"

//SECS
#include "SECS/SceneManager.h"
#include "SECS/Scene.h"
#include "SECS/Components/CameraComponent.h"
#include "SECS/Components/Collider.h"
#include "SECS/Components/RigidBody.h"
#include "SECS/Components/ModelRenderer.h"
#include "SECS/Components/SkyboxObject.h"
#include "SECS/Systems/CameraSystem.h"
#include "SECS/Systems/RenderSystem.h"

//RHI
#include "Core/Graphics/RHI/ISwapChain.h"
#include "Core/Graphics/RHI/IPipeline.h"
#include "Core/Graphics/RHI/IcommandBuffer.h"
#include "Core/Graphics/RHI/IDevice.h"

//RENDERER
#include "Core/Graphics/Renderer/IRenderer.h"

//INPUT
#include "Core/Input/IInput.h"
#include "Core/Input/InputManager.h"
#include "Core/Input/InputMappingContext.h"
#include "Core/Input/InputEnum.h"
#include "Core/Input/InputAction.h"

//STD
#include <functional>

//GLFW
#include "Core/GLFWWindow.h"

Vector2 Iris::Editor::IrisEditor::s_LastPosEditor = Vector2::s_Zero;

namespace Iris
{
	namespace Editor
	{

		IrisEditor::IrisEditor()
		{
		}

		IrisEditor::~IrisEditor()
		{
		}

		bool IrisEditor::Initialize()
		{
			p_PlayMode = false;

			if (!std::filesystem::exists(FIND_PATH("Bin").string() + "\\nethost.dll"))
				std::filesystem::copy_file(FIND_PATH("nethost.dll"), FIND_PATH("Bin").string() + "\\nethost.dll");
			if (!std::filesystem::exists(FIND_PATH("Bin").string() + "\\CSharp.runtimeconfig.json"))
				std::filesystem::copy_file(FIND_PATH("CSharp.runtimeconfig.json"), FIND_PATH("Bin").string() + "\\CSharp.runtimeconfig.json");

			Engine::Application::Initialize();

			p_Editor = true;

			p_Window->SetDropCallBackFunction(std::bind(&Iris::Editor::IrisEditor::DropCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

			m_EditorRenderer.Initialize(p_ContextInitializer.GetRawRenderingContext());
						
			InitEditorCamera();

			InitCameraInput();

			Engine::InitEditorContext* initializer = new Engine::InitEditorContext
			(
				p_SceneManager,
				p_Renderer, 
				p_ContextInitializer.GetRawResourceContext(), 
				p_ContextInitializer.GetRawRenderingContext(), 
				p_SceneManager->GetCurrentScene(),
				Engine::ResourceManager::GetInstance()->Get<Engine::Mesh>("DefaultCube.obj"),
				nullptr,
				p_Renderer->GetMainPipeline()
			);

			IELayer.Initialize(p_Window, &p_ContextInitializer, &m_EditorRenderer, p_InputManager, initializer, this, p_ThreadManager);

			p_ContextInitializer.GetRawRenderingContext()->GetSwapChain()->RecreateSwapChain(p_Window, p_ContextInitializer.GetRawRenderingContext()->GetDevice(), m_EditorRenderer.GetRenderPass());

			p_Renderer->SetFinalRenderPass(m_EditorRenderer.GetRenderPass());

			return true;
		}

		void IrisEditor::InitEditorCamera()
		{
			m_MouseSensitivity = 0.25f;

			if (!p_SceneManager->GetCurrentScene())
				return;

			if (m_CameraEditor)
				delete m_CameraEditor;

			std::vector<Engine::CameraComponent>& cams = p_SceneManager->GetCameraSystem()->GetAllCameras();

			for (Engine::CameraComponent& cam : cams)
			{
				if (cam.IsEditorCam())
				{
					m_CameraEditor = new Engine::ComponentHandle<Engine::CameraComponent>(&cam);
					p_SceneManager->GetCameraSystem()->SetEditorCamera(&cam);

					m_BaseCamSpeed = cam.GetSpeed();

					return;
				}
			}

			Engine::IrisObject* camObject = p_SceneManager->GetCurrentScene()->Instantiate(Vector3(0.f, 1.f, 5.f), Vector3(0.f, 180.f, 0.f), Vector3::s_One, nullptr, "Editor Camera");
			camObject->SetShowInHierarchy(false);
			Engine::CameraComponent* cam = camObject->AddComponent<Engine::CameraComponent>();
			cam->Init(*p_Renderer, *p_ContextInitializer.GetRawResourceContext(), *p_ContextInitializer.GetRawRenderingContext(), Vector3(0.f, 1.f, 5.f), 45.f * Deg2Rad
				, static_cast<float>(p_Window->GetWidth()) / static_cast<float>(p_Window->GetHeight()), 0.1f, 100.f, false);

			m_BaseCamSpeed = cam->GetSpeed();

			m_CameraEditor = new Engine::ComponentHandle<Engine::CameraComponent>(cam);

			p_SceneManager->GetCameraSystem()->SetEditorCamera(cam);
		}

		void IrisEditor::InitCameraInput()
		{
			if (!p_SceneManager->GetCurrentScene())
				return;

			std::function<void()> releasedFunction = [this]() {this->CamResetSpeed(); };
			std::function<void()> repeatedFunction = [this]() {this->CamSpeedUp(); };

			Engine::InputMappingContext* imcEditor = new Engine::InputMappingContext();

			Engine::InputAction* action = new Engine::InputAction();
			std::function<void()> function = [this]() { this->EditorCameraForward(); };
			action->AddRepeatedAction(function);
			action->AddRepeatedAction(repeatedFunction);
			action->AddReleasedAction(releasedFunction);
			imcEditor->AddActionToInput(action, Engine::IrisInput::IE_KEY_W);

			action = new Engine::InputAction();
			function = [this]() { this->EditorCameraBackward(); };
			action->AddRepeatedAction(function);
			action->AddRepeatedAction(repeatedFunction);
			action->AddReleasedAction(releasedFunction);
			function = [this]() {this->CtrlAndSAction(); };
			action->AddPressedAction(function);
			imcEditor->AddActionToInput(action, Engine::IrisInput::IE_KEY_S);

			action = new Engine::InputAction();
			function = [this]() { this->EditorCameraLeft(); };
			action->AddRepeatedAction(function);
			action->AddRepeatedAction(repeatedFunction);
			action->AddReleasedAction(releasedFunction);
			imcEditor->AddActionToInput(action, Engine::IrisInput::IE_KEY_A);

			action = new Engine::InputAction();
			function = [this]() { this->EditorCameraRight(); };
			action->AddRepeatedAction(function);
			action->AddRepeatedAction(repeatedFunction);
			action->AddReleasedAction(releasedFunction);
			function = [this]() { this->CtrlAndDAction(); };
			action->AddPressedAction(function);
			imcEditor->AddActionToInput(action, Engine::IrisInput::IE_KEY_D);

			action = new Engine::InputAction();
			function = [this]() { this->EditorCameraUp(); };
			action->AddRepeatedAction(function);
			action->AddRepeatedAction(repeatedFunction);
			action->AddReleasedAction(releasedFunction);
			imcEditor->AddActionToInput(action, Engine::IrisInput::IE_KEY_E);

			action = new Engine::InputAction();
			function = [this]() { this->EditorCameraUp(); };
			action->AddRepeatedAction(function);
			action->AddRepeatedAction(repeatedFunction);
			action->AddReleasedAction(releasedFunction);
			imcEditor->AddActionToInput(action, Engine::IrisInput::IE_KEY_SPACE);

			action = new Engine::InputAction();
			function = [this]() { this->EditorCameraDown(); };
			action->AddRepeatedAction(function);
			action->AddRepeatedAction(repeatedFunction);
			action->AddReleasedAction(releasedFunction);
			imcEditor->AddActionToInput(action, Engine::IrisInput::IE_KEY_Q);

			action = new Engine::InputAction();
			std::function<void(Vector2 _mousePos)> startMouseCapture = [this](Vector2 _mousePos) { this->StartCamRotate(_mousePos); };
			action->AddPressedAction(startMouseCapture);
			std::function<void(Vector2 _mousePos)> vectorFunction = [this](Vector2 _mousePos) {this->CamRotate(_mousePos); };
			action->AddRepeatedAction(vectorFunction);
			std::function<void(Vector2 _mousePos)> endMouseCapture = [this](Vector2 _mousePos) { this->TerminateCamRotate(_mousePos); };
			action->AddReleasedAction(endMouseCapture);
			imcEditor->AddActionToInput(action, Engine::IrisInput::IE_MOUSEBUTTON_RIGHT);

			action = new Engine::InputAction();
			function = [this]() {this->CaptureCtrlKey(); };
			action->AddPressedAction(function);
			imcEditor->AddActionToInput(action, Engine::IrisInput::IE_KEY_LEFT_CONTROL);
			function = [this]() {this->ReleaseCtrlKey(); };
			action->AddReleasedAction(function);
			imcEditor->AddActionToInput(action, Engine::IrisInput::IE_KEY_LEFT_CONTROL);

			action = new Engine::InputAction();
			function = [this]() {this->CaptureCtrlKey(); };
			action->AddPressedAction(function);
			imcEditor->AddActionToInput(action, Engine::IrisInput::IE_KEY_RIGHT_CONTROL);
			function = [this]() {this->ReleaseCtrlKey(); };
			action->AddReleasedAction(function);
			imcEditor->AddActionToInput(action, Engine::IrisInput::IE_KEY_RIGHT_CONTROL);

			p_InputManager->AddInputMappingContext("IMC_Editor", imcEditor);


			Engine::InputMappingContext* imcEditorGame = new Engine::InputMappingContext();

			action = new Engine::InputAction();
			function = [this]()
				{
					Engine::InputMappingContext* imcGame = p_InputManager->GetInputMappingContext("IMC_Game");
					if (imcGame && imcGame->IsActive())
					{
						ImGui::SetWindowFocus(nullptr);

						imcGame->SetActive(false);

						p_InputManager->GetInputMappingContext("IMC_Editor")->SetActive(true);
						p_Window->SetShowCursor(true);
					}
				};

			action->AddReleasedAction(function);
			imcEditorGame->AddActionToInput(action, Engine::IrisInput::IE_KEY_ESCAPE);
			p_InputManager->AddInputMappingContext("IMC_EditorGame", imcEditorGame);


			Engine::InputMappingContext* imcGame = p_InputManager->GetInputMappingContext("IMC_Game");
			if (imcGame && imcGame->IsActive())
			{
				imcGame->SetActive(false);
			}
		}

		bool IrisEditor::Terminate()
		{
			IELayer.Terminate(&p_ContextInitializer);
			m_EditorRenderer.Terminate();

			delete m_CameraEditor;
			return  Engine::Application::Terminate();
		}

		void IrisEditor::Update(double _deltaTime)
		{
			p_PlayMode = IELayer.GetEditorPlayMode();

			if (m_MustReloadSkybox)
			{
				m_MustReloadSkybox = false;

				p_ContextInitializer.GetRawResourceContext()->GetDevice()->WaitIdle();
				p_SceneManager->GetRenderSystem()->GetSkybox()->SetTexture(m_TextureToSet);
			}

			if (m_MustReloadScene)
			{
				m_MustReloadScene = false;

				p_ContextInitializer.GetRawResourceContext()->GetDevice()->WaitIdle();

				Engine::InitEditorContext initializer = Engine::InitEditorContext
				(
					p_SceneManager,
					p_Renderer,
					p_ContextInitializer.GetRawResourceContext(),
					p_ContextInitializer.GetRawRenderingContext(),
					p_SceneManager->GetCurrentScene(),
					Engine::ResourceManager::GetInstance()->Get<Engine::Mesh>("DefaultCube.obj"),
					nullptr,
					p_Renderer->GetMainPipeline()
				);

				p_SceneManager->ReLoadCurrentScene(&initializer);

				Engine::ComponentHandle<Engine::CameraComponent>* gameCamHandle = p_SceneManager->GetCameraSystem()->GetMainCamera();
				if (gameCamHandle)
				{
					Engine::CameraComponent* gameCamera = gameCamHandle->UseComponent();
					IELayer.gameViewport->Recreate(p_Renderer, &m_EditorRenderer, p_ContextInitializer.GetRawRenderingContext(), gameCamera);
				}
			}

			Engine::Application::Update(_deltaTime);

			if (!p_SceneManager->GetCurrentScene())
				return;
		
			Vector2 viewportSize = IELayer.mainViewport->GetWindowSize();
			Engine::CameraComponent* camEditor = m_CameraEditor->UseComponent();
			camEditor->SetAspect(viewportSize[0] / viewportSize[1]);
			camEditor->Update(_deltaTime);

			m_CamSpeedModified = false;

		
			if (!p_PlayMode)
			{
				Engine::IrisObject* selectedObject = p_SceneManager->GetCurrentScene()->GetSelectedObject();
				if (!selectedObject)
					return;

				if (!selectedObject->IsActive())
					return;

				Engine::RigidBody* rb = selectedObject->GetComponent<Engine::RigidBody>();
				if (!rb)
					return;

				Engine::Collider* col = selectedObject->GetComponent<Engine::Collider>();
				if (col)
				{
					Engine::ITransform* t = selectedObject->GetTransform();
					Vector3 pos = col->GetPhysicPositionWithOffSet(t);

					rb->SetTargetPosition(pos);
					rb->SetRotation(t->GetGlobalRotation());
					rb->SetPhysiquePosition();
				}
			}

		}

		void IrisEditor::Render()
		{
			Engine::Scene* currentScene = p_SceneManager->GetCurrentScene();

			if (currentScene)
			{
				Engine::CameraSystem* camSys = currentScene->GetCameraSystem();

				if (!camSys)
					return;

				Engine::ComponentHandle<Engine::CameraComponent>* editorCamHandle = camSys->GetEditorCamera();
				Engine::ComponentHandle<Engine::CameraComponent>* gameCamHandle = camSys->GetMainCamera();

				if (editorCamHandle)
				{
					Engine::CameraComponent* editCamera = editorCamHandle->UseComponent();
					IELayer.mainViewport->UpdateSize(p_Renderer, &m_EditorRenderer, p_ContextInitializer.GetRawRenderingContext(), editCamera);
				}

				if (gameCamHandle)
				{
					Engine::CameraComponent* gameCamera = gameCamHandle->UseComponent();
					IELayer.gameViewport->UpdateSize(p_Renderer, &m_EditorRenderer, p_ContextInitializer.GetRawRenderingContext(), gameCamera);
				}

				Application::Render();
			}

			p_Renderer->GetCurrentCommandBuffer()->BeginDebugLabel(p_ContextInitializer.GetRawRenderingContext()->GetDevice(), "Editor Rendering", Vector3(0.588f, 1.f, 0.98f));
			p_Renderer->StartRenderPass(m_EditorRenderer.GetRenderPass(), p_ContextInitializer.GetRawRenderingContext()->GetSwapChain()->GetFramebuffer(p_ContextInitializer.GetRawRenderingContext()->GetSwapChain()->GetCurrentSwapChainImage()), p_ContextInitializer.GetRawRenderingContext()->GetSwapChain()->GetFrameWidth(), p_ContextInitializer.GetRawRenderingContext()->GetSwapChain()->GetFrameHeight());

			IELayer.StartEditorFrame(p_ContextInitializer.GetRawRenderingContext()->GetSwapChain()->GetCurrentFrame());
			IELayer.EndEditorFrame(p_ContextInitializer.GetRawRenderingContext());

			p_Renderer->GetCurrentCommandBuffer()->EndDebugLabel(p_ContextInitializer.GetRawRenderingContext()->GetDevice());
			p_Renderer->EndRenderPass();
		}

		void IrisEditor::Run()
		{
			Application::Run();
		}

		void IrisEditor::DropCallback(Engine::IWindow* _window, unsigned int _count, const std::vector<std::string>& _paths)
		{
			_count;
			_window;
			IELayer.AddNewFiles(_paths);
		}

		void IrisEditor::EditorCameraForward()
		{
			if(m_CamIsUsed)
				m_CameraEditor->UseComponent()->MoveCallback(Vector3::s_Forward);
		}
		void IrisEditor::EditorCameraBackward()
		{
			if (m_CamIsUsed)
				m_CameraEditor->UseComponent()->MoveCallback(Vector3::s_Back);
		}
		void IrisEditor::EditorCameraLeft()
		{
			if (m_CamIsUsed)
				m_CameraEditor->UseComponent()->MoveCallback(Vector3::s_Left);
		}
		void IrisEditor::EditorCameraRight()
		{
			if (m_CamIsUsed)
				m_CameraEditor->UseComponent()->MoveCallback(Vector3::s_Right);
		}
		void IrisEditor::EditorCameraUp()
		{
			if (m_CamIsUsed)
				m_CameraEditor->UseComponent()->MoveCallback(Vector3::s_Up);
		}
		void IrisEditor::EditorCameraDown()
		{
			if (m_CamIsUsed)
				m_CameraEditor->UseComponent()->MoveCallback(Vector3::s_Down);
		}
		void IrisEditor::CamSpeedUp()
		{
			if (!m_CamSpeedModified && m_CamIsUsed)
			{
				m_CameraEditor->UseComponent()->SetSpeed(m_CameraEditor->UseComponent()->GetSpeed() + 0.1f);
				m_CamSpeedModified = true;
			}
		}

		void IrisEditor::CamResetSpeed()
		{
			m_CameraEditor->UseComponent()->SetSpeed(m_BaseCamSpeed);
		}

		void IrisEditor::StartCamRotate(Vector2 _mousePos)
		{
			_mousePos = Vector2(p_Window->GetWidth() / 2.0f, p_Window->GetHeight() / 2.0f);

			Engine::ComponentHandle<Engine::CameraComponent>* camHandle = p_SceneManager->GetCameraSystem()->GetEditorCamera();
			if (camHandle)
			{
				Engine::CameraComponent* cam = p_SceneManager->GetCameraSystem()->GetEditorCamera()->UseComponent();
				if (cam)
				{
					Vector2 lockpos = cam->GetLockPositionInWindow();
					if(lockpos[0] > 0 && lockpos[1] > 0.f)
						_mousePos = lockpos;
				}
			}

			IrisEditor::s_LastPosEditor = _mousePos ;
			p_Window->SetCursorPosition(_mousePos);
			p_Window->SetShowCursor(false);
			m_CamIsUsed = true;
		}

		void IrisEditor::CamRotate(Vector2 _mousePos)
		{
			if (m_CamIsUsed)
			{
				_mousePos = p_Window->GetCursorPosition();

				Vector2 distance = (IrisEditor::s_LastPosEditor - _mousePos);

				if (distance.Norm() > 1.f)
				{
					Engine::CameraComponent* cam = m_CameraEditor->UseComponent();

					Vector2 curRot = cam->GetCurrentRotate();

					curRot.SetY(curRot[1] + distance[0] * m_MouseSensitivity);
					curRot.SetX(curRot[0] - distance[1] * m_MouseSensitivity);

					if (curRot[0] > 80.f)
						curRot.SetX(80.f);

					if (curRot[0] < -80.f)
						curRot.SetX(-80.f);

					m_CameraEditor->UseComponent()->RotateCallback(curRot);
				}

				p_Window->SetCursorPosition(Vector2(IrisEditor::s_LastPosEditor.GetX(), IrisEditor::s_LastPosEditor.GetY()));
			}
		}

		void IrisEditor::TerminateCamRotate(Vector2 _mousePos)
		{
			if (m_CamIsUsed)
			{
				_mousePos;
				m_CamIsUsed = false;
				p_Window->SetShowCursor(true);
				p_Window->SetCursorPosition(Vector2(p_Window->GetWidth() * 0.5f, p_Window->GetHeight() * 0.5f));
			}
		}

		void IrisEditor::CaptureCtrlKey()
		{
			m_CtrlKeyPressed = true;
		}
		void IrisEditor::ReleaseCtrlKey()
		{
			m_CtrlKeyPressed = false;
		}

		void IrisEditor::CtrlAndSAction()
		{
			if(m_CtrlKeyPressed && !m_CamIsUsed)
				IELayer.SaveScene();
		}

		void IrisEditor::CtrlAndDAction()
		{
			if (m_CtrlKeyPressed && !m_CamIsUsed)
				IELayer.DuplicateObject();
		}

		void IrisEditor::SetGameState(bool _isGameActive)
		{
			Engine::InputMappingContext* imcE = p_InputManager->GetInputMappingContext("IMC_Editor");
			Engine::InputMappingContext* imcG = p_InputManager->GetInputMappingContext("IMC_Game");

			if (imcE)
				imcE->SetActive(!_isGameActive);

			if (imcG)
				imcG->SetActive(_isGameActive);
		}
	}
}