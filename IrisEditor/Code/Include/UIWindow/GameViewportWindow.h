#pragma once
#include "UIWindow/ImGuiWindow.h"

//Tools
#include "Core/Tools/Macros.h"

//ImGui
#include "imgui.h"

//STD
#include <string>

#include "Core/Graphics/VulkanRHI/VulkanDescriptor.h"

namespace Iris
{
	namespace Engine
	{
		class IRenderer;
		class SceneManager;
		class RenderingContext;
		class CameraComponent;
	}
	namespace Editor
	{
		class ImGuiSubWindow;
		class EditorLayer;
		class EditorRenderer;

		class GameViewportWindow : public ImGuiEditorWindow
		{
		public:
			GameViewportWindow() = default;
			GameViewportWindow(std::string _hiddenID, Engine::SceneManager* _sceneManager, EditorLayer* _layerEditor);
			~GameViewportWindow() override;

			void StartWindow(bool _lightMode) override final;
			void CloseWindow() override;
			bool DrawWindow(unsigned int _cFrame) override;

			void UpdateSize(Engine::IRenderer* _renderer, EditorRenderer* _editorRenderer, Engine::RenderingContext* _renderingContext, Engine::CameraComponent* _camera);

			void Recreate(Engine::IRenderer* _renderer, EditorRenderer* _editorRenderer, Engine::RenderingContext* _renderingContext, Engine::CameraComponent* _camera);

			IE_FORCE_INLINE std::vector<ImTextureID> GetGameCameraDescriptors() { return m_ViewportIDs; }
			IE_FORCE_INLINE void SetGameCameraDescriptors(std::vector<ImTextureID> _newDescriptors) { m_ViewportIDs = _newDescriptors; }

			void DrawMenuBar();

		private:
			std::vector<ImTextureID> m_ViewportIDs;
			Engine::SceneManager* m_SceneManager = nullptr;
			EditorLayer* m_LayerEditor = nullptr;
		};
	}
}