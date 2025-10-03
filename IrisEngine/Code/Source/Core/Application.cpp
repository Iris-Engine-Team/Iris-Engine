#include "Core/Application.h"

#include "Core/GLFWWindow.h"
#include "Core/Graphics/ContextInitializer.h"
#include "Core/Graphics/Renderer/ForwardRenderer.h"
#include "Core/Graphics/Renderer/DeferredRenderer.h"
#include "Core/Tools/FilePath.h"

#include "Loader/IrisLoader.h"
#include "Resources/ResourceManager.h"
#include "Resources/MetadataManager.h"

#include "SECS/SceneManager.h"
#include "SECS/Scene.h"
#include "SECS/IrisObject.h"

#include "SECS/Systems/RenderSystem.h"
#include "SECS/Systems/PhysicsSystem.h"
#include "SECS/Systems/CameraSystem.h"
#include "SECS/Systems/ScriptSystem.h"

#include "SECS/Components/IComponent.h"
#include "SECS/Components/ModelRenderer.h"
#include "SECS/Components/Transform.h"
#include "SECS/Components/CameraComponent.h"
#include "SECS/Components/LightComponent.h"
#include "SECS/Components/Collider.h"
#include "SECS/Components/SkyboxObject.h"
#include "SECS/Components/ScriptComponent.h"

#include "SECS/Components/ComponentTable.h"

#include "Core/Input/InputManager.h"
#include "Core/Input/InputMappingContext.h"
#include "Core/Input/InputEnum.h"

#include "Core/MultiThread/ThreadManager.h"

#include "Core/Graphics/VulkanRHI/VulkanImage.h"
#include "Core/Graphics/RHI/IPipeline.h"

#include "Core/Input/GLFWInput.h"
#include "Core/Input/InputManager.h"
#include "SECS/Components/PlayerInput.h"

#include "CSharpScripting/ScriptManager.h"
#include "Core/Graphics/RHI/IDevice.h"

namespace Iris
{
	namespace Engine
	{
		Application::Application()
		{
			m_MetadataManager = new MetaDataManager();
		}

		Application::~Application()
		{
			delete m_MetadataManager;
		}

