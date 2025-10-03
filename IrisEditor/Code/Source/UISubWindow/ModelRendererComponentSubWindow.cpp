#include "UISubWindow/ModelRendererComponentSubWindow.h"

//Resources
#include "Resources/ResourceManager.h"
#include "Resources/Mesh.h"

#include "UIWindow/ContentBrowserWindow.h"

#include "UITools/FileExtension.h"

//UIWindow
#include "UIWindow/InspectorWindow.h"

//SECS
#include "SECS/Components/IComponent.h"
#include "SECS/Components/ModelRenderer.h"

//RENDERER
#include "Core/Graphics/Renderer/IRenderer.h"

//RHI
#include "Core/Graphics/RHI/IPipeline.h"

//STD
#include "vector"

namespace Iris
{
	namespace Editor
	{
		ModelRendererComponentSubWindow::ModelRendererComponentSubWindow(Engine::InitEditorContext* _initializer, InspectorWindow* _inspector)
		{
			m_Initializer = _initializer;
			m_InspectorRef = _inspector;
			p_Name = "ModelRenderer";
		}

		void ModelRendererComponentSubWindow::StartSubWindow()
		{
			ImGuiTreeNodeFlags flags = 0;
			//flags |= ImGuiTreeNodeFlags_OpenOnArrow;
			flags |= ImGuiTreeNodeFlags_Selected;
			flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
			if (m_CurrentModel)
			{
				if (ImGui::TreeNodeEx(p_Name.c_str(), flags))
				{
					m_SubWindowOpened = true;
					if (!m_CurrentModel->IsEnable())
						ImGui::BeginDisabled();
				}
			}
		}
		void ModelRendererComponentSubWindow::CloseSubWindow()
		{
			if (m_SubWindowOpened)
			{
				if (!m_CurrentModel->IsEnable())
					ImGui::EndDisabled();
				ImGui::TreePop();
				m_SubWindowOpened = false;
			}
		}
		bool ModelRendererComponentSubWindow::DrawSubWindow()
		{
			if (m_SubWindowOpened)
			{
				ImGui::Text("Model");

				DrawMeshParameters();

				bool castShadow = m_CurrentModel->IsCastingShadow();
				if (ImGui::Checkbox("Cast Shadow##ModelRenderer", &castShadow))
				{
					m_CurrentModel->SetCastingShadow(castShadow);
				}

				ImGui::SeparatorText("Materials");

				std::vector<Engine::Material*> materials = m_CurrentModel->GetMaterials();
				std::vector<Engine::Material*> itemsMaterial = Engine::ResourceManager::GetInstance()->Get<Engine::Material>();

				size_t size = materials.size();
				for (int i = 0; i < size; i++)
				{
					int itemSelectedIdxMaterial = 0;

					const std::string& currentMaterialName = materials[i]->GetName();
					const char* comboPreviewValueMaterial = currentMaterialName.data();
					std::string comboName = "##Material" + std::to_string(i) + currentMaterialName;
					if (ImGui::BeginCombo(comboName.c_str(), comboPreviewValueMaterial))
					{
						for (int n = 0; n < itemsMaterial.size(); n++)
						{
							const bool isSelected = (itemSelectedIdxMaterial == n);
							std::string materialName = itemsMaterial[n]->GetName();
							if (ImGui::Selectable(materialName.c_str(), isSelected))
							{
								itemSelectedIdxMaterial = n;
								
								m_CurrentModel->SetMaterial(itemsMaterial[itemSelectedIdxMaterial], i);

								p_ModifiedValue = true;
							}

							if (isSelected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
					{
						m_InspectorRef->SetCurrentMaterial(materials[i]);
					}
				}
				if(ImGui::Button("Set Mesh's Default Materials"))
				{
					m_CurrentModel->GetDefaultMaterials();
				}
			}
			return p_ModifiedValue;
		}

		void ModelRendererComponentSubWindow::DrawMeshParameters()
		{
			std::vector<Engine::Mesh*> items = Engine::ResourceManager::GetInstance()->Get<Engine::Mesh>();
			int itemSelectedIdx = 0;

			if (m_CurrentModel->GetMesh())
			{
				for (int n = 0; n < items.size(); n++)
				{
					if (items[n]->GetName() == m_CurrentModel->GetMesh()->GetName())
						itemSelectedIdx = n;
				}
			}
			const std::string& currentFileName = m_CurrentModel->GetMesh() ? items[itemSelectedIdx]->GetName() : "";
			const char* comboPreviewValue = currentFileName.data();
			if (ImGui::BeginCombo("Type##ModelRenderer", comboPreviewValue))
			{
				for (int n = 0; n < items.size(); n++)
				{
					const bool isSelected = (itemSelectedIdx == n);
					std::string omgthename = items[n]->GetName() + "##Model";
					if (ImGui::Selectable(omgthename.c_str(), isSelected))
					{
						itemSelectedIdx = n;
						m_CurrentModel->SetMesh(items[n]);
						p_ModifiedValue = true;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			SetDragDropTarget();
		}

		void ModelRendererComponentSubWindow::SetDragDropTarget()
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropFile"))
				{
					IM_ASSERT(payload->DataSize > 0);
					std::vector<std::string> extention = FileExtension::GetMeshExtention();

					const char* pathCStr = static_cast<const char*>(payload->Data);
					std::string path(pathCStr);

					IrisFile file;
					file.path = path;
					file.filenameString = std::filesystem::path(path).filename().string();
					file.extension = std::filesystem::path(path).extension().string();

					auto it = std::find(extention.begin(), extention.end(), file.extension);

					if (it != extention.end())
					{
						Engine::Mesh* mesh = Engine::ResourceManager::GetInstance()->Get<Engine::Mesh>(file.filenameString);
						if (mesh)
						{
							m_CurrentModel->SetMesh(mesh);
						}
						else
						{
							mesh = Engine::ResourceManager::GetInstance()->Create<Engine::Mesh>(file.path);

							std::vector<Engine::ChildMesh> vector = *mesh->GetMeshData();
							m_CurrentModel->SetMesh(mesh);
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

		void ModelRendererComponentSubWindow::SetModelRendererComponent(Engine::ModelRenderer* _selectedModel)
		{
			m_CurrentModel = _selectedModel;
		}
	}
}