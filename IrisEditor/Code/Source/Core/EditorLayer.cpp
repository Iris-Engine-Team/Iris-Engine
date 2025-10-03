#include "Core/EditorLayer.h"

// STD
#include <cstdio>
#include <filesystem>

//RHI
#include "Core/Graphics/VulkanRHI/VulkanDevice.h"
#include "Core/Graphics/VulkanRHI/VulkanDescriptorAllocator.h"
#include "Core/Graphics/VulkanRHI/VulkanRenderPass.h"
#include "Core/Graphics/VulkanRHI/VulkanSwapChain.h"
#include "Core/Graphics/VulkanRHI/VulkanCommandBuffer.h"
#include "Core/Graphics/VulkanRHI/VulkanSampler.h"
#include "Core/Graphics/VulkanRHI/VulkanFramebuffer.h"
#include "Core/Graphics/VulkanRHI/VulkanDescriptor.h"

//Renderer
#include "Core/Graphics/Renderer/IRenderer.h"
#include "Core/Graphics/Renderer/EditorRenderer.h"

//Context
#include "Core/Graphics/Context/RenderingContext.h"
#include "Core/Graphics/Context/ResourceContext.h"
#include "Core/Graphics/ContextInitializer.h"
#include "Core/Graphics/Context/EditorContext.h"

#include "Resources/ResourceManager.h"

//SECS
#include "SECS/Scene.h"
#include "SECS/Components/IComponent.h"
#include "SECS/SceneManager.h"
#include "SECS/Components/LightComponent.h"
#include "SECS/Systems/RenderSystem.h"
#include "SECS/Components/SkyboxObject.h"
#include "SECS/Systems/ScriptSystem.h"

//Scripting
#include "CSharpScripting/ScriptManager.h"

//INPUT
#include "Core/Input/InputManager.h"

//GLFW
#include "Core/GLFWWindow.h"

//WINDOW
#include "UIWindow/InspectorWindow.h"
#include "UIWindow/HierarchyWindow.h"
#include "UIWindow/ContentBrowserWindow.h"
#include "UIWindow/ConsoleWindow.h"
#include "UIWindow/GameViewportWindow.h"

//IMGUI
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>
#include <imconfig.h>
#include <ImGuizmo.h>

//ENGINETOOLS
#include "Core/Tools/FilePath.h"

//EDITORTOOLS
#include "UITools/ImGuiFontManager.h"
#include "UITools/ImGuiWrapper.h"

#include "Core/IrisEditor.h"

namespace Iris
{
	namespace Editor
	{
		EditorLayer::EditorLayer()
		{
		}

		EditorLayer::~EditorLayer()
		{
		}

		void EditorLayer::Initialize(Engine::IWindow* _window, Engine::ContextInitializer* _contextInitializer, EditorRenderer* _editorRenderer, Engine::InputManager* _inputManager, Engine::InitEditorContext* _initializer, IrisEditor* _editor, Engine::ThreadManager* _threadManager)
		{
			std::vector<std::string> pathRendererType = {"Data", "Renderer Forward"};
			m_IsForward = Engine::ResourceManager::GetInstance()->GetMetadataManager()->GetMetadata("ProjectSettings")->GetData(pathRendererType);

			m_ProjPath = FIND_PATH("Game").string();

			std::string file = FIND_PATH("Bin").string();
			if (std::filesystem::exists(file + "\\UserCSharpProject.deps.json"))
				std::filesystem::remove(file + "\\UserCSharpProject.deps.json");
			if (std::filesystem::exists(file + "\\UserCSharpProject.dll"))
				std::filesystem::remove(file + "\\UserCSharpProject.dll");
			if (std::filesystem::exists(file + "\\UserCSharpProject.pdb"))
				std::filesystem::remove(file + "\\UserCSharpProject.pdb");

			m_EditorContext.Initialize(_window, _initializer->InitResourceContext->GetDevice(), _contextInitializer->GetRawRenderingContext()->GetSwapChain(), _initializer->InitResourceContext->GetRenderInterface());

			ImGuiWrapper::SetRHIType(_contextInitializer->GetRHIType());

			m_CurrentWindow = _window;

			if (_initializer->SceneManager->GetCurrentScene())
				m_InitScene = true;

			m_Editor = _editor;

			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
			io.ConfigFlags |= ImGuiConfigFlags_IsSRGB;
			io.ConfigDockingAlwaysTabBar = true;
			io.ConfigFlags |= ImGuiDockNodeFlags_NoWindowMenuButton;

			InitAllFonts();

			ImGui::StyleColorsDark();

			ImGui_ImplGlfw_InitForVulkan(_window->CastToGLFW()->GetRaw(), true);

			Engine::VulkanDevice* device = m_EditorContext.GetDevice()->CastToVulkan();

			ImGui_ImplVulkan_InitInfo info{};
			info.Instance = device->CastToVulkan()->GetInstance();
			info.Queue = device->CastToVulkan()->GetGraphicsQueue();
			info.DescriptorPool = m_EditorContext.GetDescriptorAllocator()->CastToVulkan()->GetDescriptorAllocator();
			info.RenderPass = _editorRenderer->GetRenderPass()->CastToVulkan()->GetRenderPass();
			info.Device = device->CastToVulkan()->GetLogicalDevice();
			info.PhysicalDevice = device->CastToVulkan()->GetPhysicalDevice();
			info.MinImageCount = MAX_FRAMES_IN_FLIGHT;
			info.ImageCount = MAX_FRAMES_IN_FLIGHT;
			info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

			ImGui_ImplVulkan_Init(&info);

			ImGui_ImplVulkan_CreateFontsTexture();

			m_Initializer = _initializer;

			mainViewport = new SceneViewportWindow("Main", _inputManager, _initializer, this);
			gameViewport = new GameViewportWindow("Main", _initializer->SceneManager, this);

			m_UIWindows.emplace_back(new InspectorWindow("Main", _initializer));
			m_UIWindows.emplace_back(mainViewport);
			m_UIWindows.emplace_back(gameViewport);
			m_UIWindows.emplace_back(new HierarchyWindow("Main", this, _initializer));
			m_UIWindows.emplace_back(new ContentBrowserWindow("Main", _contextInitializer->GetRawResourceContext(), this, _threadManager));
			m_UIWindows.emplace_back(new ConsoleWindow("Main"));

			ApplyStyle();

			Engine::ResourceManager* rm = Engine::ResourceManager::GetInstance();

			Engine::Texture* playIcon = rm->Create<Engine::Texture>(std::filesystem::path(FIND_PATH("PlayButton.png")));
			m_PlayIcon = ImGuiWrapper::CreateTextureID(playIcon);

			Engine::Texture* pauseIcon = rm->Create<Engine::Texture>(std::filesystem::path(FIND_PATH("PauseButton.png")));
			m_PauseIcon = ImGuiWrapper::CreateTextureID(pauseIcon);

			Engine::Texture* stopIcon = rm->Create<Engine::Texture>(std::filesystem::path(FIND_PATH("StopButton.png")));
			m_StopIcon = ImGuiWrapper::CreateTextureID(stopIcon);
		}

