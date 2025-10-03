#include "UIWindow/InspectorWindow.h"

//SECS
#include "SECS/Components/CameraComponent.h"
#include "SECS/IrisObject.h"
#include "SECS/Components/IComponent.h"
#include "SECS/Scene.h"
#include "SECS/Components/LightComponent.h"
#include "SECS/Components/ModelRenderer.h"
#include "SECS/Components/RigidBody.h"
#include "SECS/Components/Collider.h"
#include "SECS/Components/ScriptComponent.h"
#include "SECS/SceneManager.h"
#include "SECS/Scene.h"

//Resources
#include "Resources/Material.h"
#include "Resources/Script.h"

//ComponentSubWindow
#include "UISubWindow/ImGuiSubWindow.h"
#include "UISubWindow/TransformSubWindow.h"
#include "UISubWindow/LightComponentSubWindow.h"
#include "UISubWindow/ModelRendererComponentSubWindow.h"
#include "UISubWindow/PhysicsComponentSubWindow.h"
#include "UISubWindow/CameraComponentSubWindow.h"
#include "UISubWindow/ScriptComponentSubWindow.h"

//FileSubWindow
#include "UISubWindow/TextureFileSubWindow.h"

//MaterialSubWindow
#include "UISubWindow/MaterialSubWindow.h"

//UIWindow
#include "UIWindow/ContentBrowserWindow.h"

//Tools
#include "Core/Tools/FilePath.h"

//STD
#include "iostream"

//Imgui
#include "imgui.h"

namespace Iris
{
	namespace Editor
	{

		InspectorWindow::InspectorWindow(std::string _hiddenID, Engine::InitEditorContext* _initializer)
		{
			m_SubWindows.push_back(new TransformSubWindow());
			m_SubWindows.push_back(new LightComponentSubWindow());
			m_SubWindows.push_back(new ModelRendererComponentSubWindow(_initializer, this));
			m_SubWindows.push_back(new PhysicsComponentSubWindow());
			m_SubWindows.push_back(new CameraComponentSubWindow(_initializer->SceneManager));
			m_SubWindows.push_back(new ScriptComponentSubWindow());

			m_FileSubWindows.push_back(new TextureFileSubWindow());

			m_MaterialSubWindows.push_back(new MaterialSubWindow());

			p_Name = "Inspector##" + _hiddenID;
			m_CurrentObject = nullptr;

			m_SceneManager = _initializer->SceneManager;

			m_Initializer = _initializer;

			std::filesystem::path pathGame = FIND_PATH("Game");

			m_AssetsPath = pathGame == "" ? "Assets" : "Game";
		}

		InspectorWindow::~InspectorWindow()
		{
			ImGuiEditorWindow::~ImGuiEditorWindow();

			size_t subWindowsCount = m_SubWindows.size();

			for (size_t i = 0; i < subWindowsCount; ++i)
			{
				delete m_SubWindows[i];
				m_SubWindows[i] = nullptr;
			}

			subWindowsCount = m_FileSubWindows.size();

			for (size_t i = 0; i < subWindowsCount; ++i)
			{
				delete m_FileSubWindows[i];
				m_FileSubWindows[i] = nullptr;
			}

			subWindowsCount = m_MaterialSubWindows.size();

			for (size_t i = 0; i < subWindowsCount; ++i)
			{
				delete m_MaterialSubWindows[i];
				m_MaterialSubWindows[i] = nullptr;
			}

			delete m_CurrentFile;
		}

		void InspectorWindow::StartWindow(bool _lightMode)
		{
			ImGuiEditorWindow::StartWindow(_lightMode);
		}

		void InspectorWindow::CloseWindow()
		{
			ImGuiEditorWindow::CloseWindow();
		}

		bool InspectorWindow::DrawWindow(unsigned int _cFrame)
		{
			_cFrame;
			if (p_WindowOpened)
			{
				if (!m_ObjectUpdated)
				{
					SetCurrentObject(m_CurrentObject);
					m_ObjectUpdated = true;
				}

				DrawMenuBar();

				if (m_CurrentObject && m_SceneManager->GetCurrentScene())
				{
					DrawObjectInfo();
				}
				else if (m_CurrentFile)
				{
					DrawFileInfo();
				}
				else if (m_CurrentMaterial)
				{
					DrawMaterialInfo();
				}
			}
			return p_UnSavedDocument;
		}

