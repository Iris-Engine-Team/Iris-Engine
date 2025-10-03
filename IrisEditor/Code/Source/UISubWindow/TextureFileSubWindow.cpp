#include "UISubWindow/TextureFileSubWindow.h"

#include "imgui.h"

#include "UITools/ImGuiWrapper.h"

#include "UITools/FileExtension.h"

#include "Resources/ResourceManager.h"

#include "Resources/Texture.h"

#include "Core/Graphics/RHI/CommonTypes.h"

#include "Core/Graphics/RHI/IImage.h"

namespace Iris
{
	namespace Editor
	{
		TextureFileSubWindow::TextureFileSubWindow()
		{
			p_Name = "Texture";
		}

		TextureFileSubWindow::~TextureFileSubWindow()
		{
			m_File = nullptr;
		}

		void TextureFileSubWindow::StartSubWindow()
		{
			ImGuiTreeNodeFlags flags = 0;
			//flags |= ImGuiTreeNodeFlags_OpenOnArrow;
			flags |= ImGuiTreeNodeFlags_DefaultOpen;
			flags |= ImGuiTreeNodeFlags_Selected;
			flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
			if (m_LoadedTexture)
			{
				if (ImGui::TreeNodeEx(p_Name.c_str(), flags))
				{
					m_SubWindowOpened = true;
				}
			}
		}

		void TextureFileSubWindow::CloseSubWindow()
		{
			if (m_SubWindowOpened)
			{
				ImGui::TreePop();
				m_SubWindowOpened = false;
			}
		}

		bool TextureFileSubWindow::DrawSubWindow()
		{
			if (m_SubWindowOpened)
			{
				Engine::Texture* texture = Engine::ResourceManager::GetInstance()->Get<Engine::Texture>(m_File->filenameString);

				if (texture)
				{
					float spacing = ImGui::GetStyle().ItemSpacing.x;
					float width = 0;
					switch (texture->GetImage()->GetTextureType())
					{
					case Engine::RHITextureType::IE_RHI_TEXTURE_2D:
						width = ImGui::CalcTextSize("Texture2D").x + ImGui::GetStyle().FramePadding.x * 2.0f;
						ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x) / 2 + ImGui::GetWindowContentRegionMin().x - (width + spacing) / 2);
						ImGui::Text("Texture2D");
						break;
					case Engine::RHITextureType::IE_RHI_TEXTURE_CUBEMAP:
						width = ImGui::CalcTextSize("CubeMap").x + ImGui::GetStyle().FramePadding.x * 2.0f;
						ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x) / 2 + ImGui::GetWindowContentRegionMin().x - (width + spacing) / 2);
						ImGui::Text("CubeMap");
						break;
					}

					if (texture->GetImage()->GetTextureType() == Engine::RHITextureType::IE_RHI_TEXTURE_2D)
					{
						float imageSize = ImGui::GetWindowWidth();
						ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMin().x, ImGui::GetCursorPos().y));
						ImGui::Image(m_FileTextureID, ImVec2(imageSize, imageSize), { 0,1 }, { 1,0 });
					}
				}
			}
			return p_ModifiedValue;
		}

		void TextureFileSubWindow::SetFile(IrisFile* _file)
		{
			m_File = _file;
			if (m_File)
			{
				std::vector<std::string> textureExtention = FileExtension::GetTextureExtention();

				auto hasTexture = std::find(textureExtention.begin(), textureExtention.end(), m_File->extension);

				if (hasTexture != textureExtention.end())
				{
					Engine::Texture* texture = Engine::ResourceManager::GetInstance()->Get<Engine::Texture>(m_File->filenameString);

					if (!texture)
					{
						Engine::TextureLoadingParameters parameters{};

						if (m_File->extension == ".hdr")
							parameters.ColorFormat = Engine::RHIColorFormat::IE_RHI_FORMAT_R32G32B32A32_SFLOAT;

						texture = Engine::ResourceManager::GetInstance()->Create<Engine::Texture>(FIND_PATH(m_File->filenameString), true, parameters);
					}

					m_LoadedTexture = texture;
					m_FileTextureID = ImGuiWrapper::CreateTextureID(texture);
				}
				else
				{
					m_LoadedTexture = false;
				}
			}
		}
	}
}