		void EditorLayer::Terminate(Engine::ContextInitializer* _contextInitializer)
		{
			std::vector<std::string> pathRendererType = { "Data", "Renderer Forward" };
			Engine::ResourceManager::GetInstance()->GetMetadataManager()->GetMetadata("ProjectSettings")->Write(pathRendererType, m_IsForward);

			_contextInitializer->GetRawRenderingContext()->GetDevice()->WaitIdle();

			size_t size = m_UIWindows.size();
			for (size_t i = 0; i < size; i++)
			{
				delete m_UIWindows[i];
			}
			m_UIWindows.clear();

			delete m_Initializer;
			m_Initializer = nullptr;

			FontManager::DeleteEachFont();
			ImGui::GetIO().Fonts->ClearFonts();
			ImGui::GetIO().Fonts->Clear();

			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();


			m_EditorContext.Terminate(_contextInitializer->GetRawResourceContext()->GetRenderInterface());
		}

		void EditorLayer::InitAllFonts()
		{
			ImGuiIO& io = ImGui::GetIO();

			std::filesystem::path fontMainPath1 = FIND_PATH("RobotoMono");
			float baseHeight = 1080;
			float scale =(float)(m_CurrentWindow->GetHeight())/ baseHeight;
			float size_pixels = 15.f * scale;
			ImFont* font = io.Fonts->AddFontFromFileTTF((fontMainPath1.string() + "\\RobotoMono-Bold.ttf").c_str(), size_pixels);
			FontManager::AddFontWithName("RobotoMono-Bold", font);

			font = io.Fonts->AddFontFromFileTTF((fontMainPath1.string() + "\\RobotoMono-BoldItalic.ttf").c_str(), size_pixels);
			FontManager::AddFontWithName("RobotoMono-BoldItalic", font);

			font = io.Fonts->AddFontFromFileTTF((fontMainPath1.string() + "\\RobotoMono-ExtraLight.ttf").c_str(), size_pixels);
			FontManager::AddFontWithName("RobotoMono-ExtraLight", font);

			font = io.Fonts->AddFontFromFileTTF((fontMainPath1.string() + "\\RobotoMono-ExtraLightItalic.ttf").c_str(), size_pixels);
			FontManager::AddFontWithName("RobotoMono-ExtraLightItalic", font);

			font = io.Fonts->AddFontFromFileTTF((fontMainPath1.string() + "\\RobotoMono-Italic.ttf").c_str(), size_pixels);
			FontManager::AddFontWithName("RobotoMono-Italic", font);

			font = io.Fonts->AddFontFromFileTTF((fontMainPath1.string() + "\\RobotoMono-Medium.ttf").c_str(), size_pixels);
			FontManager::AddFontWithName("RobotoMono-Medium", font);

			font = io.Fonts->AddFontFromFileTTF((fontMainPath1.string() + "\\RobotoMono-MediumItalic.ttf").c_str(), size_pixels);
			FontManager::AddFontWithName("RobotoMono-MediumItalic", font);

			font = io.Fonts->AddFontFromFileTTF((fontMainPath1.string() + "\\RobotoMono-Regular.ttf").c_str(), size_pixels);
			FontManager::AddFontWithName("RobotoMono-Regular", font);

			font = io.Fonts->AddFontFromFileTTF((fontMainPath1.string() + "\\RobotoMono-SemiBold.ttf").c_str(), size_pixels);
			FontManager::AddFontWithName("RobotoMono-SemiBold", font);

			font = io.Fonts->AddFontFromFileTTF((fontMainPath1.string() + "\\RobotoMono-SemiBoldItalic.ttf").c_str(), size_pixels);
			FontManager::AddFontWithName("RobotoMono-SemiBoldItalic", font);

			font = io.Fonts->AddFontFromFileTTF((fontMainPath1.string() + "\\RobotoMono-Thin.ttf").c_str(), size_pixels);
			FontManager::AddFontWithName("RobotoMono-Thin", font);

			font = io.Fonts->AddFontFromFileTTF((fontMainPath1.string() + "\\RobotoMono-ThinItalic.ttf").c_str(), size_pixels);
			FontManager::AddFontWithName("RobotoMono-ThinItalic", font);

			io.Fonts->Build();
		}

