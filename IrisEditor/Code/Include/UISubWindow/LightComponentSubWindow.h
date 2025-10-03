#pragma once
#include "UISubWindow/ImGuiSubWindow.h"

//STD
#include <string>

namespace Iris
{
	namespace Engine
	{
		class ILight;
		class LightComponent;
	}

	namespace Editor
	{
		class LightComponentSubWindow : public ImGuiSubWindow
		{
		public:
			LightComponentSubWindow();
			~LightComponentSubWindow() override final;

			void StartSubWindow() override;
			void CloseSubWindow() override;
			bool DrawSubWindow() override;

			void DrawLightParameters();

			void SetLightComponent(Engine::LightComponent* _selectedLight);
		private:

			Engine::LightComponent* m_CurrentLight = nullptr;

			bool m_SubWindowOpened = false;
		};
	}
}