		void InspectorWindow::SceneSaved()
		{ 
			ImGuiEditorWindow::SceneSaved(); 
			size_t size = m_SubWindows.size();
			for (size_t i = 0; i < size; i++)
			{
				m_SubWindows[i]->CastToImGui()->SetSave(false);
			}
			size = m_FileSubWindows.size();
			for (size_t i = 0; i < size; i++)
			{
				m_FileSubWindows[i]->CastToImGui()->SetSave(false);
			}
			size = m_MaterialSubWindows.size();
			for (size_t i = 0; i < size; i++)
			{
				m_MaterialSubWindows[i]->CastToImGui()->SetSave(false);
			}
		}

		void InspectorWindow::DrawMenuBar()
		{
			if (ImGui::BeginMenuBar())
			{
				float spacing = ImGui::GetStyle().ItemSpacing.x;
				float width = ImGui::CalcTextSize("Create").x + ImGui::GetStyle().FramePadding.x * 2.0f;

				ImVec2 newPos = ImVec2(ImGui::GetCursorPosX() + ImGui::GetWindowWidth() - (width + spacing), ImGui::GetCursorPosY());
				ImGui::SetCursorPos(newPos);

				if (ImGui::BeginMenu("Create##NewInspector"))
				{
					char* saveText = new char[50];
					std::copy(m_NewMaterialName.begin(), m_NewMaterialName.end(), saveText);
					saveText[m_NewMaterialName.size()] = '\0';

					if (ImGui::InputText("##Rename", saveText, 50, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
					{
						m_NewMaterialName = saveText;
						m_CreatedANewNameForMaterial = true;
					}
					if (!ImGui::IsItemActive() && ImGui::IsItemDeactivated())
					{
						m_NewMaterialName = saveText;
						m_CreatedANewNameForMaterial = true;
					}
					delete[] saveText;

					if (!m_CreatedANewNameForMaterial)
					{
						ImGui::BeginDisabled();
					}
					if (ImGui::MenuItem("Material##NewInspector"))
					{
						Engine::Texture* defaultTexture = Engine::ResourceManager::GetInstance()->Get<Engine::Texture>("DefaultTexture.jpg");
						Engine::Texture* defaultNormal = Engine::ResourceManager::GetInstance()->Get<Engine::Texture>("DefaultNormal.jpg");
						Engine::LitPBRInputs inputs;
						inputs.SetAlbedo(defaultTexture);
						inputs.SetRoughnessTexture(defaultTexture);
						inputs.SetMetallicTexture(defaultTexture);
						inputs.SetNormal(defaultNormal);

						Engine::MaterialData data{};
						data.ContainerName = m_NewMaterialName;
						data.Inputs = &inputs;

						m_NewMaterialName = "New Material";
						m_CreatedANewNameForMaterial = false;

						Engine::Material* newMat = Engine::ResourceManager::GetInstance()->CreateResourceContainer<Engine::Material>(&data);

						SetCurrentMaterial(newMat);
					}
					else if (ImGui::MenuItem("Script##NewInspector"))
					{
						//Create New Script
						std::filesystem::path templatePath = FIND_PATH("Template.cs");

						std::string path = m_AssetsPath + "\\" + m_NewMaterialName + ".cs";
						std::ofstream newCsScript(path);

						std::ifstream templateScript(templatePath);

						if (!templateScript.is_open() || !newCsScript.is_open()) {
							std::cerr << "Erreur lors de l'ouverture des fichiers." << std::endl;
							return;
						}

						std::string line = "";
						while (std::getline(templateScript, line)) {
							size_t pos = 0;
							while ((pos = line.find("Template", pos)) != std::string::npos) {
								line.replace(pos, 8, m_NewMaterialName);
								pos += m_NewMaterialName.size();
							}
							newCsScript << line << '\n';
						}

						templateScript.close();

						newCsScript.close();

						Engine::ResourceManager::GetInstance()->Create<Engine::Script>(path);

						m_NewMaterialName = "New Material";
						m_CreatedANewNameForMaterial = false;
					}
					else if (!m_CreatedANewNameForMaterial)
					{
						ImGui::EndDisabled();
					}
					ImGui::EndMenu();
				}

				ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMin().x, ImGui::GetCursorPosY()));
				float offsetY = 2.f;
				float maxPosButton = ImGui::GetItemRectMax().y;
				float offset = 15.f;
				ImVec2 p1 = ImVec2(ImGui::GetCursorScreenPos().x - offset, maxPosButton - offsetY);
				ImVec2 p2 = ImVec2(p1.x + ImGui::GetWindowSize().x + offset, p1.y + 2.0f);
				ImGui::GetWindowDrawList()->AddRectFilled(p1, p2, IM_COL32(0, 0, 0, 255));
				ImGui::EndMenuBar();
			}
		}