		void EditorLayer::StartEditorFrame(unsigned int _cFrame)
		{
			std::vector<Engine::Texture*> items = Engine::ResourceManager::GetInstance()->Get<Engine::Texture>();
			if (m_HoldTextureSize != items.size())
			{
				size_t textures = items.size();
				for (size_t i = 0; i < textures; i++)
				{
					if (items[i]->GetID() <= 0)
					{
						items[i]->SetID((unsigned long long)ImGuiWrapper::CreateTextureID(items[i]));
					}
				}
				m_HoldTextureSize = textures;
			}

			ImGuiIO& io = ImGui::GetIO();
			io.IniFilename = nullptr;

			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGuiViewport* viewport = ImGui::GetMainViewport();

			ImGuiDockNodeFlags dockNodeFlags = 0;
			dockNodeFlags |= ImGuiDockNodeFlags_NoWindowMenuButton;

			ImGuiID dockSpaceID = ImGui::DockSpaceOverViewport(0,viewport, ImGuiDockNodeFlags_PassthruCentralNode);

			if (m_FirstFrame)
			{
				SetupMyDockingLayout(dockSpaceID);
				m_FirstFrame = false;
			}

			ImGuizmo::SetOrthographic(false);
			ImGuizmo::BeginFrame();
		
			ImGui::PushFont(FontManager::GetFontFromName("RobotoMono-Bold"));

			DrawMainTabBar();

			if (m_OpenNewProjectPopup)
			{
				ImGui::OpenPopup("Initializing New Project");
				m_OpenNewProjectPopup = false;
			}

			NewProject();

			ImVec4 color(127.f / 255.f, 56.f / 255.f, 234.f / 255.f, 1.f);

			size_t size = m_UIWindows.size();

			for (size_t i = 0; i < size; i++)
			{
				m_UIWindows[i]->StartWindow(m_LightMode);
				bool savedState = m_UIWindows[i]->DrawWindow(_cFrame);
				if (savedState) { m_UnsavedProject = true; }
				m_UIWindows[i]->CloseWindow();
			}

			if (m_CurrentWindow->WindowShouldClose() && m_UnsavedProject)
			{
				ImGui::OpenPopup("Close without saving");
			}
			else if(m_CurrentWindow->WindowShouldClose())
			{
				m_Editor->SetCloseWindow(true);
			}

			// Always center this window when appearing
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			if (ImGui::BeginPopupModal("Close without saving", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("All the unsaved changes won't be saved.\nThis operation cannot be undone!");
				ImGui::Separator();

				if (ImGui::Button("Close", ImVec2(80, 0)))
				{ 
					m_Editor->SetCloseWindow(true);
					ImGui::CloseCurrentPopup();
				}

				ImGui::SetItemDefaultFocus();
				ImGui::SameLine();

				if (ImGui::Button("Save", ImVec2(80, 0))) 
				{ 
					m_Initializer->SceneManager->SaveCurrentScene();
					SceneSaved();
					m_Editor->SetCloseWindow(true);
					ImGui::CloseCurrentPopup(); 
				}

				ImGui::SetItemDefaultFocus();
				ImGui::SameLine();

				if (ImGui::Button("Cancel", ImVec2(80, 0)))
				{
					m_CurrentWindow->SetWindowShouldClose(false);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			ImGui::PopFont();
		}

		void EditorLayer::EndEditorFrame(Engine::RenderingContext* _renderingContext)
		{
			_renderingContext;
			ImGui::Render();
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), _renderingContext->GetCommandBuffer(_renderingContext->GetSwapChain()->GetCurrentFrame())->CastToVulkan()->GetCommandBuffer());
			ImGui::EndFrame();

			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
		}

		void EditorLayer::SetInspectorCurrentObject(Engine::IrisObject* _selectedObject)
		{
			dynamic_cast<InspectorWindow*>(m_UIWindows[0])->SetCurrentObject(_selectedObject);
			dynamic_cast<SceneViewportWindow*>(m_UIWindows[1])->SetCurrentObject(_selectedObject);
		}

		void EditorLayer::SetInspectorCurrentFile(IrisFile* _selectedFile)
		{
			dynamic_cast<InspectorWindow*>(m_UIWindows[0])->SetCurrentFile(_selectedFile);
			dynamic_cast<HierarchyWindow*>(m_UIWindows[3])->PushSelectedObject(nullptr);
		}

		void EditorLayer::SetInspectorCurrentMaterial(Engine::Material* _selectedMat)
		{
			dynamic_cast<InspectorWindow*>(m_UIWindows[0])->SetCurrentMaterial(_selectedMat);
			dynamic_cast<HierarchyWindow*>(m_UIWindows[3])->PushSelectedObject(nullptr);
		}

		void EditorLayer::SetupMyDockingLayout(ImGuiID _dockSpaceID)
		{
			ImGuiID dockspace_id = _dockSpaceID;

			ImGui::DockBuilderRemoveNode(dockspace_id);
			ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

			ImGuiID dock_main_id = dockspace_id;
			ImGuiID dock_id_left, dock_id_right, dock_id_bottom;

			// Split: Left 20%, Center 60%, Right 20%
			ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, &dock_id_right, &dock_main_id); // Inspector
			ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.25f, &dock_id_bottom, &dock_main_id); // Content Browser/Log(bottom of center)
			ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, &dock_id_left, &dock_main_id);   // Scene Hierarchy