		bool Application::Initialize()
		{
			Debug::Log::OpenFile(FIND_PATH("log.txt"));

			std::filesystem::path path = FIND_PATH("Bin");
			std::filesystem::create_directory(path / "ProjectSettings");

			std::filesystem::path projSettingsPath = FIND_PATH("ProjectSettings.meta");
			bool newCreate = false;
			if (projSettingsPath == "")
			{
				newCreate = true;
				projSettingsPath = "ProjectSettings\\ProjectSettings.meta";
			}

			IMetadata* projSettings = m_MetadataManager->CreateMetadata(projSettingsPath, "ProjectSettings");
			if (newCreate)
			{
				std::vector<std::string> pathRendererType = { "Data", "Renderer Forward" };
				projSettings->Write(pathRendererType, false);
			}

			p_Window = new GLFWWindow;
			p_Window->Initialize();

			p_Window->SetResizeCallBackFunction(std::bind(&Application::ResizeWindow, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
			p_Window->SetInputCallBackFunction(std::bind(&Application::InputCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
			p_Window->SetMouseInputCallBackFunction(std::bind(&Application::MouseInputCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
			//p_Window->SetMousePositionCallBackFunction(std::bind(&Application::MousePositionCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	
			//the other threads are dedicated to the physics manager thread
			size_t nbThread = std::thread::hardware_concurrency() - 1;
			size_t nbThreadForManager = static_cast<size_t>(nbThread * (2.f / 3.f));
			p_ThreadManager = new ThreadManager(nbThreadForManager);

			p_ContextInitializer.Initialize(p_Window);


			std::vector<std::string> pathRendererType = { "Data", "Renderer Forward"};
			if(projSettings->GetData(pathRendererType))
				p_Renderer = new ForwardRenderer;
			else
				p_Renderer = new DeferredRenderer;

			ResourceManager::Init(*p_ContextInitializer.GetRawResourceContext(), p_ContextInitializer.GetRawRenderingContext(), p_Renderer, p_ThreadManager, m_MetadataManager);
			ResourceManager* rm = ResourceManager::GetInstance();
			rm->GetScriptManager()->Init();

			TextureLoadingParameters param{};
			param.LoadInGraphicsData = false;

			TextureLoadingParameters cubeParam{};
			cubeParam.TextureType = RHITextureType::IE_RHI_TEXTURE_CUBEMAP;

			Texture* dTexture = rm->Create<Texture>(FIND_PATH("DefaultTexture.jpg"));
			Texture* dNormal = rm->Create<Texture>(FIND_PATH("DefaultNormal.jpg"));
			rm->Create<Texture>(FIND_PATH("DefaultCubeMapTexture.jpg"), true, cubeParam);

			p_Renderer->Initialize(p_ContextInitializer.GetRawRenderingContext(), p_ContextInitializer.GetRawResourceContext());

			ResourceManager::LoadMetadata();

			p_Window->MaximizeWindow();

			p_InputManager = new InputManager(p_Window);

			LitPBRInputs inputs{};
			inputs.SetAlbedo(dTexture);
			inputs.SetMetallicTexture(dTexture);
			inputs.SetRoughnessTexture(dTexture);
			inputs.SetNormal(dNormal);

			MaterialData dmaterial{};
			dmaterial.ContainerName = "Default Material";
			dmaterial.Inputs = &inputs;
			rm->CreateResourceContainer<Material>(&dmaterial);

			rm->Create<Mesh>(FIND_PATH("DamagedHelmet.gltf"));
			rm->Create<Mesh>(FIND_PATH("FlightHelmet.gltf"));
			rm->Create<Mesh>(FIND_PATH("DefaultCube.obj"));
			rm->Create<Mesh>(FIND_PATH("logo.fbx"));
			rm->Create<Mesh>(FIND_PATH("DefaultSphere.obj"));
			rm->Create<Mesh>(FIND_PATH("DefaultPlane.obj"));
			rm->Create<Mesh>(FIND_PATH("DefaultCapsule.obj"));
			rm->Create<Mesh>(FIND_PATH("DefaultCone.obj"));
			rm->Create<Mesh>(FIND_PATH("DefaultCylinder.obj"));
			rm->Create<Mesh>(FIND_PATH("SimpleSphere.obj"));
			rm->Create<Mesh>(FIND_PATH("SimpleSphere.obj"));

			p_ThreadManager->WaitAllTasks();

			p_SceneManager = new SceneManager();
			p_SceneManager->Init(m_MetadataManager, p_ThreadManager, p_ContextInitializer.GetRawResourceContext(), p_ContextInitializer.GetRawRenderingContext(), PhysicType::IE_TYPE_JOLT);

			p_SceneManager->GetRenderSystem()->GetSkybox()->Init(&p_ContextInitializer, p_Renderer);

			TextureLoadingParameters param2{};
			param2.ColorFormat = RHIColorFormat::IE_RHI_FORMAT_R32G32B32A32_SFLOAT;
			ResourceManager::GetInstance()->Create<Texture>(FIND_PATH("golden_gate.hdr"), true, param2);

			p_SceneManager->GetRenderSystem()->GetSkybox()->SetTexture("dikhololo_night_4k.hdr");

			if (FIND_PATH("Scene0.meta") != "")
			{
				p_SceneManager->AddScene("Scene0");
			}

			Scene* currentScene = p_SceneManager->GetCurrentScene();

			InitEditorContext initializer(
				p_SceneManager,
				p_Renderer,
				p_ContextInitializer.GetRawResourceContext(),
				p_ContextInitializer.GetRawRenderingContext(),
				currentScene,
				nullptr,
				nullptr,
				p_Renderer->GetMainPipeline());

			if (currentScene)
			{
				currentScene->Load(initializer);
			}

			if (p_PlayMode)
			{
				ScriptManager* sm = rm->GetScriptManager();
				sm->InitCSharpSingleton(&p_Time, SingletonType::TIME);
				sm->InitCSharpSingleton(p_SceneManager, SingletonType::SCENE_MANAGER);
				sm->InitCSharpSingleton(p_SceneManager->GetCurrentScene(), SingletonType::SCENE);
				ScriptSystem* ss = p_SceneManager->GetScriptSystem();
				ss->SetScriptInstances();
				ss->SetInstanciateScript(true);
			}
			

			p_PlayerInput = new PlayerInput(p_SceneManager->GetCameraSystem(), p_Window);
			p_PlayerInput->SetInput(p_InputManager);

			p_Renderer->SetFinalRenderPass(p_Renderer->GetMainRenderPass());

			return false;
		}

		bool Application::Terminate()
		{
			delete p_PlayerInput;

			p_SceneManager->Destroy();
			delete p_SceneManager;

			ResourceManager::Terminate();

			p_Renderer->Terminate();
			delete p_Renderer;

			p_ContextInitializer.Terminate();

			p_Window->Terminate();
			delete p_Window;

			delete p_InputManager;

			Debug::Log::CloseFile();

			delete p_ThreadManager;
			return false;
		}

		void Application::Run()
		{
			while (!p_CloseWindow)
			{
				Update(p_Time.GetDeltaTimeD());

				IRenderPass* rp = p_Renderer->GetFinalRenderPass();
				p_Renderer->StartFrame(p_Window, rp);

				Render();

				p_Renderer->EndFrame(p_Window, rp);

				p_Time.UpdateDeltaTime(p_Window);

				if (!p_Editor && p_Window->WindowShouldClose())
				{
					p_CloseWindow = true;
				}
			}
		}

		void Application::ResizeWindow(IWindow* _window, unsigned int _width, unsigned int _height)
		{
			int width = _width, height = _height;

			while (width == 0 || height == 0)
			{
				_window->GetWindowSize(&width, &height);
				_window->WaitEvents();
			}

			p_Window->SetWidth(width);
			p_Window->SetHeight(height);

			p_Renderer->RecreateSwapChain();
		}

		void Application::InputCallback(IWindow* _window, unsigned int _key, unsigned int _scanCode, unsigned int _action, unsigned int _mods)
		{
			_window, _scanCode, _mods;
			GLFWInput input;
			input.SetInput((Engine::IrisInput)input.ConvertToIris(_key));
			if (input.ConvertToGLFW() < (unsigned int)-1)
			{
				switch (_action)
				{
				case GLFW_PRESS:
					input.SetInputState(Engine::InputState::PRESS);
					p_InputManager->AddInputInUpdate(input);
					break;
				case GLFW_REPEAT:
					input.SetInputState(Engine::InputState::REPEAT);
					p_InputManager->AddInputInUpdate(input);
					break;
				case GLFW_RELEASE:
					input.SetInputState(Engine::InputState::RELEASE);
					p_InputManager->AddInputInUpdate(input);
					break;
				}
			}
		}

		void Application::MousePositionCallback(IWindow* _window, double _xPos, double _yPos)
		{
			_window,_xPos,_yPos;
		}

		void Application::MouseInputCallback(IWindow* _window, unsigned int _button, unsigned int _action, unsigned int _mods)
		{
			InputMappingContext* imcEditor = p_InputManager->GetInputMappingContext("IMC_Editor");
			if (imcEditor && imcEditor->IsActive())
				return;

			_window, _mods;
			GLFWInput input;
			input.SetInput((Engine::IrisInput)_button);
			if (input.ConvertToGLFW() < (unsigned int)-1)
			{
				switch (_action)
				{
				case GLFW_PRESS:
					input.SetInputState(Engine::InputState::PRESS);
					p_InputManager->AddInputInUpdate(input);
					break;
				case GLFW_RELEASE:
					input.SetInputState(Engine::InputState::RELEASE);
					p_InputManager->AddInputInUpdate(input);
					break;
				}
			}
		}

		void Application::Update(double _deltaTime)
		{
			p_Window->Update();
			p_InputManager->Update();

			ResourceManager::GetInstance()->GetMaterialSystem()->UpdateMaterials();

			Scene* currentScene = p_SceneManager->GetCurrentScene();

			if (!currentScene)
				return;

			currentScene->Update(_deltaTime);

			ComponentHandle<CameraComponent>* camGameHandle = currentScene->GetCameraSystem()->GetMainCamera();
			CameraComponent* camGame = nullptr;
			if (camGameHandle)
			{
				camGame = camGameHandle->UseComponent();
				if(camGame && camGame->GetLockPositionInWindow()[0] < 0)
					camGame->SetAspect((float)p_Window->GetWidth() / (float)p_Window->GetHeight());
			}

			if (p_PlayMode)
			{
				p_PlayerInput->Update(_deltaTime);
				currentScene->GetScriptSystem()->Update();

				if (camGame)
					camGame->Update(_deltaTime);

				p_Time.IncrementFixedDeltaTime();

				float interpolTimer = static_cast<float>(p_Time.GetFixedDeltaTimeCounterD() / p_Time.GetFixedDeltaTimeD());
				if (1.f >= interpolTimer)
					currentScene->GetPhysicsSystem()->Interpolate(interpolTimer);

				while (p_Time.UpdateFixedDeltaTime())
				{
					currentScene->GetScriptSystem()->FixedUpdate();
					currentScene->FixedUpdate(p_Time.GetFixedDeltaTimeD());
				}
			}
		}

		void Application::Render()
		{
			Scene* currentScene = p_SceneManager->GetCurrentScene();

			if (!currentScene)
				return;

			std::vector<CameraComponent> cams = currentScene->GetCameraSystem()->GetAllCameras();

			RenderSystem* rs = currentScene->GetRenderSystem();
			rs->Update(p_Renderer, p_ContextInitializer.GetRawRenderingContext(), p_ContextInitializer.GetRawResourceContext());

			std::vector<LightComponent> dirlights = rs->GetDirectionalLights();
			std::vector<LightComponent> spotlights = rs->GetSpotLights();
			std::vector<LightComponent> pointlights = rs->GetPointLights();

			std::vector<ShadowMap*> dirShadowMap{};
			std::vector<ShadowMap*> spotShadowMap{};
			std::vector<ShadowMap*> pointShadowMap{};

			p_Renderer->StartRenderingShadows();
			{
				size_t dirlightCount = dirlights.size();
				for (size_t i = 0; i < dirlightCount; ++i)
				{
					if (!dirlights[i].IsEnable() || !dirlights[i].CalculateShadow())
						continue;

					if (i >= IE_MAX_DIRECTIONAL_LIGHTS_SHADOWS)
						continue;
					
					p_Renderer->StartRenderingDirectionalShadow(dirlights[i]);
					{
						rs->RenderShadow(p_Renderer);
					}
					p_Renderer->EndRenderingShadow();

					dirShadowMap.push_back(dirlights[i].GetShadowMap());
				}

				size_t spotlightCount = spotlights.size();
				for (size_t i = 0; i < spotlightCount; ++i)
				{
					if (!spotlights[i].IsEnable() || !spotlights[i].CalculateShadow())
						continue;

					if (i >= IE_MAX_SPOT_LIGHTS_SHADOWS)
						continue;

					p_Renderer->StartRenderingDirectionalShadow(spotlights[i]);
					{
						rs->RenderShadow(p_Renderer);
					}
					p_Renderer->EndRenderingShadow();

					spotShadowMap.push_back(spotlights[i].GetShadowMap());
				}

				size_t pointLightCount = pointlights.size();
				for (size_t i = 0; i < pointLightCount; ++i)
				{
					if (!pointlights[i].IsEnable() || !pointlights[i].CalculateShadow())
						continue;

					if (i >= IE_MAX_POINT_LIGHTS_SHADOWS)
						continue;

					p_Renderer->StartRenderingPointShadow(pointlights[i]);
					{
						rs->RenderShadow(p_Renderer);
					}
					p_Renderer->EndRenderingShadow();

					pointShadowMap.push_back(pointlights[i].GetShadowMap());
				}
			}
			p_Renderer->EndRenderingShadows();

			p_Renderer->UpdateShadowMaps(dirShadowMap, spotShadowMap, pointShadowMap);

			rs->UpdateLightData(p_Renderer);

			SkyboxObject* sb = p_SceneManager->GetRenderSystem()->GetSkybox();

			size_t cameraCount = cams.size();
			for (size_t i = 0; i < cameraCount; ++i)
			{
				if (!cams[i].IsEnable())
					continue;

				cams[i].Bind(*p_Renderer);

				p_Renderer->StartRenderingOpaque(cams[i], sb);
				{
					rs->RenderOpaque(p_Renderer, cams[i], sb);
				}
				p_Renderer->EndRenderingOpaque(cams[i], sb);

				if (cams[i].GetDrawColliders())
				{
					p_Renderer->StartRenderingColliders(cams[i]);
					{
						rs->RenderColliders(p_Renderer);
					}
					p_Renderer->EndRenderingColliders();
				}

				if (cams[i].GetDrawSkybox())
				{
					p_Renderer->StartRenderingSkybox(sb->GetSkyboxDescriptor(), cams[i]);
					{
					}
					p_Renderer->EndRenderingSkybox(cams[i]);
				}

				p_Renderer->StartRenderingTransparent(cams[i], sb);
				{
					rs->RenderTransparent(p_Renderer, cams[i], sb);
				}
				p_Renderer->EndRenderingTransparent(cams[i]);
			}
		}
	}
}