		void InspectorWindow::DrawObjectInfo()
		{
			if (m_CurrentObject->IsDestroyed())
				return;

			std::string str = m_CurrentObject->GetName();
			char* saveText = new char[50];
			std::copy(str.begin(), str.end(), saveText);
			saveText[str.size()] = '\0';

			if (ImGui::InputText("##Rename", saveText, 50, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				m_CurrentObject->SetName(saveText);
				p_UnSavedDocument = true;
			}
			if (!ImGui::IsItemActive() && ImGui::IsItemDeactivated())
			{
				m_CurrentObject->SetName(saveText);
				p_UnSavedDocument = true;
			}
			delete[] saveText;

			ImGui::SameLine();

			float spacing = ImGui::GetStyle().ItemSpacing.x;
			float width = 15 + ImGui::GetStyle().FramePadding.x * 2.0f;

			ImVec2 newPos = ImVec2(ImGui::GetWindowContentRegionMin().x + ImGui::GetWindowWidth() - (width + spacing), ImGui::GetCursorPosY());
			ImGui::SetCursorPos(newPos);
			bool activeObject = m_CurrentObject->IsActive();
			if(ImGui::Checkbox("##DisableObject", &activeObject))
			{
				m_CurrentObject->SetActive(activeObject);
				p_UnSavedDocument = true;
			}

			bool disableOption = false;

			ImGui::BeginChild("##SubInspectorWindow");
			size_t size = m_SubWindows.size();
			for (size_t i = 0; i < size; i++)
			{
				m_SubWindows[i]->StartSubWindow();
				bool saved = m_SubWindows[i]->DrawSubWindow();
				if (saved) p_UnSavedDocument = true;
				m_SubWindows[i]->CloseSubWindow();

				if (m_CurrentObject && !disableOption && m_OptionComponentIndex <= 0 && ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					if (i == 0)
					{
						disableOption = true;
						continue;
					}
					ImGui::OpenPopup("ComponentOptionRightClick");
					m_OptionComponentIndex = (int)i;
				}
			}

			AddComponentButton();

			if (ImGui::BeginPopup("ComponentOptionRightClick"))
			{
				if (m_CurrentObject)
				{
					DeleteComponentPopup();
					DisableComponentPopup();
				}

				ImGui::EndPopup();
			}
			else
			{
				m_OptionComponentIndex = -1;
			}

			ImGui::EndChild();
		}

		void InspectorWindow::DrawFileInfo()
		{
			std::string filename = "Name : " + m_CurrentFile->filenameString;

			std::filesystem::path relativePath = std::filesystem::relative(m_CurrentFile->path, m_AssetsPath);
			std::string path = "Path : " + relativePath.string();
			std::string extension = "Extension : " + m_CurrentFile->extension;
			ImGui::SeparatorText(filename.c_str());
			ImGui::Text(path.c_str());
			ImGui::Text(extension.c_str());

			ImGui::BeginChild("##SubInspectorWindow");
			size_t size = m_FileSubWindows.size();
			for (size_t i = 0; i < size; i++)
			{
				m_FileSubWindows[i]->StartSubWindow();
				bool saved = m_FileSubWindows[i]->DrawSubWindow();
				if (saved) p_UnSavedDocument = true;
				m_FileSubWindows[i]->CloseSubWindow();
			}
			ImGui::EndChild();
		}

		void InspectorWindow::DrawMaterialInfo()
		{
			std::string matName = "Name : " + m_CurrentMaterial->GetName();;

			ImGui::SeparatorText(matName.c_str());

			ImGui::BeginChild("##SubInspectorWindow");
			size_t size = m_MaterialSubWindows.size();
			for (size_t i = 0; i < size; i++)
			{
				m_MaterialSubWindows[i]->StartSubWindow();
				bool saved = m_MaterialSubWindows[i]->DrawSubWindow();
				if (saved) p_UnSavedDocument = true;
				m_MaterialSubWindows[i]->CloseSubWindow();
			}
			ImGui::EndChild();
		}

		void InspectorWindow::AddComponentButton()
		{
			// Y step
			ImGui::InvisibleButton("AddComponentSpace", ImVec2(10, 25));

			// Center step
			ImGui::SetCursorPos(ImVec2((ImGui::GetWindowWidth() - ImGui::CalcTextSize("AddComponent").x) / 2, ImGui::GetCursorPosY()));
			if (ImGui::Button("AddComponent##Button"))
				ImGui::OpenPopup("AddComponentPopUp");

			if (ImGui::BeginPopup("AddComponentPopUp"))
			{
				ImGui::SeparatorText("Components");
				for (int i = 0; i < m_UnpossessedComponents.size(); i++)
					if (m_UnpossessedComponents[i] != Engine::ComponentID::IE_SCRIPT && ImGui::Selectable(m_UnpossessedComponentsNames[i].c_str()))
					{
						Engine::Scene* currentScene = m_SceneManager->GetCurrentScene();
						if (currentScene)
						{
							if (m_UnpossessedComponents[i] == Engine::ComponentID::IE_NUM_COMPONENT)
							{
								Engine::IComponent* newComponent1 = currentScene->AddComponent(m_CurrentObject, Engine::ComponentID::IE_COLLIDER);
								newComponent1->Init(*m_Initializer);
								Engine::IComponent* newComponent2 = currentScene->AddComponent(m_CurrentObject, Engine::ComponentID::IE_RIGIDBODY);
								newComponent2->Init(*m_Initializer);
								p_UnSavedDocument = true;
							}
							else if(m_UnpossessedComponents[i] != Engine::ComponentID::IE_SCRIPT)
							{
								Engine::IComponent* newComponent = currentScene->AddComponent(m_CurrentObject, m_UnpossessedComponents[i]);
								newComponent->Init(*m_Initializer);
								p_UnSavedDocument = true;
							}
							m_ObjectUpdated = false;
						}
					}
					else if (m_UnpossessedComponents[i] == Engine::ComponentID::IE_SCRIPT)
					{
						if (ImGui::BeginMenu("Scripts"))
						{
							std::vector<Engine::Script*> scripts = Engine::ResourceManager::GetInstance()->Get<Engine::Script>();
							size_t size = scripts.size();
							for (size_t j = 0; j < size; j++)
							{
								std::string scriptName = scripts[j]->GetName() + "##Script" + std::to_string(j);
								if (ImGui::MenuItem(scriptName.c_str()))
								{
									Engine::Scene* currentScene = m_SceneManager->GetCurrentScene();
									if (currentScene)
									{
										Engine::ScriptComponent* newComponent = dynamic_cast<Engine::ScriptComponent*>(currentScene->AddComponent(m_CurrentObject, m_UnpossessedComponents[i]));
										newComponent->Init(m_SceneManager->GetScriptSystem(), scripts[j]->GetName());
										p_UnSavedDocument = true;
									}
									m_ObjectUpdated = false;
								}
							}
							if (size == 0)
							{
								ImGui::Text("Create a script first");
							}
							ImGui::EndMenu();
						}
					}
				ImGui::EndPopup();
			}
		}

		void InspectorWindow::SetCurrentObject(Engine::IrisObject* _selectedObject)
		{
			m_CurrentObject = m_SceneManager->GetCurrentScene() ? _selectedObject : nullptr;
			if (m_CurrentObject)
			{
				dynamic_cast<TransformSubWindow*>(m_SubWindows[0])->SetTransform(m_CurrentObject->GetTransform());
				dynamic_cast<LightComponentSubWindow*>(m_SubWindows[1])->SetLightComponent(m_CurrentObject->GetComponent<Engine::LightComponent>());
				dynamic_cast<ModelRendererComponentSubWindow*>(m_SubWindows[2])->SetModelRendererComponent(m_CurrentObject->GetComponent<Engine::ModelRenderer>());
				dynamic_cast<PhysicsComponentSubWindow*>(m_SubWindows[3])->SetRigidBodyComponent(m_CurrentObject->GetComponent<Engine::RigidBody>(), m_CurrentObject->GetComponent<Engine::Collider>());
				dynamic_cast<CameraComponentSubWindow*>(m_SubWindows[4])->SetCameraComponent(m_CurrentObject->GetComponent<Engine::CameraComponent>());
				dynamic_cast<ScriptComponentSubWindow*>(m_SubWindows[5])->SetScriptComponent(m_CurrentObject->GetComponent<Engine::ScriptComponent>());
				if (m_CurrentFile)
				{
					SetCurrentFile(nullptr);
				}
				if (m_CurrentMaterial)
				{
					SetCurrentMaterial(nullptr);
				}
			}
			if(m_SceneManager->GetCurrentScene())
				m_SceneManager->GetCurrentScene()->SetSelectedObject(m_CurrentObject);
			GetUnpossessedComponents();
		}

		void InspectorWindow::SetCurrentFile(IrisFile* _selectedFile)
		{
			if (m_CurrentFile)
				delete m_CurrentFile;
			m_CurrentFile = _selectedFile;
			if (m_CurrentFile)
			{
				SetCurrentObject(nullptr);
				SetCurrentMaterial(nullptr);
				dynamic_cast<TextureFileSubWindow*>(m_FileSubWindows[0])->SetFile(_selectedFile);
			}
		}

		void InspectorWindow::SetCurrentMaterial(Engine::Material* _selectedMat)
		{
			if (m_CurrentMaterial)
				m_CurrentMaterial = nullptr;
			m_CurrentMaterial = _selectedMat;
			if (m_CurrentMaterial)
			{
				SetCurrentObject(nullptr);
				SetCurrentFile(nullptr);
				dynamic_cast<MaterialSubWindow*>(m_MaterialSubWindows[0])->SetMaterial(_selectedMat);
			}
		}

		void InspectorWindow::GetUnpossessedComponents()
		{ 
			if(m_UnpossessedComponents.size() > 0)
				m_UnpossessedComponents.clear();
			if(m_UnpossessedComponentsNames.size() > 0)
				m_UnpossessedComponentsNames.clear();
			if (m_CurrentObject)
			{
				if (!m_CurrentObject->GetComponent<Engine::LightComponent>())
				{
					m_UnpossessedComponents.push_back(Engine::LightComponent::GetID());
					m_UnpossessedComponentsNames.push_back(Engine::LightComponent::GetName());
				}
				if (!m_CurrentObject->GetComponent<Engine::ModelRenderer>())
				{
					m_UnpossessedComponents.push_back(Engine::ModelRenderer::GetID());
					m_UnpossessedComponentsNames.push_back(Engine::ModelRenderer::GetName());
				}
				if (!m_CurrentObject->GetComponent<Engine::RigidBody>() && !m_CurrentObject->GetComponent<Engine::Collider>())
				{
					m_UnpossessedComponents.push_back(Engine::ComponentID::IE_NUM_COMPONENT);
					m_UnpossessedComponentsNames.push_back("Physics");
				}
				if (!m_CurrentObject->GetComponent<Engine::CameraComponent>())
				{
					m_UnpossessedComponents.push_back(Engine::CameraComponent::GetID());
					m_UnpossessedComponentsNames.push_back(Engine::CameraComponent::GetName());
				}
				if (!m_CurrentObject->GetComponent<Engine::ScriptComponent>())
				{
					m_UnpossessedComponents.push_back(Engine::ScriptComponent::GetID());
					m_UnpossessedComponentsNames.push_back(Engine::ScriptComponent::GetName());
				}
			}
		}

		void InspectorWindow::DeleteComponentPopup()
		{
			if (ImGui::MenuItem("Delete Component##Inspector"))
			{
				switch (m_OptionComponentIndex)
				{
				case 1:
					m_CurrentObject->RemoveComponent<Engine::LightComponent>();
					break;
				case 2:
					m_CurrentObject->RemoveComponent<Engine::ModelRenderer>();
					break;
				case 3:
					m_CurrentObject->RemoveComponent<Engine::RigidBody>();
					m_CurrentObject->RemoveComponent<Engine::Collider>();
					break;
				case 4:
					m_CurrentObject->RemoveComponent<Engine::CameraComponent>();
					break;
				case 5:
					m_CurrentObject->RemoveComponent<Engine::ScriptComponent>();
					break;
				}
				m_ObjectUpdated = false;
				p_UnSavedDocument = true;
				m_OptionComponentIndex = -1;
			}
		}

		void InspectorWindow::DisableComponentPopup()
		{
			Engine::IComponent* component = nullptr;
			Engine::IComponent* component2 = nullptr;
			switch (m_OptionComponentIndex)
			{
			case 1:
				component = m_CurrentObject->GetComponent<Engine::LightComponent>();
				break;
			case 2:
				component = m_CurrentObject->GetComponent<Engine::ModelRenderer>();
				break;
			case 3:
				component = m_CurrentObject->GetComponent<Engine::RigidBody>();
				component2 = m_CurrentObject->GetComponent<Engine::Collider>();
				break;
			case 4:
				component = m_CurrentObject->GetComponent<Engine::CameraComponent>();
				break;
			case 5:
				component = m_CurrentObject->GetComponent<Engine::ScriptComponent>();
				break;
			}

			if (component)
			{
				bool enable = component->IsEnable();
				if (ImGui::Checkbox("Disable Component##Inspector", &enable))
				{
					component->SetEnable(enable);

					if(component2)
						component2->SetEnable(enable);

					m_ObjectUpdated = false;
					p_UnSavedDocument = true;
				}
			}
		}
		void InspectorWindow::ProjectLaunched()
		{
			m_AssetsPath = "Game";
		}
	}
}