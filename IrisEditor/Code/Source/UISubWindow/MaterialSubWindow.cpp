#include "UISubWindow/MaterialSubWindow.h"

#include "Core/Graphics/RHI/IImage.h"

//Resources
#include "Resources/ResourceManager.h"
#include "Resources/Material.h"

#include "UIWindow/ContentBrowserWindow.h"

//UITools
#include "UITools/ImGuiWrapper.h"
#include "UITools/FileExtension.h"

namespace Iris
{
	namespace Editor
	{
		MaterialSubWindow::MaterialSubWindow()
		{
			p_Name = "Material";

			for (int i = 0; i < 4; i++)
			{
				m_TexturesID.push_back(i);
			}

			m_DefaultNoneTexture = ImGuiWrapper::CreateTextureID(Engine::ResourceManager::GetInstance()->Create<Engine::Texture>(FIND_PATH("Black.jpg")));
		}

		void MaterialSubWindow::StartSubWindow()
		{
			ImGuiTreeNodeFlags flags = 0;
			flags |= ImGuiTreeNodeFlags_DefaultOpen;
			//flags |= ImGuiTreeNodeFlags_OpenOnArrow;
			flags |= ImGuiTreeNodeFlags_Selected;
			flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
			if (m_Material)
			{
				if (ImGui::TreeNodeEx(p_Name.c_str(), flags))
				{
					m_SubWindowOpened = true;
				}
			}
		}

		void MaterialSubWindow::CloseSubWindow()
		{
			if (m_SubWindowOpened)
			{
				ImGui::TreePop();
				m_SubWindowOpened = false;
			}
		}

		bool MaterialSubWindow::DrawSubWindow()
		{
			if (m_SubWindowOpened)
			{
				DrawOverallMaterialParameters();

				if (m_CurrentMatIsLit)
				{
					DrawLitExclusiveMaterialParameters();
				}
			}
			return p_ModifiedValue;
		}

		void MaterialSubWindow::DrawOverallMaterialParameters()
		{
			DrawMaterialType();

			DrawTransparency();

			DrawDrawMode();

			DrawAlbedo();
		}

		void MaterialSubWindow::DrawMaterialType()
		{
			switch(m_Material->GetMaterialTemplate())
			{
			case Engine::MaterialTemplate::IE_MATERIAL_LIT:
				ImGui::SeparatorText("Lit");
				break;
			case Engine::MaterialTemplate::IE_MATERIAL_UNLIT:
				ImGui::SeparatorText("Unlit");
				break;
			}


		}

