#pragma once
#include "SECS/Components/ComponentID.h"
#include "UIWindow/ImGuiWindow.h"

//STD
#include <string>
#include <vector>

namespace Iris
{
	namespace Engine
	{
		class SceneManager;
		class IrisObject;
		struct InitEditorContext;
		class Material;
	}
	namespace Editor
	{
		class ImGuiSubWindow;
		struct IrisFile;

		class InspectorWindow : public ImGuiEditorWindow
		{
		public:
			InspectorWindow(std::string _hiddenID, Engine::InitEditorContext* _initializer);
			~InspectorWindow() override;

			void StartWindow(bool _lightMode) override;
			void CloseWindow() override;
			bool DrawWindow(unsigned int _cFrame) override;

			void SceneSaved() override;

			void DrawMenuBar();

			void DrawObjectInfo();
			void DrawFileInfo();
			void DrawMaterialInfo();

			void SetCurrentObject(Engine::IrisObject* _selectedObject);
			void SetCurrentFile(IrisFile* _selectedFile);
			void SetCurrentMaterial(Engine::Material* _selectedMat);

			void GetUnpossessedComponents();

			void AddComponentButton();

			void DeleteComponentPopup();
			void DisableComponentPopup();

			void ProjectLaunched();
		private:
			std::vector<ImGuiSubWindow*> m_SubWindows;
			std::vector<ImGuiSubWindow*> m_FileSubWindows;
			std::vector<ImGuiSubWindow*> m_MaterialSubWindows;
			Engine::IrisObject* m_CurrentObject = nullptr;
			IrisFile* m_CurrentFile = nullptr;
			Engine::Material* m_CurrentMaterial = nullptr;

			bool m_ObjectUpdated = false;

			std::vector<Engine::ComponentID> m_UnpossessedComponents;
			std::vector<std::string> m_UnpossessedComponentsNames;

			std::string m_AssetsPath;

			int m_OptionComponentIndex = -1;

			Engine::SceneManager* m_SceneManager = nullptr;

			Engine::InitEditorContext* m_Initializer;

			bool m_CreatedANewNameForMaterial = false;

			std::string m_NewMaterialName = "New Material";
		};
	}
}