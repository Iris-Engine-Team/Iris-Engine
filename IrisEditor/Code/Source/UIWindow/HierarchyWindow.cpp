#include "UIWindow/HierarchyWindow.h"

//SECS
#include "SECS/Scene.h"
#include "SECS/IrisObject.h"
#include "SECS/Components/Transform.h"
#include "SECS/Components/IComponent.h"
#include "SECS/Components/ModelRenderer.h"
#include "SECS/Components/CameraComponent.h"
#include "SECS/SceneManager.h"
#include "SECS/Systems/CameraSystem.h"

//RESOURCES
#include "Resources/ResourceManager.h"
#include "Resources/Material.h"

//EDITORLAYER
#include "Core/EditorLayer.h"

//RENDERER
#include "Core/Graphics/Renderer/IRenderer.h"

//RHI
#include "Core/Graphics/RHI/IPipeline.h"

//CONTEXT
#include "Core/Graphics/Context/ResourceContext.h"

namespace Iris
{
	namespace Editor
	{
		HierarchyWindow::HierarchyWindow(std::string _hiddenID, EditorLayer* _editorLayer, Engine::InitEditorContext* _initializer)
		{
			p_Name = "Scene Hierarchy##" + _hiddenID;
			m_EditorLayer = _editorLayer;
			m_Initializer = _initializer;
		}
		HierarchyWindow::~HierarchyWindow()
		{
			ImGuiEditorWindow::~ImGuiEditorWindow();

			m_EditorLayer = nullptr;
		}

		void HierarchyWindow::StartWindow(bool _lightMode)
		{
			ImGuiEditorWindow::StartWindow(_lightMode);
		}
		void HierarchyWindow::CloseWindow()
		{
			ImGuiEditorWindow::CloseWindow();
		}
		bool HierarchyWindow::DrawWindow(unsigned int _cFrame)
		{
			_cFrame;

			if (p_WindowOpened)
			{
				Engine::Scene* currentScene = m_Initializer->SceneManager->GetCurrentScene();
				bool scene = currentScene;
				if (ImGui::BeginMenuBar())
				{
					if(scene)
						AddObjectInScene();

					ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMin().x, ImGui::GetCursorPosY()));
					float offsetY = 2.f;
					float maxPosButton = scene ? ImGui::GetItemRectMax().y : 56.f;
					float offset = 15.f;
					ImVec2 p1 = ImVec2(ImGui::GetCursorScreenPos().x - offset, maxPosButton - offsetY);
					ImVec2 p2 = ImVec2(p1.x + ImGui::GetWindowSize().x + offset, p1.y + 2.0f);
					ImGui::GetWindowDrawList()->AddRectFilled(p1, p2, IM_COL32(0, 0, 0, 255));
					ImGui::EndMenuBar();
				}

