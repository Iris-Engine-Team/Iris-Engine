#pragma once
#include "Core/Application.h"
#include "Core/EditorLayer.h"

#include "Vector/Vector2.h"
#include "Matrix/Matrix4.h"

#include "Core/Graphics/Renderer/EditorRenderer.h"

namespace Iris
{
	namespace Editor
	{
		class EditorLayer;

		class IrisEditor : public Engine::Application
		{
		public:
			IrisEditor();

			~IrisEditor() override;

			bool Initialize() override;

			void InitEditorCamera();

			void InitCameraInput();

			bool Terminate() override;

			void Update(double _deltaTime) override;
			void Render() override;

			void Run() override;

			void DropCallback(Engine::IWindow* _window, unsigned int _count, const std::vector<std::string>& _paths);

			void EditorCameraForward();
			void EditorCameraBackward();
			void EditorCameraLeft();
			void EditorCameraRight();
			void EditorCameraUp();
			void EditorCameraDown();
			void CamSpeedUp();
			void CamResetSpeed();

			void StartCamRotate(Vector2 _mousePos);
			void CamRotate(Vector2 _mousePos);
			void TerminateCamRotate(Vector2 _mousePos);

			IE_FORCE_INLINE bool GetCloseWindow() { return p_CloseWindow; }
			IE_FORCE_INLINE void SetCloseWindow(bool _closeWindow) { p_CloseWindow = _closeWindow; }

			void CaptureCtrlKey();
			void ReleaseCtrlKey();

			void CtrlAndSAction();
			void CtrlAndDAction();

			void SetGameState(bool _isGameActive);

			IE_FORCE_INLINE void SetNeedReloadScene() { m_MustReloadScene = true; }
			IE_FORCE_INLINE void SetNeedReloadSkybox(std::string _skyboxTextureName) { m_MustReloadSkybox = true; m_TextureToSet = _skyboxTextureName; }
		private:

			//VARIABLES
		public:
			static Vector2 s_LastPosEditor;
			EditorLayer IELayer;

		private:
			bool m_CamIsUsed = false;
			Engine::ComponentHandle<Engine::CameraComponent>* m_CameraEditor = nullptr;

			EditorRenderer m_EditorRenderer{};
			float m_BaseCamSpeed = 0;
			bool m_CamSpeedModified = false;
			bool m_SceneSaved = true;
			float m_MouseSensitivity = 0;
			bool m_CtrlKeyPressed = false;


			bool m_MustReloadScene = false;
			bool m_MustReloadSkybox = false;
			std::string m_TextureToSet;
		};
	}
}