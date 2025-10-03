#pragma once
#include "UISubWindow/ImGuiSubWindow.h"

#include "UIWindow/ContentBrowserWindow.h"

#include "imgui.h"

//STD
#include <string>

namespace Iris
{
	namespace Editor
	{
		class TextureFileSubWindow : public ImGuiSubWindow
		{
		public:
			TextureFileSubWindow();
			~TextureFileSubWindow() override final;

			void StartSubWindow() override;
			void CloseSubWindow() override;
			bool DrawSubWindow() override;

			void SetFile(IrisFile* _file);
		private:
			IrisFile* m_File = nullptr;

			bool m_SubWindowOpened = false;

			bool m_LoadedTexture = false;

			ImTextureID m_FileTextureID = 0;
		};
	}
}