				if (scene)
				{
					AddInheritanceSpacing();
					RecursiveDrawScene(currentScene->GetRoot());
					m_NbOfInvisibleSpacing = 0;
					AddObjectRightClick();
				}
			}
			return p_UnSavedDocument;
		}

		void HierarchyWindow::AddObjectInScene()
		{
			float spacing = ImGui::GetStyle().ItemSpacing.x;
			float width = ImGui::CalcTextSize("Add").x + ImGui::GetStyle().FramePadding.x * 2.0f;

			ImVec2 newPos = ImVec2(ImGui::GetCursorPosX() + ImGui::GetWindowWidth() - (width + spacing), ImGui::GetCursorPosY());
			ImGui::SetCursorPos(newPos);

			if (ImGui::BeginMenu("Add##Hierarchy"))
			{
				AddObject("HierarchyButton");

				ImGui::EndMenu();
			}
		}

		void HierarchyWindow::AddObjectRightClick()
		{
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMin().x, ImGui::GetCursorPosY()));
			ImGui::InvisibleButton("##RightClickInteractionHierarchy", ImVec2(ImGui::GetContentRegionMax().x - ImGui::GetCursorPos().x, ImGui::GetContentRegionMax().y - ImGui::GetCursorPos().y));

			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				ImGui::OpenPopup("AddObjectRightClick");
			}
			if (ImGui::BeginPopup("AddObjectRightClick"))
			{
				AddObject("HierarchyRightClick");

				ImGui::EndPopup();
			}
		}

		void HierarchyWindow::AddObject(std::string _hiddenTag)
		{
			bool newObject = false;
			std::string objectName = "";

			Engine::ComponentHandle<Engine::CameraComponent>* camH = m_Initializer->InitScene->GetCameraSystem()->GetEditorCamera();
			Engine::ITransform* t = camH->GetIrisObject()->GetTransform();
			Vector3 posNewObject = t->GetGlobalPosition() + t->GetForward() * 10.f;

			std::string menuItemName = "Empty##" + _hiddenTag;
			if (ImGui::MenuItem(menuItemName.c_str()))
			{
				m_Initializer->InitScene->Instantiate(posNewObject, nullptr, "Empty IrisObject");
				PushSelectedObject(nullptr);
			}

			menuItemName = "Cube##" + _hiddenTag;
			if (ImGui::MenuItem(menuItemName.c_str()))
			{
				newObject = true;
				objectName = "Cube";
			}

			menuItemName = "Sphere##" + _hiddenTag;
			if (ImGui::MenuItem(menuItemName.c_str()))
			{
				newObject = true;
				objectName = "Sphere";
			}

			menuItemName = "Pill##" + _hiddenTag;
			if (ImGui::MenuItem(menuItemName.c_str()))
			{
				newObject = true;
				objectName = "Capsule";
			}

			menuItemName = "Plane##" + _hiddenTag;
			if (ImGui::MenuItem(menuItemName.c_str()))
			{
				newObject = true;
				objectName = "Plane";
			}

			menuItemName = "Cone##" + _hiddenTag;
			if (ImGui::MenuItem(menuItemName.c_str()))
			{
				newObject = true;
				objectName = "Cone";
			}

			if (newObject)
			{
				Engine::IrisObject* object = m_Initializer->InitScene->Instantiate(posNewObject, nullptr, objectName);

				Engine::ModelRenderer* mr = object->AddComponent<Engine::ModelRenderer>();
				Engine::Mesh* msh = Engine::ResourceManager::GetInstance()->Get<Engine::Mesh>("Default" + objectName + ".obj");
				mr->Init(m_Initializer->InitResourceContext, msh, m_Initializer->InitPipeline);

				PushSelectedObject(nullptr);

				p_UnSavedDocument = true;
			}
		}

		void HierarchyWindow::RecursiveDrawScene(Engine::IrisObject* _root)
		{
			ImGuiTreeNodeFlags flags = 0;
			flags |= ImGuiTreeNodeFlags_OpenOnArrow;
			flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

			std::vector<Engine::ComponentHandle<Engine::ITransform>> children = _root->GetTransform()->GetChildren();

			for (Engine::ComponentHandle< Engine::ITransform> hT : children)
			{
				Engine::ITransform* transform = hT.UseComponent();
				Engine::IrisObject* object = transform->GetIrisObject();

				if (!object->GetShowInHierarchy())
					continue;
				if (transform->GetChildren().size() <= 0)
				{
					flags |= ImGuiTreeNodeFlags_Leaf;
				}

				bool treeOpened = DrawObject(object, flags);

				AddInheritanceSpacing();

				if (treeOpened)
				{
					if (transform->GetChildren().size() > 0)
					{
						RecursiveDrawScene(object);
					}
					ImGui::TreePop();
				}
				flags &= ~ImGuiTreeNodeFlags_Leaf;
			}
		}

		bool HierarchyWindow::DrawObject(Engine::IrisObject* _object, ImGuiTreeNodeFlags _flags)
		{
			std::string treeName = _object->GetName() + "##Object" + std::to_string(_object->GetID());

			bool opened = false;
			if (_object == m_ObjectToRename)
			{
				DrawRenameObjectTextField(_object);
			}
			else
			{
				if (!_object->IsActive())
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.f));

				if (_object == m_CurrentObject)
					_flags |= ImGuiTreeNodeFlags_Selected;

				opened = ImGui::TreeNodeEx(treeName.c_str(), _flags);
				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					PushSelectedObject(_object);
				}
				else if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					ImGui::OpenPopup("ObjectOptionPopup");
					m_OptionObject = _object;
				}
				DrawObjectOptionPopup(_object);

				if (!_object->IsActive())
					ImGui::PopStyleColor();
			}

			// Our objects are both drag sources and drag targets
			AddObjectDragDropSource(_object, "Inheritance");

			AddObjectDragDropTarget(_object, "Inheritance");

			return opened;
		}

		void HierarchyWindow::DrawRenameObjectTextField(Engine::IrisObject* _object)
		{
			std::string str = _object->GetName();
			char* saveText = new char[50];
			std::copy(str.begin(), str.end(), saveText);
			saveText[str.size()] = '\0';

			if (ImGui::InputText("##Rename", saveText, 50, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				_object->SetName(saveText);
				m_ObjectToRename = nullptr;
				p_UnSavedDocument = true;
			}
			if (!ImGui::IsItemActive() && ImGui::IsItemDeactivated())
			{
				_object->SetName(saveText);
				m_ObjectToRename = nullptr;
				p_UnSavedDocument = true;
			}
			delete[] saveText;
		}

		void HierarchyWindow::DrawObjectOptionPopup(Engine::IrisObject* _object)
		{
			if (_object == m_OptionObject)
			{
				if (ImGui::BeginPopup("ObjectOptionPopup"))
				{
					m_OptionOpened = true;
					if (ImGui::MenuItem("Delete"))
					{
						m_OptionObject->Destroy();
						PushSelectedObject(nullptr);
						m_OptionObject = nullptr;
						p_UnSavedDocument = true;
					}
					if (ImGui::MenuItem("Rename"))
					{
						m_ObjectToRename = m_OptionObject;
						m_OptionObject = nullptr;
						p_UnSavedDocument = true;
					}
					if (ImGui::MenuItem("Duplicate"))
					{
						m_OptionObject->Duplicate(m_Initializer, m_OptionObject->GetTransform()->GetParent());
						m_OptionObject = nullptr;
						PushSelectedObject(nullptr);
						p_UnSavedDocument = true;
					}
					ImGui::EndPopup();
				}
				else if (m_OptionOpened)
				{
					m_OptionOpened = false;
					m_OptionObject = nullptr;
				}
			}
		}

		void HierarchyWindow::PushSelectedObject(Engine::IrisObject* _selectedObject)
		{
			m_EditorLayer->SetInspectorCurrentObject(_selectedObject);
			m_CurrentObject = _selectedObject;
		}

		void HierarchyWindow::AddInheritanceSpacing()
		{
			ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMin().x);
			std::string buttonName = "RemoveParent##Spacing" + std::to_string(m_NbOfInvisibleSpacing);
			ImGui::InvisibleButton(buttonName.c_str(), ImVec2(ImGui::GetWindowSize().x, 2));
			m_NbOfInvisibleSpacing++;
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Inheritance"))
				{
					IM_ASSERT(payload->DataSize == sizeof(uint64_t));
					uint64_t payload_n = *(const uint64_t*)payload->Data;
					Engine::IrisObject* payloadObject = m_Initializer->InitScene->GetObjectByID(payload_n);
					if (payloadObject)
					{
						payloadObject->GetTransform()->SetParent(m_Initializer->InitScene->GetRoot()->GetTransform());
						p_UnSavedDocument = true;
					}
				}
				ImGui::EndDragDropTarget();
			}
		}

		void HierarchyWindow::DuplicateCurrentObject()
		{
			if (m_CurrentObject)
			{
				m_CurrentObject->Duplicate(m_Initializer, m_CurrentObject->GetTransform()->GetParent());
				PushSelectedObject(nullptr);
				p_UnSavedDocument = true;
			}
		}

		void HierarchyWindow::AddObjectDragDropSource(Engine::IrisObject* _object, std::string _dragdropName)
		{
			static_cast<void>(_dragdropName);

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				// Set payload to carry the index of our item (could be anything)
				uint64_t objectID = _object->GetID();
				ImGui::SetDragDropPayload("Inheritance", &objectID, sizeof(uint64_t));

				// Display preview (could be anything, when dragging an image we could decide to display the filename and a small preview of the image, etc.)
				ImGui::Text(_object->GetName().c_str());
				ImGui::EndDragDropSource();
			}
		}
		void HierarchyWindow::AddObjectDragDropTarget(Engine::IrisObject* _object, std::string _dragdropName)
		{
			static_cast<void>(_dragdropName);

			Engine::ITransform* transform = _object->GetTransform();
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Inheritance"))
				{
					IM_ASSERT(payload->DataSize == sizeof(uint64_t));
					uint64_t payload_n = *(const uint64_t*)payload->Data;
					Engine::IrisObject* payloadObject = m_Initializer->InitScene->GetObjectByID(payload_n);
					if (payloadObject)
					{
						std::cout << payloadObject->GetName() << std::endl;
						payloadObject->GetTransform()->SetParent(transform);
						p_UnSavedDocument = true;
					}
				}
				ImGui::EndDragDropTarget();
			}
		}
	}
}