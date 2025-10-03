#pragma once
#include "UISubWindow/ImGuiSubWindow.h"

#include "imgui.h"

//STD
#include <string>

namespace Iris
{
	namespace Engine
	{
		class CameraComponent;
		class SceneManager;
	}
	namespace Editor
	{
		class CameraComponentSubWindow : public ImGuiSubWindow
		{
		public:
			CameraComponentSubWindow(Engine::SceneManager* _sceneManager);
			~CameraComponentSubWindow() override final = default;

			void StartSubWindow() override;
			void CloseSubWindow() override;
			bool DrawSubWindow() override;

			void SetCameraComponent(Engine::CameraComponent* _selectedCamera);
		private:
			bool m_SubWindowOpened = false;

			Engine::SceneManager* m_SceneManager = nullptr;
			Engine::CameraComponent* m_CurrentCamera = nullptr;
		};
	}
}