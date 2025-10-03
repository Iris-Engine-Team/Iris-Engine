#pragma once

//ENGINE
#include "Core/IWindow.h"
#include "Core/Graphics/Context/EditorContext.h"

#include "UIWindow/SceneViewportWindow.h"
#include "UIWindow/GameViewportWindow.h"

#include "Resources/Texture.h"

#include "imgui.h"

namespace Iris
{
	namespace Engine
	{
		struct InitEditorContext;
		class ContextInitializer;
		class RenderingContext;
		class IRenderer;
		class InputManager;
		class ThreadManager;
	}
	namespace Editor
	{
		class IrisEditor;

		class EditorRenderer;

		class IUIWindow;

		struct IrisFile;

		class EditorLayer
		{
		public:
			EditorLayer();
			~EditorLayer();

			void Initialize(Engine::IWindow* _window, Engine::ContextInitializer* _contextInitializer, EditorRenderer* _editorRenderer, Engine::InputManager* _inputManager, Engine::InitEditorContext* _initializer, IrisEditor* _editor, Engine::ThreadManager* _threadManager);
			void Terminate(Engine::ContextInitializer* _contextInitializer);

			void InitAllFonts();

			void StartEditorFrame(unsigned int _cFrame);
			void EndEditorFrame(Engine::RenderingContext* _renderingContext);

			void InputCallback(Engine::InputManager* _inputManager);

			void SetInspectorCurrentObject(Engine::IrisObject* _selectedObject);
			void SetInspectorCurrentFile(IrisFile* _selectedFile);
			void SetInspectorCurrentMaterial(Engine::Material* _selectedMat);

			void SetupMyDockingLayout(ImGuiID _dockSpaceID = 1);

			void NewProject();

			void CreateNewProjectFiles();

			void CreateCSharpProject();
			void CompileDotNetProject();

			void AddNewFiles(std::vector<std::string> _paths);

			void ApplyStyle();

			void DrawMainTabBar();

			void SetGameState(bool _isGameActive);

			IE_FORCE_INLINE bool GetEditorPlayMode() { return m_EditorPlayMode; }

			void SceneSaved();

			void SaveScene();

			void DuplicateObject();

			void DrawSkyboxCombo();

			SceneViewportWindow* mainViewport = nullptr;
			GameViewportWindow* gameViewport = nullptr;

		private:
			IrisEditor* m_Editor = nullptr;

			std::string m_ProjPath{};

			EditorContext m_EditorContext{};

			Engine::IWindow* m_CurrentWindow = nullptr;

			bool m_OpenNewProjectPopup = false;

			bool m_FirstFrame = true;
			std::vector<IUIWindow*> m_UIWindows;

			std::vector<ImTextureID> m_VPD;

			std::vector<ImFont*> m_Fonts;

			Engine::InitEditorContext* m_Initializer = nullptr;

			ImTextureID m_PlayIcon = 0;
			ImTextureID m_PauseIcon = 0;
			ImTextureID m_StopIcon = 0;

			size_t m_HoldTextureSize;
			
			bool m_UnsavedProject = false;

			bool m_LightMode = false;

			bool m_EditorPlayMode = false;
			bool m_EditorStartPlayMode = false;

			bool m_InitScene = false;

			bool m_IsForward = false;
		};
	}
}