			// `dock_main_id` is the central area for the Scene Viewport

			// Dock windows
			ImGui::DockBuilderDockWindow(m_UIWindows[3]->GetName().c_str(), dock_id_left);
			ImGui::DockBuilderDockWindow(m_UIWindows[0]->GetName().c_str(), dock_id_right);
			ImGui::DockBuilderDockWindow(m_UIWindows[4]->GetName().c_str(), dock_id_bottom);
			ImGui::DockBuilderDockWindow(m_UIWindows[5]->GetName().c_str(), dock_id_bottom);
			ImGui::DockBuilderDockWindow(m_UIWindows[1]->GetName().c_str(), dock_main_id);
			ImGui::DockBuilderDockWindow(m_UIWindows[2]->GetName().c_str(), dock_main_id);

			ImGui::DockBuilderFinish(dockspace_id);

			ImGui::Begin(m_UIWindows[3]->GetName().c_str()); ImGui::End();
			ImGui::Begin(m_UIWindows[0]->GetName().c_str()); ImGui::End();
			ImGui::Begin(m_UIWindows[4]->GetName().c_str()); ImGui::End();
			ImGui::Begin(m_UIWindows[5]->GetName().c_str()); ImGui::End();
			ImGui::Begin(m_UIWindows[1]->GetName().c_str()); ImGui::End();
			ImGui::Begin(m_UIWindows[2]->GetName().c_str()); ImGui::End();
			ImGui::SetWindowFocus(m_UIWindows[1]->GetName().c_str());
			ImGui::SetWindowFocus(m_UIWindows[4]->GetName().c_str());
		}

		void EditorLayer::NewProject()
		{
			// Always center this window when appearing
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			if (ImGui::BeginPopupModal("Initializing New Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				static float progress = 0.0f, progress_dir = 1.0f;

				progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;

				if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }

				ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
				ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
				ImGui::Text("Progress Bar");
				if (progress >= 1.f) 
				{ 
					progress = 0;
					m_InitScene = true;
					CreateNewProjectFiles();
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}

		void EditorLayer::CreateNewProjectFiles()
		{
			// create project
			std::filesystem::path path = FIND_PATH("Bin");
			m_ProjPath = path.string() + "\\Game";
			std::filesystem::create_directory(m_ProjPath);
			CreateCSharpProject();

			std::filesystem::create_directory(m_ProjPath + "\\Scenes");

			m_Initializer->SceneManager->AddScene("Scene0");

			m_Initializer->InitScene = m_Initializer->SceneManager->GetCurrentScene();
			
			m_Initializer->SceneManager->LoadCurrentScene(m_Initializer);

			dynamic_cast<ContentBrowserWindow*>(m_UIWindows[4])->ProjectLaunched();
			dynamic_cast<InspectorWindow*>(m_UIWindows[0])->ProjectLaunched();

			m_Editor->InitEditorCamera();
			m_Editor->InitCameraInput();

			m_Initializer->SceneManager->SaveCurrentScene();

			m_Initializer->InitRenderer->SetFinalRenderPass(m_Initializer->InitRenderer->GetMainRenderPass());
		}

		void EditorLayer::CreateCSharpProject()
		{
			std::string command = "dotnet new classlib -o \"" + m_ProjPath + "\" -n " + "UserCSharpProject";
			int result = std::system(command.c_str());
			if (result != 0)
			{
				DEBUG_WARN("C# project not created!");
				return;
			}
			std::remove((m_ProjPath + "\\Class1.cs").c_str());

			if (!std::filesystem::exists(m_ProjPath + "\\CSharpBridge"))
				std::filesystem::copy(FIND_PATH("CSharpBridge"), m_ProjPath + "\\CSharpBridge", std::filesystem::copy_options::recursive);
		}

		void EditorLayer::CompileDotNetProject()
		{
			std::string file = FIND_PATH("Bin").string();

			std::string command = "dotnet build " + FIND_PATH("Game").string() + "\\UserCSharpProject.csproj";
			int result = std::system(command.c_str());
			if (result != 0)
				DEBUG_WARN("C# project not build!");

			if (!std::filesystem::exists(file + "\\UserCSharpProject.deps.json"))
				std::filesystem::copy_file(FIND_PATH("UserCSharpProject.deps.json"), file + "\\UserCSharpProject.deps.json");
			if (!std::filesystem::exists(file + "\\UserCSharpProject.dll"))
				std::filesystem::copy_file(FIND_PATH("UserCSharpProject.dll"), file + "\\UserCSharpProject.dll");
			if (!std::filesystem::exists(file + "\\UserCSharpProject.pdb"))
				std::filesystem::copy_file(FIND_PATH("UserCSharpProject.pdb"), file + "\\UserCSharpProject.pdb");
		}

		void EditorLayer::AddNewFiles(std::vector<std::string> _paths)
		{
			dynamic_cast<ContentBrowserWindow*>(m_UIWindows[4])->AddNewFilesToContentBrowser(_paths);
		}

		void EditorLayer::DrawMainTabBar()
		{
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New", (const char*)0, false, !m_InitScene))
					{
						m_OpenNewProjectPopup = true;
					}
					ImGui::Separator();
					if (ImGui::MenuItem("Save", "CTRL+S", false, !m_EditorPlayMode)) 
					{ 
						SaveScene();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Edit"))
				{
					if (ImGui::MenuItem("Duplicate", "CTRL+D"))
					{
						DuplicateObject();
					}
					ImGui::Separator();
					if (ImGui::MenuItem("Cut", "CTRL+X")) {}
					if (ImGui::MenuItem("Copy", "CTRL+C")) {}
					if (ImGui::MenuItem("Paste", "CTRL+V")) {}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Editor Settings"))
				{
					if (ImGui::Checkbox("Light Mode", &m_LightMode))
						ApplyStyle();

					ImGui::EndMenu();
				}

				ImVec2 buttonSize(15,15);

				float spacing = ImGui::GetStyle().ItemSpacing.x;
				float playSpacing = 15.f;

				if (m_EditorStartPlayMode)
				{
					ImVec2 newPos;
					if (!m_EditorPlayMode)
					{
						newPos = ImVec2(ImGui::GetWindowContentRegionMin().x + ImGui::GetWindowWidth() / 2.f - (buttonSize.x + spacing) - playSpacing/2, ImGui::GetCursorPosY());
						ImGui::SetCursorPos(newPos);
						if (ImGui::ImageButton("##PlayButton", m_PlayIcon, buttonSize))
						{
							m_EditorPlayMode = !m_EditorPlayMode;
						}
					}
					else
					{
						newPos = ImVec2(ImGui::GetWindowContentRegionMin().x + ImGui::GetWindowWidth() / 2.f - (buttonSize.x + spacing) - playSpacing / 2, ImGui::GetCursorPosY());
						ImGui::SetCursorPos(newPos);
						if (ImGui::ImageButton("##PauseButton", m_PauseIcon, buttonSize))
						{
							m_EditorPlayMode = !m_EditorPlayMode;
						}
					}

					newPos = ImVec2(ImGui::GetWindowContentRegionMin().x + ImGui::GetWindowWidth() / 2.f + playSpacing / 2, ImGui::GetCursorPosY());
					ImGui::SetCursorPos(newPos);
					if (ImGui::ImageButton("##StopButton", m_StopIcon, buttonSize))
					{
						m_EditorPlayMode = false;
						m_EditorStartPlayMode = false;
						m_Editor->SetNeedReloadScene();
						SetInspectorCurrentObject(nullptr);
						m_Initializer->SceneManager->GetScriptSystem()->SetInstanciateScript(false);
					}
				}
				else
				{
					ImVec2 newPos = ImVec2(ImGui::GetWindowContentRegionMin().x + ImGui::GetWindowWidth() / 2.f - (buttonSize.x + spacing) / 2.f, ImGui::GetCursorPosY());
					ImGui::SetCursorPos(newPos);
					if (ImGui::ImageButton("##PlayButton", m_PlayIcon, buttonSize))
					{
						m_EditorPlayMode = !m_EditorPlayMode;
						m_EditorStartPlayMode = true;
						
						m_Initializer->SceneManager->SaveCurrentScene();

						// Compile C# dll
						CompileDotNetProject();
						Engine::ScriptManager* scriptm = Engine::ResourceManager::GetInstance()->GetScriptManager();

						// Init C# singleton in dll
						scriptm->InitCSharpSingleton(m_Editor->GetTimePtr(), Engine::SingletonType::TIME);
						Engine::SceneManager* scenem = m_Initializer->SceneManager;
						scriptm->InitCSharpSingleton(scenem, Engine::SingletonType::SCENE_MANAGER);
						scriptm->InitCSharpSingleton(scenem->GetCurrentScene(), Engine::SingletonType::SCENE);

						// Init all script instances
						Engine::ScriptSystem* ss = scenem->GetScriptSystem();
						ss->SetScriptInstances();
						ss->SetInstanciateScript(true);
					}
				}

				float width = ImGui::CalcTextSize("Project Settings").x + ImGui::GetStyle().FramePadding.x * 2.0f;

				ImVec2 newPos = ImVec2(ImGui::GetWindowContentRegionMin().x + ImGui::GetWindowWidth() - (width + spacing), ImGui::GetCursorPosY());
				ImGui::SetCursorPos(newPos);

				if (ImGui::BeginMenu("Project Settings##MainTabBar"))
				{
					ImGui::SeparatorText("Skybox");

					DrawSkyboxCombo();

					ImGui::SeparatorText("Rendering");

					bool isDeferred = !m_IsForward;
					
					ImGui::Checkbox("Forward", &m_IsForward);
					ImGui::SetItemTooltip("You will need to restart the editor");

					if(ImGui::Checkbox("Deferred", &isDeferred))
					{
						m_IsForward = !isDeferred;
					}
					ImGui::SetItemTooltip("You will need to restart the editor");

					ImGui::EndMenu();
				}

				ImGui::EndMainMenuBar();
			}
		}

		void EditorLayer::SetGameState(bool _isGameActive)
		{
			m_Editor->SetGameState(_isGameActive);
		}

		void EditorLayer::ApplyStyle()
		{
			ImGuiStyle& style = ImGui::GetStyle();
			ImVec4* colors = style.Colors;

			// Corners
			style.WindowRounding = 8.0f;
			style.ChildRounding = 8.0f;
			style.FrameRounding = 6.0f;
			style.PopupRounding = 6.0f;
			style.ScrollbarRounding = 6.0f;
			style.GrabRounding = 6.0f;
			style.TabRounding = 6.0f;

			ImVec4 clear = ImVec4(0.f, 0.f, 0.f, 0.f);

			Vector3 black = Vector3(0.f, 0.f, 0.f);

			Vector3 white = Vector3(1.f, 1.f, 1.f);

			Vector3 darkPurple = Vector3(42.f / 255.f, 19.f / 255.f, 47.f / 255.f);

			Vector3 lightPurple = Vector3(127.f / 255.f, 56.f / 255.f, 234.f / 255.f);

			Vector3 darkBlue = Vector3(3.f / 255.f, 52.f / 255.f, 102.f / 255.f);

			Vector3 skyBlue = Vector3(122.f / 255.f, 185.f / 255.f, 221.f / 255.f);

			Vector3 darkRed = Vector3(115.f / 255.f, 0.f / 255.f, 1.f / 255.f);

			Vector3 brightRed = Vector3(255.f / 255.f, 8.f / 255.f, 1.f / 255.f);

			Vector3 darkPink = Vector3(198.f / 255.f, 50.f / 255.f, 135.f / 255.f);

			Vector3 lightPink = Vector3(255.f / 255.f, 153.f / 255.f, 218.f / 255.f);

			Vector3 whiteCol;
			Vector3 blackCol;
			Vector3 darkPurpleCol;
			Vector3 lightPurpleCol;
			Vector3 darkBlueCol;
			Vector3 skyBlueCol;
			Vector3 darkRedCol;
			Vector3 brightRedCol;
			Vector3 darkPinkCol;
			Vector3 lightPinkCol;

			if (m_LightMode)
			{
				whiteCol = black;
				blackCol = white;
				darkPurpleCol = lightPurple;
				lightPurpleCol = darkPurple;
				darkBlueCol = skyBlue;
				skyBlueCol = darkBlue;
				darkRedCol = brightRed;
				brightRedCol = darkRed;
				darkPinkCol = lightPink;
				lightPinkCol = darkPink;
			}
			else
			{
				whiteCol = white;
				blackCol = black;
				darkPurpleCol = darkPurple;
				lightPurpleCol = lightPurple;
				darkBlueCol = darkBlue;
				skyBlueCol = skyBlue;
				darkRedCol = darkRed;
				brightRedCol = brightRed;
				darkPinkCol = darkPink;
				lightPinkCol = lightPink;
			}

			// Colors
			
			// /!\ Be careful with the alpha, the color will not respect the rgb values correctly if the alpha is not 1
			colors[ImGuiCol_Text] = ImVec4(whiteCol.GetX(), whiteCol.GetY(), whiteCol.GetZ(), 1.00f);
			colors[ImGuiCol_TextDisabled] = ImVec4(blackCol.GetX(), blackCol.GetY(), blackCol.GetZ(), 1.00f);
			colors[ImGuiCol_WindowBg] = ImVec4(darkPurpleCol.GetX(), darkPurpleCol.GetY(), darkPurpleCol.GetZ(), 0.7f);
			colors[ImGuiCol_ChildBg] = ImVec4(whiteCol.GetX(), whiteCol.GetY(), whiteCol.GetZ(), 0.1f);
			colors[ImGuiCol_PopupBg] = ImVec4(darkPurpleCol.GetX(), darkPurpleCol.GetY(), darkPurpleCol.GetZ(), 0.92f);
			colors[ImGuiCol_Border] = ImVec4(darkPurpleCol.GetX(), darkPurpleCol.GetY(), darkPurpleCol.GetZ(), 0.50f);
			colors[ImGuiCol_BorderShadow] = clear;
			colors[ImGuiCol_FrameBg] = ImVec4(whiteCol.GetX(), whiteCol.GetY(), whiteCol.GetZ(), 0.39f);
			colors[ImGuiCol_FrameBgHovered] = ImVec4(whiteCol.GetX(), whiteCol.GetY(), whiteCol.GetZ(), 0.1f);
			colors[ImGuiCol_FrameBgActive] = ImVec4(whiteCol.GetX(), whiteCol.GetY(), whiteCol.GetZ(), 0.69f);
			colors[ImGuiCol_TitleBg] = ImVec4(darkBlueCol.GetX(), darkBlueCol.GetY(), darkBlueCol.GetZ(), 0.65f);
			colors[ImGuiCol_TitleBgActive] = ImVec4(darkBlueCol.GetX(), darkBlueCol.GetY(), darkBlueCol.GetZ(), 0.87f);
			colors[ImGuiCol_TitleBgCollapsed] = ImVec4(darkBlueCol.GetX(), darkBlueCol.GetY(), darkBlueCol.GetZ(), 0.20f);
			colors[ImGuiCol_MenuBarBg] = ImVec4(blackCol.GetX(), blackCol.GetY(), blackCol.GetZ(), 0.80f);
			colors[ImGuiCol_ScrollbarBg] = ImVec4(blackCol.GetX(), blackCol.GetY(), blackCol.GetZ(), 0.60f);
			colors[ImGuiCol_ScrollbarGrab] = ImVec4(darkPinkCol.GetX(), darkPinkCol.GetY(), darkPinkCol.GetZ(), 0.30f);
			colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(darkPinkCol.GetX(), darkPinkCol.GetY(), darkPinkCol.GetZ(), 0.40f);
			colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(darkPinkCol.GetX(), darkPinkCol.GetY(), darkPinkCol.GetZ(), 0.60f);
			colors[ImGuiCol_CheckMark] = ImVec4(darkPurpleCol.GetX(), darkPurpleCol.GetY(), darkPurpleCol.GetZ(), 0.50f);
			colors[ImGuiCol_SliderGrab] = ImVec4(whiteCol.GetX(), whiteCol.GetY(), whiteCol.GetZ(), 0.30f);
			colors[ImGuiCol_SliderGrabActive] = ImVec4(lightPurpleCol.GetX(), lightPurpleCol.GetY(), lightPurpleCol.GetZ(), 0.60f);
			colors[ImGuiCol_Button] = ImVec4(whiteCol.GetX(), whiteCol.GetY(), whiteCol.GetZ(), 0.62f);
			colors[ImGuiCol_ButtonHovered] = ImVec4(darkRedCol.GetX(), darkRedCol.GetY(), darkRedCol.GetZ(), 0.79f);
			colors[ImGuiCol_ButtonActive] = ImVec4(darkRedCol.GetX(), darkRedCol.GetY(), darkRedCol.GetZ(), 1.00f);
			colors[ImGuiCol_Header] = ImVec4(darkBlueCol.GetX(), darkBlueCol.GetY(), darkBlueCol.GetZ(), 0.45f);
			colors[ImGuiCol_HeaderHovered] = ImVec4(darkBlueCol.GetX(), darkBlueCol.GetY(), darkBlueCol.GetZ(), 1.00f);
			colors[ImGuiCol_HeaderActive] = ImVec4(darkBlueCol.GetX(), darkBlueCol.GetY(), darkBlueCol.GetZ(), 0.80f);
			colors[ImGuiCol_Separator] = ImVec4(whiteCol.GetX(), whiteCol.GetY(), whiteCol.GetZ(), 0.60f);
			colors[ImGuiCol_SeparatorHovered] = ImVec4(whiteCol.GetX(), whiteCol.GetY(), whiteCol.GetZ(), 1.00f);
			colors[ImGuiCol_SeparatorActive] = ImVec4(whiteCol.GetX(), whiteCol.GetY(), whiteCol.GetZ(), 1.00f);
			colors[ImGuiCol_ResizeGrip] = ImVec4(lightPinkCol.GetX(), lightPinkCol.GetY(), lightPinkCol.GetZ(), 0.10f);
			colors[ImGuiCol_ResizeGripHovered] = ImVec4(darkPinkCol.GetX(), darkPinkCol.GetY(), darkPinkCol.GetZ(), 0.60f);
			colors[ImGuiCol_ResizeGripActive] = ImVec4(darkPinkCol.GetX(), darkPinkCol.GetY(), darkPinkCol.GetZ(), 0.90f);

			//black.GetX(), black.GetY(), black.GetZ()
			//white.GetX(), white.GetY(), white.GetZ()
			//darkPurple.GetX(), darkPurple.GetY(), darkPurple.GetZ()
			//lightPurple.GetX(), lightPurple.GetY(), lightPurple.GetZ()
			//darkBlue.GetX(), darkBlue.GetY(), darkBlue.GetZ()
			//skyBlue.GetX(), skyBlue.GetY(), skyBlue.GetZ()
			//darkRed.GetX(), darkRed.GetY(), darkRed.GetZ()
			//brightRed.GetX(), brightRed.GetY(), brightRed.GetZ()
			//darkPink.GetX(), darkPink.GetY(), darkPink.GetZ()
			//lightPink.GetX(), lightPink.GetY(), lightPink.GetZ()

			colors[ImGuiCol_TabHovered] = ImVec4(darkPinkCol.GetX(), darkPinkCol.GetY(), darkPinkCol.GetZ(), 0.80f);
			colors[ImGuiCol_Tab] = ImVec4(darkPurpleCol.GetX(), darkPurpleCol.GetY(), darkPurpleCol.GetZ(), 0.79f);
			colors[ImGuiCol_TabSelected] = ImVec4(blackCol.GetX(), blackCol.GetY(), blackCol.GetZ(), 1.00f);
			colors[ImGuiCol_TabSelectedOverline] = ImVec4(darkPurpleCol.GetX(), darkPurpleCol.GetY(), darkPurpleCol.GetZ(), 0.40f);
			colors[ImGuiCol_TabDimmed] = ImVec4(darkPurpleCol.GetX(), darkPurpleCol.GetY(), darkPurpleCol.GetZ(), 0.83f);
			colors[ImGuiCol_TabDimmedSelected] = ImVec4(darkPurpleCol.GetX(), darkPurpleCol.GetY(), darkPurpleCol.GetZ(), 0.83f);
			//colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(blackCol.GetX(), blackCol.GetY(), blackCol.GetZ(), 1.00f);
			colors[ImGuiCol_DockingPreview] = ImVec4(darkPurpleCol.GetX(), darkPurpleCol.GetY(), darkPurpleCol.GetZ(), 0.31f);
			colors[ImGuiCol_DockingEmptyBg] = ImVec4(darkPurpleCol.GetX(), darkPurpleCol.GetY(), darkPurpleCol.GetZ(), 1.00f);
			//colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			//colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			//colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			//colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
			//colors[ImGuiCol_TableHeaderBg] = ImVec4(darkPurple.GetX(), darkPurple.GetY(), darkPurple.GetZ(), 0.45f);
			//colors[ImGuiCol_TableBorderStrong] = ImVec4(darkPurple.GetX(), darkPurple.GetY(), darkPurple.GetZ(), 0.80f);
			//colors[ImGuiCol_TableBorderLight] = ImVec4(darkPurple.GetX(), darkPurple.GetY(), darkPurple.GetZ(), 1.00f);
			//colors[ImGuiCol_TableRowBg] = ImVec4(darkPurple.GetX(), darkPurple.GetY(), darkPurple.GetZ(), 0.00f);
			//colors[ImGuiCol_TableRowBgAlt] = ImVec4(darkPurple.GetX(), darkPurple.GetY(), darkPurple.GetZ(), 0.07f);
			colors[ImGuiCol_TextSelectedBg] = ImVec4(skyBlueCol.GetX(), skyBlueCol.GetY(), skyBlueCol.GetZ(), 0.35f);
			colors[ImGuiCol_DragDropTarget] = ImVec4(skyBlueCol.GetX(), skyBlueCol.GetY(), skyBlueCol.GetZ(), 0.90f);
			colors[ImGuiCol_NavHighlight] = ImVec4(darkPurpleCol.GetX(), darkPurpleCol.GetY(), darkPurpleCol.GetZ(), 0.80f);
			colors[ImGuiCol_NavWindowingHighlight] = ImVec4(whiteCol.GetX(), whiteCol.GetY(), whiteCol.GetZ(), 0.70f);
			colors[ImGuiCol_NavWindowingDimBg] = ImVec4(darkPurpleCol.GetX(), darkPurpleCol.GetY(), darkPurpleCol.GetZ(), 0.20f);
			colors[ImGuiCol_ModalWindowDimBg] = ImVec4(darkPurpleCol.GetX(), darkPurpleCol.GetY(), darkPurpleCol.GetZ(), 0.35f);

			if (GImGui && (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_IsSRGB))
				ImGui::ForceStyleColorSpaceConversion(&style, true);
		}

		void EditorLayer::SceneSaved()
		{
			size_t size = m_UIWindows.size();
			for (size_t i = 0; i < size; i++)
			{
				m_UIWindows[i]->CastToImGui()->SceneSaved();
			}
		}

		void EditorLayer::SaveScene()
		{
			if (!m_EditorPlayMode)
			{
				m_Initializer->SceneManager->SaveCurrentScene();
				SceneSaved();
				m_UnsavedProject = false;
			}
		}

		void EditorLayer::DuplicateObject()
		{
			dynamic_cast<HierarchyWindow*>(m_UIWindows[3])->DuplicateCurrentObject();
		}

		void EditorLayer::DrawSkyboxCombo()
		{
			std::vector<Engine::Texture*> items = Engine::ResourceManager::GetInstance()->Get<Engine::Texture>();
			int itemSelectedIdx = 0;
			const Engine::Texture& skyboxText = m_Initializer->SceneManager->GetRenderSystem()->GetSkybox()->GetTexture();
			
			const char* comboPreviewValue = skyboxText.GetName().data();
			if (ImGui::BeginCombo("##Skybox", comboPreviewValue))
			{
				float padding = 16.f;
				float thumbnailSize = 20;
				float cellSize = thumbnailSize + padding;
				float panelWidth = ImGui::GetContentRegionAvail().x;

				int columnCount = (int)(panelWidth / cellSize);
				if (columnCount < 1)
					columnCount = 1;

				ImGui::BeginTable("Skybox", columnCount);
				ImGui::TableNextColumn();
				for (int n = 0; n < items.size(); n++)
				{
					if (!items[n]->IsHDR())
						continue;

					const bool isSelected = (itemSelectedIdx == n);
					std::string textureName = items[n]->GetName();
					std::string textureID = items[n]->GetName() + "##Skybox";
					ImTextureID id = (ImTextureID)items[n]->GetID();
					ImGui::ImageButton(textureID.c_str(), id, { thumbnailSize,thumbnailSize });

					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						m_Editor->SetNeedReloadSkybox(textureName);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();

					ImGui::TableNextColumn();
				}
				ImGui::EndTable();
				ImGui::EndCombo();
			}
		}
	}
}