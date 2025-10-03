#pragma once
#include "UIWindow/ImGuiWindow.h"

//STD
#include <string>
#include "tuple"

//IMGUI
#include "imgui.h"

namespace Iris
{
	namespace Engine
	{
		class IrisObject;
		class ITransform;
		struct InitEditorContext;
		class Scene;
		class IRenderer;
	}

	namespace Editor
	{
		class EditorLayer;

		class HierarchyWindow : public ImGuiEditorWindow
		{
		public:
			HierarchyWindow(std::string _hiddenID, EditorLayer* _editorLayer, Engine::InitEditorContext* _initializer);
			~HierarchyWindow() override;

			void StartWindow(bool _lightMode) override;
			void CloseWindow() override;
			bool DrawWindow(unsigned int _cFrame) override;

			void AddObjectInScene();
			void AddObjectRightClick();

			void AddObject(std::string _hiddenTag);

			void PushSelectedObject(Engine::IrisObject* _selectedObject);

			void RecursiveDrawScene(Engine::IrisObject* _root);
			bool DrawObject(Engine::IrisObject* _object, ImGuiTreeNodeFlags _flags);
			void DrawRenameObjectTextField(Engine::IrisObject* _object);
			void DrawObjectOptionPopup(Engine::IrisObject* _object);
			void AddInheritanceSpacing();

			void DuplicateCurrentObject();

			void AddObjectDragDropSource(Engine::IrisObject* _object, std::string _dragdropName = "default");
			void AddObjectDragDropTarget(Engine::IrisObject* _object, std::string _dragdropName = "default");

		private:
			Engine::IrisObject* m_ObjectToRename = nullptr;

			EditorLayer* m_EditorLayer;

			Engine::InitEditorContext* m_Initializer = nullptr;

			bool m_OptionOpened = false;

			Engine::IrisObject* m_CurrentObject = nullptr;

			Engine::IrisObject* m_OptionObject = nullptr;

			int m_NbOfInvisibleSpacing = 0;
		};
	}
}