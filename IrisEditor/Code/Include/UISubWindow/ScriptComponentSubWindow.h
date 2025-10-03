#pragma once
#include "UISubWindow/ImGuiSubWindow.h"

#include "imgui.h"

//STD
#include <string>

namespace Iris
{
	namespace Engine
	{
		class ScriptComponent;
		class SceneManager;
	}
	namespace Editor
	{
		class ScriptComponentSubWindow : public ImGuiSubWindow
		{
		public:
			ScriptComponentSubWindow();
			~ScriptComponentSubWindow() override final = default;

			void StartSubWindow() override;
			void CloseSubWindow() override;
			bool DrawSubWindow() override;

			void SetScriptComponent(Engine::ScriptComponent* _selectedScript);
		private:
			bool m_SubWindowOpened = false;

			Engine::ScriptComponent* m_CurrentScript = nullptr;
		};
	}
}