		void MaterialSubWindow::DrawTransparency()
		{
			std::vector<std::string> itemsTransparency{ "Opaque","Alpha Bend", "Additive"};
			int itemSelectedIdxTransparency = 0;

			switch (m_Material->GetTransparencyMode())
			{
			case Engine::MaterialTransparency::IE_OPAQUE:
				itemSelectedIdxTransparency = 0;
				break;
			case Engine::MaterialTransparency::IE_ALPHA_BLEND:
				itemSelectedIdxTransparency = 1;
				break;
			case Engine::MaterialTransparency::IE_ADDITIVE:
				itemSelectedIdxTransparency = 2;
				break;
			}

			const char* comboPreviewValueTransparency = itemsTransparency[itemSelectedIdxTransparency].c_str();

			if (ImGui::BeginCombo("Transparency Mode##Material", comboPreviewValueTransparency))
			{
				for (int n = 0; n < itemsTransparency.size(); n++)
				{
					const bool isSelected = (itemSelectedIdxTransparency == n);
					if (ImGui::Selectable(itemsTransparency[n].c_str(), isSelected))
					{
						itemSelectedIdxTransparency = n;
						switch (itemSelectedIdxTransparency)
						{
						case 0:
							m_Material->SetTransparencyMode(Engine::MaterialTransparency::IE_OPAQUE);
							break;
						case 1:
							m_Material->SetTransparencyMode(Engine::MaterialTransparency::IE_ALPHA_BLEND);
							break;
						case 2:
							m_Material->SetTransparencyMode(Engine::MaterialTransparency::IE_ADDITIVE);
							break;
						}
						p_ModifiedValue = true;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}

		void MaterialSubWindow::DrawDrawMode()
		{
			std::vector<std::string> itemsDrawMode{ "IE_BACK","IE_FRONT", "IE_FRONT_AND_BACK"};
			int itemSelectedIdxDrawMode = 0;

			switch (m_Material->GetDrawMode())
			{
			case Engine::MaterialDraw::IE_BACK:
				itemSelectedIdxDrawMode = 0;
				break;
			case Engine::MaterialDraw::IE_FRONT:
				itemSelectedIdxDrawMode = 1;
				break;
			case Engine::MaterialDraw::IE_FRONT_AND_BACK:
				itemSelectedIdxDrawMode = 2;
				break;
			}

			const char* comboPreviewValueDrawMode = itemsDrawMode[itemSelectedIdxDrawMode].c_str();

			if (ImGui::BeginCombo("Draw Mode##Material", comboPreviewValueDrawMode))
			{
				for (int n = 0; n < itemsDrawMode.size(); n++)
				{
					const bool is_selected = (itemSelectedIdxDrawMode == n);
					if (ImGui::Selectable(itemsDrawMode[n].c_str(), is_selected))
					{
						itemSelectedIdxDrawMode = n;
						switch (itemSelectedIdxDrawMode)
						{
						case 0:
							m_Material->SetDrawMode(Engine::MaterialDraw::IE_BACK);
							break;
						case 1:
							m_Material->SetDrawMode(Engine::MaterialDraw::IE_FRONT);
							break;
						case 2:
							m_Material->SetDrawMode(Engine::MaterialDraw::IE_FRONT_AND_BACK);
							break;
						}
						p_ModifiedValue = true;
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

		}

		void MaterialSubWindow::DrawAlbedo()
		{
			ImGui::SeparatorText("Albedo##Separator");

			std::vector<Engine::Texture*> itemsAlbedo;
			itemsAlbedo.push_back(nullptr);

			std::vector<Engine::Texture*> tempTexture = Engine::ResourceManager::GetInstance()->Get<Engine::Texture>();
			size_t size = tempTexture.size();
			for (size_t i = 0; i < size; i++)
			{
				if (tempTexture[i]->GetImage()->GetTextureType() != Engine::RHITextureType::IE_RHI_TEXTURE_2D)
					continue;

				itemsAlbedo.push_back(tempTexture[i]);
			}
			int itemSelectedIdxAlbedo = 0;

			Engine::LitPBRInputs* litInfo = dynamic_cast<Engine::LitPBRInputs*>(m_Material->GetInputVariables());
			const Engine::Texture* currentTexture = litInfo->GetAlbedoTexture();

			if (currentTexture)
			{
				for (int n = 1; n < itemsAlbedo.size(); n++)
				{
					if (itemsAlbedo[n]->GetName() == currentTexture->GetName())
						itemSelectedIdxAlbedo = n;
				}
			}
			std::string currentTextureName;
			if (itemsAlbedo[itemSelectedIdxAlbedo])
				currentTextureName = itemsAlbedo[itemSelectedIdxAlbedo]->GetName();
			else
				currentTextureName = "None";
			const char* comboPreviewValueAlbedo = currentTextureName.data();
			if (ImGui::BeginCombo("Albedo##Material", comboPreviewValueAlbedo))
			{
				float padding = 16.f;
				float thumbnailSize = 20;
				float cellSize = thumbnailSize + padding;
				float panelWidth = ImGui::GetContentRegionAvail().x;

				int columnCount = (int)(panelWidth / cellSize);
				if (columnCount < 1)
					columnCount = 1;

				ImGui::BeginTable("AlbedoMaterial", columnCount);
				ImGui::TableNextColumn();
				for (int n = 0; n < itemsAlbedo.size(); n++)
				{
					const bool isSelected = (itemSelectedIdxAlbedo == n);
					std::string textureName = "None";
					std::string textureID = textureName + "##AlbedoMaterial";
					ImTextureID id = m_DefaultNoneTexture;
					if (itemsAlbedo[n])
					{
						textureName = itemsAlbedo[n]->GetName();
						textureID = itemsAlbedo[n]->GetName() + "##AlbedoMaterial";
						id = (ImTextureID)itemsAlbedo[n]->GetID();
					}
					ImGui::ImageButton(textureID.c_str(), id, { thumbnailSize,thumbnailSize });

					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						itemSelectedIdxAlbedo = n;
						litInfo->SetAlbedo(itemsAlbedo[itemSelectedIdxAlbedo]);
						if (itemsAlbedo[itemSelectedIdxAlbedo])
							m_TexturesID[0] = itemsAlbedo[itemSelectedIdxAlbedo]->GetID();
						else
							m_TexturesID[0] = m_DefaultNoneTexture;
						p_ModifiedValue = true;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();

					ImGui::TableNextColumn();
				}
				ImGui::EndTable();
				ImGui::EndCombo();
			}
			SetDragDropTarget(0);
			if (currentTexture)
			{
				float imageSize = ImGui::GetWindowWidth()/10;
				ImGui::Image(m_TexturesID[0], ImVec2(imageSize, imageSize));
			}

			Vector4 albedoColor = litInfo->GetAlbedo();
			ImGui::PushItemWidth(100);
			ImGui::ColorPicker4("Albedo", &albedoColor.GetXRef());

			if(albedoColor != litInfo->GetAlbedo())
				p_ModifiedValue = true;

			litInfo->SetAlbedo(albedoColor);
			ImGui::PopItemWidth();

		}

		void MaterialSubWindow::DrawLitExclusiveMaterialParameters()
		{
			ImGui::SeparatorText("LIT##Separator");

			ImGui::Indent(20.f);

			bool gltf = m_Material->IsGLTF();
			if (ImGui::Checkbox("GLTF", &gltf))
			{
				m_Material->SetGLTF(gltf);
			}

			DrawRoughness();

			if(m_Material->IsGLTF())
				DrawGLTFMetallic();
			else
				DrawMetallic();


			DrawNormal();

			ImGui::Unindent();
		}

		void MaterialSubWindow::DrawRoughness()
		{
			ImGui::SeparatorText("Roughness##Separator");

			Engine::LitPBRInputs* litInfo = dynamic_cast<Engine::LitPBRInputs*>(m_Material->GetInputVariables());

			std::string checkName = "Roughness to float";
			if (m_Material->IsGLTF())
				checkName = "Roughness/Metallic to float";

			if (ImGui::Checkbox(checkName.c_str(), &m_RoughnessFloat))
			{
				if (m_RoughnessFloat)
				{
					litInfo->SetRoughnessValue(0.f);
					if (m_Material->IsGLTF())
						litInfo->SetMetallicValue(0.f);
				}
				else
				{
					litInfo->SetRoughnessTexture(Engine::ResourceManager::GetInstance()->Get<Engine::Texture>("DefaultTexture.jpg"));
					ImGuiWrapper::CreateTextureID(litInfo->GetRoughnessTexture());
				}
			}

			if (m_RoughnessFloat)
			{
				ImGui::Text("Roughness");
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				float x = litInfo->GetRoughness();
				ImGui::DragFloat("##MaterialRoughness", &x, m_Step, m_MinValue, m_MaxValue, "%.3f");

				if(x != litInfo->GetRoughness())
					p_ModifiedValue = true;

				litInfo->SetRoughnessValue(x);
			}
			else
			{
				std::vector<Engine::Texture*> items;
				items.push_back(nullptr);

				std::vector<Engine::Texture*> tempTexture = Engine::ResourceManager::GetInstance()->Get<Engine::Texture>();
				size_t size = tempTexture.size();
				for (size_t i = 0; i < size; i++)
				{
					if (tempTexture[i]->GetImage()->GetTextureType() != Engine::RHITextureType::IE_RHI_TEXTURE_2D)
						continue;

					items.push_back(tempTexture[i]);
				}
				int itemSelectedIdx = 0;

				const Engine::Texture* currentTexture = litInfo->GetRoughnessTexture();

				if (currentTexture)
				{
					for (int n = 1; n < items.size(); n++)
					{
						if (items[n]->GetName() == currentTexture->GetName())
							itemSelectedIdx = n;
					}
				}

				const std::string& currentTextureName2 = items[itemSelectedIdx]->GetName();
				const char* comboPreviewValue = currentTextureName2.data();
				if (ImGui::BeginCombo("Roughness##Material", comboPreviewValue))
				{
					float padding = 16.f;
					float thumbnailSize = 20;
					float cellSize = thumbnailSize + padding;
					float panelWidth = ImGui::GetContentRegionAvail().x;

					int columnCount = (int)(panelWidth / cellSize);
					if (columnCount < 1)
						columnCount = 1;

					ImGui::BeginTable("RoughnessMaterial", columnCount);
					ImGui::TableNextColumn();
					for (int n = 0; n < items.size(); n++)
					{
						const bool isSelected = (itemSelectedIdx == n);
						std::string textureName = "None";
						std::string textureID = textureName + "##RoughnessMaterial";
						ImTextureID id = m_DefaultNoneTexture;
						if (items[n])
						{
							textureName = items[n]->GetName();
							textureID = items[n]->GetName() + "##RoughnessMaterial";
							id = (ImTextureID)items[n]->GetID();
						}
						ImGui::ImageButton(textureID.c_str(), id, { thumbnailSize,thumbnailSize });

						if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
						{
							itemSelectedIdx = n;
							if (items[n])
							{
								litInfo->SetRoughnessTexture(items[itemSelectedIdx]);
								m_TexturesID[1] = items[itemSelectedIdx]->GetID();
							}
							else
							{
								litInfo->SetRoughnessValue(0.f);
								m_RoughnessFloat = true;
							}
							p_ModifiedValue = true;
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();

						ImGui::TableNextColumn();
					}
					ImGui::EndTable();
					ImGui::EndCombo();
				}
				SetDragDropTarget(1);
				if (currentTexture)
				{
					float imageSize = ImGui::GetWindowWidth() / 10;
					ImGui::Image(m_TexturesID[1], ImVec2(imageSize, imageSize));
				}
			}
		}

		void MaterialSubWindow::DrawMetallic()
		{
			ImGui::SeparatorText("Metallic##Separator");

			Engine::LitPBRInputs* litInfo = dynamic_cast<Engine::LitPBRInputs*>(m_Material->GetInputVariables());

			if (!litInfo->GetMetallicTexture())
				m_MetallicFloat = true;

			if (ImGui::Checkbox("Metallic to float", &m_MetallicFloat))
			{
				if (m_MetallicFloat)
				{
					litInfo->SetMetallicValue(0.f);
				}
				else
				{
					litInfo->SetMetallicTexture(Engine::ResourceManager::GetInstance()->Get<Engine::Texture>("DefaultTexture.jpg"));
					ImGuiWrapper::CreateTextureID(litInfo->GetMetallicTexture());
				}
			}

			if (m_MetallicFloat)
			{
				ImGui::Text("Metallic");
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				float x = litInfo->GetMetallic();
				ImGui::DragFloat("##MaterialMetallic", &x, m_Step, m_MinValue, m_MaxValue, "%.3f");

				if(x != litInfo->GetMetallic())
					p_ModifiedValue = true;

				litInfo->SetMetallicValue(x);
			}
			else
			{
				std::vector<Engine::Texture*> items;
				items.push_back(nullptr);

				std::vector<Engine::Texture*> tempTexture = Engine::ResourceManager::GetInstance()->Get<Engine::Texture>();
				size_t size = tempTexture.size();
				for (size_t i = 0; i < size; i++)
				{
					if (tempTexture[i]->GetImage()->GetTextureType() != Engine::RHITextureType::IE_RHI_TEXTURE_2D)
						continue;

					items.push_back(tempTexture[i]);
				}
				int itemSelectedIdx = 0;

				const Engine::Texture* currentTexture = litInfo->GetMetallicTexture();
				if (currentTexture)
				{
					for (int n = 1; n < items.size(); n++)
					{
						if (items[n]->GetName() == currentTexture->GetName())
							itemSelectedIdx = n;
					}
				}

				const std::string& currentTextureName3 = items[itemSelectedIdx]->GetName();
				const char* comboPreviewValue = currentTextureName3.data();

				if (ImGui::BeginCombo("Metallic##Material", comboPreviewValue))
				{
					float padding = 16.f;
					float thumbnailSize = 20;
					float cellSize = thumbnailSize + padding;
					float panelWidth = ImGui::GetContentRegionAvail().x;

					int columnCount = (int)(panelWidth / cellSize);
					if (columnCount < 1)
						columnCount = 1;

					ImGui::BeginTable("MetallicMaterial", columnCount);
					ImGui::TableNextColumn();
					for (int n = 0; n < items.size(); n++)
					{
						const bool isSelected = (itemSelectedIdx == n);
						std::string textureName = "None";
						std::string textureID = textureName + "##MetallicMaterial";
						ImTextureID id = m_DefaultNoneTexture;
						if (items[n])
						{
							textureName = items[n]->GetName();
							textureID = items[n]->GetName() + "##MetallicMaterial";
							id = (ImTextureID)items[n]->GetID();
						}
						ImGui::ImageButton(textureID.c_str(), id, { thumbnailSize,thumbnailSize });

						if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
						{
							itemSelectedIdx = n;
							if (items[n])
							{
								litInfo->SetMetallicTexture(items[itemSelectedIdx]);
								m_TexturesID[2] = items[itemSelectedIdx]->GetID();
							}
							else
							{
								litInfo->SetMetallicValue(0.f);
								m_MetallicFloat = true;
							}
							p_ModifiedValue = true;
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();

						ImGui::TableNextColumn();
					}
					ImGui::EndTable();
					ImGui::EndCombo();
				}
				SetDragDropTarget(2);
				if (currentTexture)
				{
					float imageSize = ImGui::GetWindowWidth() / 10;
					ImGui::Image(m_TexturesID[2], ImVec2(imageSize, imageSize));
				}
			}
		}

		void MaterialSubWindow::DrawGLTFMetallic()
		{

			if (m_RoughnessFloat)
			{
				ImGui::SeparatorText("Metallic##Separator");
				Engine::LitPBRInputs* litInfo = dynamic_cast<Engine::LitPBRInputs*>(m_Material->GetInputVariables());
				ImGui::Text("Metallic");
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				float x = litInfo->GetMetallic();
				ImGui::DragFloat("##MaterialMetallic", &x, m_Step, m_MinValue, m_MaxValue, "%.3f");
				litInfo->SetMetallicValue(x);
			}
		}

		void MaterialSubWindow::DrawNormal()
		{
			ImGui::SeparatorText("Normal##Separator");
			std::vector<Engine::Texture*> itemsNormal;
			itemsNormal.push_back(nullptr);

			std::vector<Engine::Texture*> tempTexture = Engine::ResourceManager::GetInstance()->Get<Engine::Texture>();
			size_t size = tempTexture.size();
			for (size_t i = 0; i < size; i++)
			{
				if (tempTexture[i]->GetImage()->GetTextureType() != Engine::RHITextureType::IE_RHI_TEXTURE_2D)
					continue;

				itemsNormal.push_back(tempTexture[i]);
			}
			int itemSelectedIdxNormal = 0;

			Engine::LitPBRInputs* litInfo = dynamic_cast<Engine::LitPBRInputs*>(m_Material->GetInputVariables());
			const Engine::Texture* currentTexture = litInfo->GetNormalTexture();
			if (currentTexture)
			{
				for (int n = 1; n < itemsNormal.size(); n++)
				{
					if (itemsNormal[n]->GetName() == currentTexture->GetName())
						itemSelectedIdxNormal = n;
				}
			}

			std::string currentTextureName;
			if (itemsNormal[itemSelectedIdxNormal])
				currentTextureName = itemsNormal[itemSelectedIdxNormal]->GetName();
			else
				currentTextureName = "None";
			const char* comboPreviewValueNormal = currentTextureName.data();

			if (ImGui::BeginCombo("Normal##Material", comboPreviewValueNormal))
			{
				float padding = 16.f;
				float thumbnailSize = 20;
				float cellSize = thumbnailSize + padding;
				float panelWidth = ImGui::GetContentRegionAvail().x;

				int columnCount = (int)(panelWidth / cellSize);
				if (columnCount < 1)
					columnCount = 1;

				ImGui::BeginTable("NormalMaterial", columnCount);
				ImGui::TableNextColumn();
				for (int n = 0; n < itemsNormal.size(); n++)
				{
					const bool isSelected = (itemSelectedIdxNormal == n);
					std::string textureName = "None";
					std::string textureID = textureName + "##NormalMaterial";
					ImTextureID id = m_DefaultNoneTexture;
					if (itemsNormal[n])
					{
						textureName = itemsNormal[n]->GetName();
						textureID = itemsNormal[n]->GetName() + "##NormalMaterial";
						id = (ImTextureID)itemsNormal[n]->GetID();
					}
					ImGui::ImageButton(textureID.c_str(), id, { thumbnailSize,thumbnailSize });

					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						itemSelectedIdxNormal = n;
						if (itemsNormal[n])
						{
							litInfo->SetNormal(itemsNormal[itemSelectedIdxNormal]);
							m_TexturesID[3] = itemsNormal[itemSelectedIdxNormal]->GetID();
						}
						else
						{
							litInfo->SetNormal(itemsNormal[itemSelectedIdxNormal]);
							m_TexturesID[3] = m_DefaultNoneTexture;
						}
						p_ModifiedValue = true;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();

					ImGui::TableNextColumn();
				}
				ImGui::EndTable();
				ImGui::EndCombo();
			}
			SetDragDropTarget(3);

			if (currentTexture)
			{
				float imageSize = ImGui::GetWindowWidth()/10;
				ImGui::Image(m_TexturesID[3], ImVec2(imageSize, imageSize));
			}
		}

		void MaterialSubWindow::SetMaterial(Engine::Material* _mat)
		{
			m_Material = _mat;
			Engine::LitPBRInputs* litInfo = dynamic_cast<Engine::LitPBRInputs*>(m_Material->GetInputVariables());

			if (litInfo)
			{
				m_CurrentMatIsLit = true;
				if (litInfo->GetAlbedoTexture())
				{
					m_TexturesID[0] = litInfo->GetAlbedoTexture()->GetID();
				}
				else
				{
					m_TexturesID[0] = 0;
				}
				if (litInfo->GetRoughnessTexture())
				{
					m_RoughnessFloat = false;
					m_TexturesID[1] = litInfo->GetRoughnessTexture()->GetID();
				}
				else
				{
					m_RoughnessFloat = true;
					m_TexturesID[1] = 0;
				}
				if (litInfo->GetMetallicTexture())
				{
					m_MetallicFloat = false;
					m_TexturesID[2] = litInfo->GetMetallicTexture()->GetID();
				}
				else
				{
					m_MetallicFloat = true;
					m_TexturesID[2] = 0;
				}
				if (litInfo->GetNormalTexture())
				{
					m_TexturesID[3] = litInfo->GetNormalTexture()->GetID();
				}
				else
				{
					m_TexturesID[3] = 0;
				}
			}
			else
			{
				m_CurrentMatIsLit = false;
				Engine::UnlitInputs* unlitInfo = dynamic_cast<Engine::UnlitInputs*>(m_Material->GetInputVariables());
				if (unlitInfo->GetAlbedoTexture())
				{
					m_TexturesID[0] = unlitInfo->GetAlbedoTexture()->GetID();
				}
				else
				{
					m_TexturesID[0] = 0;
				}
				m_TexturesID[1] = 0;
				m_TexturesID[2] = 0;
				m_TexturesID[3] = 0;
			}
		}

		void MaterialSubWindow::SetDragDropTarget(int _materialPart)
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropFile"))
				{
					IM_ASSERT(payload->DataSize > 0);
					std::vector<std::string> extention = FileExtension::GetTextureExtention();

					const char* pathCStr = static_cast<const char*>(payload->Data);
					std::string path(pathCStr);

					IrisFile file;
					file.path = path;
					file.filenameString = std::filesystem::path(path).filename().string();
					file.extension = std::filesystem::path(path).extension().string();

					auto it = std::find(extention.begin(), extention.end(), file.extension);

					if (it != extention.end())
					{
						Engine::Texture* texture = Engine::ResourceManager::GetInstance()->Get<Engine::Texture>(file.filenameString);
						if (!texture)
						{
							texture = Engine::ResourceManager::GetInstance()->Create<Engine::Texture>(file.path);
							texture->SetID(ImGuiWrapper::CreateTextureID(texture));
						}


						if (m_CurrentMatIsLit)
						{
							Engine::LitPBRInputs* litInfo = dynamic_cast<Engine::LitPBRInputs*>(m_Material->GetInputVariables());
							switch (_materialPart)
							{
							case 0:
								litInfo->SetAlbedo(texture);
								m_TexturesID[0] = texture->GetID();
								break;
							case 1:
								litInfo->SetRoughnessTexture(texture);
								m_TexturesID[1] = texture->GetID();
								break;
							case 2:
								litInfo->SetMetallicTexture(texture);
								m_TexturesID[2] = texture->GetID();
								break;
							case 3:
								litInfo->SetNormal(texture);
								m_TexturesID[3] = texture->GetID();
								break;
							default:
								break;
							}
						}
						else
						{
							Engine::UnlitInputs* unlitInfo = dynamic_cast<Engine::UnlitInputs*>(m_Material->GetInputVariables());
							switch (_materialPart)
							{
							case 0:
								unlitInfo->SetAlbedo(texture);
								m_TexturesID[0] = texture->GetID();
								break;
							default:
								break;
							}
						}


						p_ModifiedValue = true;
					}
					else
					{
						std::cout << "Not A Mesh" << std::endl;
					}
				}
				ImGui::EndDragDropTarget();
			}
		}
	}
}