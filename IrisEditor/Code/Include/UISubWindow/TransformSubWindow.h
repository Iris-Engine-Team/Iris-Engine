#pragma once
#include "UISubWindow/ImGuiSubWindow.h"

//STD
#include <string>

namespace Iris
{
	namespace Engine
	{
		class ITransform;
	}
	namespace Editor
	{
		class TransformSubWindow : public ImGuiSubWindow
		{
		public:
			TransformSubWindow();
			~TransformSubWindow() override final;

			void StartSubWindow() override;
			void CloseSubWindow() override;
			bool DrawSubWindow() override;
			void SetTransform(Engine::ITransform* _selectedTransform);
		private:
			Engine::ITransform* m_CurrentTransform = nullptr;

			bool m_SubWindowOpened = false;
		};
	}
}