#pragma once
#include "UIWindow/ImGuiWindow.h"

#include "Resources/Texture.h"

#include "imgui.h"

//STD
#include <string>
#include <filesystem>

namespace Iris
{
	namespace Engine
	{
		class ResourceContext;
		class ThreadManager;
	}
	namespace Editor
	{
		class EditorLayer;

		struct IrisFile
		{
			std::string path;
			std::string filenameString;
			std::string extension;
			ImTextureID textureID;
		};

		struct IrisDirectory
		{
			std::string path;
			std::string directoryNameString;
			std::vector<IrisDirectory*> folderChilds;
			bool opened = false;
		};

		class ContentBrowserWindow : public ImGuiEditorWindow
		{
		public:
			ContentBrowserWindow(std::string _hiddenID, Engine::ResourceContext* _resourceContext, EditorLayer* _EditorLayer, Engine::ThreadManager* _threadManager);
			~ContentBrowserWindow() override;

			void StartWindow(bool _lightMode) override;
			void CloseWindow() override;
			bool DrawWindow(unsigned int _cFrame) override;

			void ProjectLaunched();

			void AddNewFilesToContentBrowser(std::vector<std::string> _paths);

		private:
			void DrawContentHierarchy();
			void RecursiveDrawImGuiTreeNode(IrisDirectory* _searchDir);

			void DrawContentExplorer();
			void RecursiveTreeNode(IrisDirectory* _directory);
			bool FolderHasFolderChild(const std::filesystem::path& _folder);

			void DrawDirectory(IrisDirectory _directory, float _thumbnail = 125.f);
			void DrawFile(IrisFile _file, float _thumbnail = 125.f);

			void RenameDirectory(IrisDirectory _directory);

			void CopyDroppedFileToProject(std::filesystem::path _filePath);

			void DragDropFolders(IrisDirectory* _directory);
			void DropFiles(IrisDirectory* _directory);

			void ContentBrowserPopup();

			void RecursiveDeleteFolders(IrisDirectory* _directory);

			void ReloadContentBrowser();
			void DeleteContentBrowser();

			void UpdateCurrentContent();
			void ReloadCurrentContent();

			void DragDropFromWindows();

			void LoadNotLoadedTextures();

			void CheckAssetRootFileStatus();

		private:
			EditorLayer* m_EditorLayer;

			std::string m_Path;
			std::filesystem::path m_CurrentDirectory;

			IrisDirectory* m_AssetDirectory;

			ImTextureID m_DirectoryIcon;
			ImTextureID m_FileIcon;
			ImTextureID m_CSFileIcon;

			std::vector<std::filesystem::path> m_DroppedFiles;

			std::vector<IrisFile> m_CurrentFiles;
			std::vector<IrisDirectory> m_CurrentFolders;

			IrisDirectory m_DirectoryToRename{"",""};
			IrisDirectory m_SelectedDirectory{"",""};

			bool m_UpdateContent = false;
			bool m_UpdateContentBrowser = false;

			bool m_OpenRenamePopup = false;
			bool m_RenamePopupOpened = false;

			Engine::ThreadManager* m_ThreadManager = nullptr;

			bool m_LaunchedThread = false;
			bool m_StopWatching = false;

			HANDLE m_StopWatchingEvent = NULL;
			char buffer[4096];
			DWORD bytesReturned;
			HANDLE hDir = NULL;
		};
	}
}