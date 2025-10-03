#pragma once
#include "UIWindow/ImGuiWindow.h"

//ImGui : imguizmo needs imgui included before him otherwise it won't work
#include "imgui.h"
#include "ImGuizmo.h"

//STD
#include <string>

//RHI
//#include "Core/Graphics/VulkanRHI/VulkanDescriptor.h"

namespace Iris
{
	namespace Engine
	{
		class InputManager;
		class IrisObject;
		class CameraComponent;
		class IRenderer;
		class RenderingContext;

		struct InitEditorContext;
	}
	namespace Editor
	{
		class ImGuiSubWindow;
		class EditorLayer;
		class EditorRenderer;

		class SceneViewportWindow : public ImGuiEditorWindow
		{
		public:
			SceneViewportWindow() = default;
			SceneViewportWindow(std::string _hiddenID, Engine::InputManager* _inputManager, Engine::InitEditorContext* _initializer, EditorLayer* _layerEditor);
			~SceneViewportWindow() override;

			void StartWindow(bool _lightMode) override final;
			void CloseWindow() override;
			bool DrawWindow(unsigned int _cFrame) override;

			void DrawMenuBar();

			void DrawCameraParameters();

			void DrawGuizmo();

			void ImGuiSceneCallback();

			void SetCurrentObject(Engine::IrisObject* _selectedObject);

			void UpdateSize(Engine::IRenderer* _renderer, EditorRenderer* _editorRenderer, Engine::RenderingContext* _renderingContext, Engine::CameraComponent* _camera);

			IE_FORCE_INLINE std::vector<ImTextureID> GetEditorCameraDescriptors() { return m_ViewportIDs; }
			IE_FORCE_INLINE void SetEditorCameraDescriptors(std::vector<ImTextureID> _newDescriptors) { m_ViewportIDs = _newDescriptors; }

			ImGuizmo::MODE GetMode();
			ImGuizmo::OPERATION GetOperation();
			void SetMode(ImGuizmo::MODE _mode);
			void SetOperation(ImGuizmo::OPERATION _operation);

		private:
			EditorLayer* m_LayerEditor = nullptr;
			Engine::InputManager* m_InputManager = nullptr;
			std::vector<ImTextureID> m_ViewportIDs;
			Engine::InitEditorContext* m_Initializer = nullptr;
			Engine::IrisObject* m_CurrentObject = nullptr;

			ImGuizmo::OPERATION m_CurrentOperation = ImGuizmo::OPERATION::TRANSLATE;
			ImGuizmo::MODE m_CurrentModicationMode = ImGuizmo::MODE::WORLD;

			Vector2 m_SubWindowPos = Vector2(0,0);

			float m_MenuOffset = 0;

			ImGuiSubWindow* m_SubWindow = nullptr;
		};
	}
}