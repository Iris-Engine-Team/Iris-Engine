#include "UIWindow/ContentBrowserWindow.h"

//EngineTools
#include "Core/Tools/FilePath.h"

//UITools
#include "UITools/ImGuiWrapper.h"
#include "UITools/FileExtension.h"

//Graphics
#include "Core/Graphics/Context/ResourceContext.h"
#include "Core/Graphics/VulkanRHI/VulkanDescriptor.h"
#include "Core/Graphics/VulkanRHI/VulkanSampler.h"

//Resources
#include "Resources/ResourceManager.h"
#include "Resources/Texture.h"

//Loader
#include "Loader/TextureLoader/StbImageLoader.h"

//EditorLayer
#include "Core/EditorLayer.h"

//IMGUI
#include "imgui.h"
#include <imgui_impl_vulkan.h>

//STD
#include <tuple>
#include <vector>

//Windows
#include "WinBase.h"

namespace Iris
{
	namespace Editor
	{

		ContentBrowserWindow::ContentBrowserWindow(std::string _hiddenID, Engine::ResourceContext* _resourceContext, EditorLayer* _EditorLayer, Engine::ThreadManager* _threadManager)
		{
			_resourceContext;
			m_EditorLayer = _EditorLayer;

			m_ThreadManager = _threadManager;

			p_Name = "Content Browser##" + _hiddenID;

			std::filesystem::path pathGame = FIND_PATH("Game");

			m_CurrentDirectory = pathGame == "" ? FIND_PATH("Assets") : pathGame;

			hDir = CreateFileW(
				m_CurrentDirectory.c_str(),
				FILE_LIST_DIRECTORY,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
				NULL);

			if (hDir == INVALID_HANDLE_VALUE) 
			{
				std::wcerr << L"CreateFileW failed: " << GetLastError() << std::endl;
			}

			m_Path = m_CurrentDirectory.string();
			Engine::ResourceManager* rm = Engine::ResourceManager::GetInstance();
			Engine::Texture* directoryIcon = rm->Create<Engine::Texture>(std::filesystem::path(FIND_PATH("DirectoryIcon.png")));
			Engine::Texture* fileIcon = rm->Create<Engine::Texture>(std::filesystem::path(FIND_PATH("FileIcon.png")));
			Engine::Texture* CSFileIcon = rm->Create<Engine::Texture>(std::filesystem::path(FIND_PATH("CSFileIcon.png")));
			m_DirectoryIcon = ImGuiWrapper::CreateTextureID(directoryIcon);
			m_FileIcon = ImGuiWrapper::CreateTextureID(fileIcon);
			m_CSFileIcon = ImGuiWrapper::CreateTextureID(CSFileIcon);


			std::vector<IrisDirectory*> vector = std::vector<IrisDirectory*>();
			m_AssetDirectory = new IrisDirectory(m_Path, m_CurrentDirectory.filename().string(), vector);

			RecursiveTreeNode(m_AssetDirectory);
			UpdateCurrentContent();
		}

		ContentBrowserWindow::~ContentBrowserWindow()
		{
			ImGuiEditorWindow::~ImGuiEditorWindow();

			m_StopWatching = true;

			SetEvent(m_StopWatchingEvent);
			while (m_LaunchedThread)
			{
			}

			CloseHandle(hDir);

			DeleteContentBrowser();
			delete m_AssetDirectory;
		}

		void ContentBrowserWindow::StartWindow(bool _lightMode)
		{
			ImGuiEditorWindow::StartWindow(_lightMode);
		}

		void ContentBrowserWindow::CloseWindow()
		{
			ImGuiEditorWindow::CloseWindow();
		}

		bool ContentBrowserWindow::DrawWindow(unsigned int _cFrame)
		{
			_cFrame;
			if (p_WindowOpened)
			{

				if (ImGui::BeginMenuBar())
				{
					if (ImGui::Button("Reload"))
					{
						m_UpdateContent = true;
						m_UpdateContentBrowser = true;
					}
					ImGui::SetItemTooltip("Reload if you added files directly in the Windows Explorer");

					if (ImGui::Button("Load Textures"))
					{
						LoadNotLoadedTextures();
					}
					ImGui::SetItemTooltip("You will need to reload the content browser to see the changes");

					ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMin().x, ImGui::GetCursorPosY()));
					float offsetY = 2.f;
					float maxPosButton = ImGui::GetItemRectMax().y;
					float offset = 15.f;
					ImVec2 p1 = ImVec2(ImGui::GetCursorScreenPos().x - offset, maxPosButton - offsetY);
					ImVec2 p2 = ImVec2(p1.x + ImGui::GetWindowSize().x + offset, p1.y + 2.0f);
					ImGui::GetWindowDrawList()->AddRectFilled(p1, p2, IM_COL32(0, 0, 0, 255));
					ImGui::EndMenuBar();
				}

				DragDropFromWindows();

				if (ImGui::BeginTable("ContentBrowser", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerH))
				{
					ImGui::TableNextColumn();

					DrawContentHierarchy();

					ImGui::TableNextColumn();

					DrawContentExplorer();

					ImGui::EndTable();
				}

				if (m_UpdateContentBrowser)
					ReloadContentBrowser();
				if (m_UpdateContent)
					UpdateCurrentContent();

				ContentBrowserPopup();

				p_Focused = ImGui::IsWindowFocused();

				if (!m_LaunchedThread)
				{
					m_StopWatchingEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
					m_LaunchedThread = true;
					m_ThreadManager->SubmitTask([this]() { this->CheckAssetRootFileStatus(); });
				}
			}
			return p_UnSavedDocument;
		}

		void ContentBrowserWindow::ProjectLaunched()
		{
			DeleteContentBrowser();
			delete m_AssetDirectory;

			m_CurrentDirectory = FIND_PATH("Game");
			m_Path = m_CurrentDirectory.string();

			std::vector<IrisDirectory*> vector = std::vector<IrisDirectory*>();
			m_AssetDirectory = new IrisDirectory(m_Path, m_CurrentDirectory.filename().string(), vector);

			RecursiveTreeNode(m_AssetDirectory);
			UpdateCurrentContent();
		}

		void ContentBrowserWindow::AddNewFilesToContentBrowser(std::vector<std::string> _paths)
		{
			for (std::string pathFile : _paths)
			{
				m_DroppedFiles.push_back(pathFile);
			}
			DEBUG_LOG("Added files");
		}

		void ContentBrowserWindow::DrawContentHierarchy()
		{
			RecursiveDrawImGuiTreeNode(m_AssetDirectory);
		}

		void ContentBrowserWindow::RecursiveDrawImGuiTreeNode(IrisDirectory* _searchDir)
		{
			size_t size = _searchDir->folderChilds.size();
			for (size_t i = 0; i < size; i++)
			{
				ImGuiTreeNodeFlags flags = 0;
				flags |= ImGuiTreeNodeFlags_OpenOnArrow;

				bool hasChild = _searchDir->folderChilds[i]->folderChilds.size() > 0;
				if (!hasChild)
				{
					flags |= ImGuiTreeNodeFlags_Leaf;
				}

				bool treeOpened = ImGui::TreeNodeEx(_searchDir->folderChilds[i]->directoryNameString.c_str(), flags);

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					m_CurrentDirectory = _searchDir->folderChilds[i]->path;
					m_UpdateContent = true;
				}

				DragDropFolders(_searchDir->folderChilds[i]);
				DropFiles(_searchDir->folderChilds[i]);

				if (treeOpened)
				{
					if (!_searchDir->folderChilds[i]->opened)
						_searchDir->folderChilds[i]->opened = true;

					RecursiveDrawImGuiTreeNode(_searchDir->folderChilds[i]);
					ImGui::TreePop();
				}
				else
				{
					if (_searchDir->folderChilds[i]->opened)
						_searchDir->folderChilds[i]->opened = false;
				}
			}
		}

		void ContentBrowserWindow::DrawContentExplorer()
		{
			//Setup the cellsize for each file and folder
			float padding = 16.f;
			float thumbnailSize = 125.f;
			float cellSize = thumbnailSize + padding;
			float panelWidth = ImGui::GetContentRegionAvail().x;

			int columnCount = (int)(panelWidth / cellSize);
			if (columnCount < 1)
				columnCount = 1;

			if (ImGui::BeginTable("ContentExplorer", columnCount))
			{
				ImGui::TableNextColumn();
				if (m_CurrentDirectory != std::filesystem::path(m_Path))
				{
					if (ImGui::Button("<-"))
					{
						m_CurrentDirectory = m_CurrentDirectory.parent_path();
						m_UpdateContent = true;
					}

					ImGui::TableNextColumn();

					//add space that has the size of the "<-" button for every column if it is shown
					float fontHeightSize = ImGui::GetTextLineHeightWithSpacing();

					for (size_t i = 0; i < columnCount-1; i++)
					{
						ImGui::Dummy(ImVec2(0.0f, fontHeightSize));

						ImGui::TableNextColumn();
					}
				}

				size_t sizeDirectoryVector = m_CurrentFolders.size();
				size_t sizeFileVector = m_CurrentFiles.size();

				for (size_t i = 0; i < (sizeDirectoryVector + sizeFileVector); i++)
				{
					if (i < sizeDirectoryVector)
					{
						DrawDirectory(m_CurrentFolders[i]);
					}
					else
					{
						DrawFile(m_CurrentFiles[i - sizeDirectoryVector]);
					}
				}
				ImGui::TableNextRow(ImGuiTableRowFlags_None, ImGui::GetWindowHeight() - ImGui::GetCursorPosY() + ImGui::GetItemRectSize().y);
				ImGui::EndTable();
			}
		}

		void ContentBrowserWindow::RecursiveTreeNode(IrisDirectory* _directory)
		{
			if (!FolderHasFolderChild(_directory->path))
				return;

			ImGuiTreeNodeFlags flags = 0;
			flags |= ImGuiTreeNodeFlags_OpenOnArrow;

			for (auto& directoryEntry : std::filesystem::directory_iterator(_directory->path))
			{
				const std::filesystem::path& path = directoryEntry.path();
				std::string filenameString = path.filename().string();

				if (directoryEntry.is_directory())
				{
					IrisDirectory* directory = new IrisDirectory(path.string(), filenameString);
					_directory->folderChilds.push_back(directory);
					RecursiveTreeNode(directory);
				}
			}
		}

		bool ContentBrowserWindow::FolderHasFolderChild(const std::filesystem::path& _folder)
		{
			for (auto& directoryEntry : std::filesystem::directory_iterator(_folder))
			{
				if (directoryEntry.is_directory())
				{
					return true;
				}
			}
			return false;
		}

		void ContentBrowserWindow::DrawDirectory(IrisDirectory _directory, float _thumbnail)
		{
			//Change the color of the button to make a difference between files and folders
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.5f, 1.f));

			ImGui::ImageButton(_directory.directoryNameString.c_str(), m_DirectoryIcon, { _thumbnail, _thumbnail }, { 0,1 }, { 1,0 });

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				m_CurrentDirectory /= _directory.path;
				m_UpdateContent = true;
			}
			if (!m_OpenRenamePopup && ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				m_OpenRenamePopup = true;
				m_SelectedDirectory = _directory;
			}

			DragDropFolders(&_directory);
			DropFiles(&_directory);

			ImGui::PopStyleColor(3);
			if (_directory.path == m_DirectoryToRename.path)
			{
				RenameDirectory(_directory);
			}
			else
				ImGui::Text(_directory.directoryNameString.c_str());
			ImGui::TableNextColumn();
		}

		void ContentBrowserWindow::DrawFile(IrisFile _file, float _thumbnail)
		{
			//Set the base color of the button to transparent
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));

			ImGui::ImageButton(_file.filenameString.c_str(), _file.textureID, { _thumbnail, _thumbnail }, { 0,1 }, { 1,0 });

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				m_EditorLayer->SetInspectorCurrentFile(new IrisFile(_file));
			}

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				// Set payload to carry the index of our item (could be anything)
				std::string pathStr = _file.path;
				ImGui::SetDragDropPayload("DragDropFile", pathStr.c_str(), pathStr.size() + 1);

				// Display preview (could be anything, when dragging an image we could decide to display the filename and a small preview of the image, etc.)
				ImGui::Text(_file.filenameString.c_str());
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor(1);
			ImGui::Text(_file.filenameString.c_str());
			ImGui::TableNextColumn();
		}

		void ContentBrowserWindow::RenameDirectory(IrisDirectory _directory)
		{
			std::string str = _directory.directoryNameString;
			char* saveText = new char[50];
			std::copy(str.begin(), str.end(), saveText);
			saveText[str.size()] = '\0';

			bool rename = false;

			if (ImGui::InputText(("##RenameDirectory" + _directory.directoryNameString).c_str(), saveText, 50, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				rename = true;
			}
			else if (!ImGui::IsItemActive() && ImGui::IsItemDeactivated())
			{
				rename = true;
			}

			if (rename)
			{
				std::string newName = saveText;
				std::filesystem::path newPath = std::filesystem::path(m_CurrentDirectory) / newName;
				std::filesystem::rename(std::filesystem::path(m_DirectoryToRename.path), newPath);
				m_DirectoryToRename.path = "";
				m_DirectoryToRename.directoryNameString = "";
				m_UpdateContent = true;
				m_UpdateContentBrowser = true;
			}

			delete[] saveText;
		}

		void ContentBrowserWindow::CopyDroppedFileToProject(std::filesystem::path _filePath)
		{
			std::filesystem::path srcPath(_filePath);
			std::filesystem::path dstPath = m_CurrentDirectory / srcPath.filename();

			try {
				if (!std::filesystem::exists(dstPath)) 
				{
					std::filesystem::copy_file(srcPath, dstPath);

					if (dstPath.has_extension())
					{
						std::vector<std::string> textureExtention = FileExtension::GetTextureExtention();

						auto texture = std::find(textureExtention.begin(), textureExtention.end(), dstPath.extension().string());

						if (texture != textureExtention.end())
						{
							Engine::TextureLoadingParameters parameters{};

							if (dstPath.extension().string() == ".hdr")
								parameters.ColorFormat = Engine::RHIColorFormat::IE_RHI_FORMAT_R32G32B32A32_SFLOAT;

							if(!Engine::ResourceManager::GetInstance()->Get<Engine::Texture>(dstPath.filename().string()))
								Engine::ResourceManager::GetInstance()->Create<Engine::Texture>(FIND_PATH(dstPath.filename().string()), true, parameters);
						}
						else
						{
							std::vector<std::string> meshExtention = FileExtension::GetMeshExtention();

							auto mesh = std::find(meshExtention.begin(), meshExtention.end(), dstPath.extension().string());

							if (mesh != meshExtention.end())
							{
								if (!Engine::ResourceManager::GetInstance()->Get<Engine::Mesh>(dstPath.filename().string()))
									Engine::ResourceManager::GetInstance()->Create<Engine::Mesh>(FIND_PATH(dstPath.filename().string()));
							}
						}
					}
				}
				else 
				{
					DEBUG_WARN(("File already exists: " + dstPath.string()).c_str());
				}
			}
			catch (const std::filesystem::filesystem_error& e) 
			{
				std::string error = "Error copying file: " + std::string(e.what());
				DEBUG_ERROR( error.c_str() );
			}

			UpdateCurrentContent();
		}

		void ContentBrowserWindow::DragDropFolders(IrisDirectory* _directory)
		{
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				// Set payload to carry the index of our item (could be anything)
				std::string pathStr = _directory->path;
				ImGui::SetDragDropPayload("DragDropDirectory", pathStr.c_str(), pathStr.size() + 1);

				// Display preview
				ImGui::Text(_directory->directoryNameString.c_str());
				ImGui::EndDragDropSource();
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropDirectory"))
				{
					IM_ASSERT(payload->DataSize > 0);
					const char* payload_n = static_cast<const char*>(payload->Data);

					std::string path(payload_n);

					IrisDirectory folder;
					folder.path = path;
					folder.directoryNameString = std::filesystem::path(path).filename().string();

					if (folder.path == _directory->path)
					{
						DEBUG_WARN("Can't put a file inside itself");
						return;
					}

					try
					{
						std::filesystem::path newPath = std::filesystem::path(_directory->path) / folder.directoryNameString;
						std::filesystem::rename(std::filesystem::path(folder.path), newPath);
						std::string text = folder.path + " is now at " + newPath.string();
						DEBUG_LOG(text.c_str());
						m_UpdateContent = true;
						m_UpdateContentBrowser = true;
					}
					catch (const std::filesystem::filesystem_error& e)
					{
						std::string text = "Error : " + std::string(e.what());
						DEBUG_ERROR(text.c_str());
					}
				}
				ImGui::EndDragDropTarget();
			}
		}

		void ContentBrowserWindow::DropFiles(IrisDirectory* _directory)
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropFile"))
				{
					IM_ASSERT(payload->DataSize > 0);
					const char* payload_n = static_cast<const char*>(payload->Data);

					std::string path(payload_n);

					IrisFile file;
					file.path = path;
					file.filenameString = std::filesystem::path(path).filename().string();

					try
					{
						std::filesystem::path newPath = std::filesystem::path(_directory->path) / file.filenameString;
						std::filesystem::rename(std::filesystem::path(file.path), newPath);
						std::string text = file.path + " is now at " + newPath.string();
						DEBUG_LOG(text.c_str());
						m_UpdateContent = true;
						m_UpdateContentBrowser = true;
					}
					catch (const std::filesystem::filesystem_error& e)
					{
						std::string text = "Error : " + std::string(e.what());
						DEBUG_ERROR(text.c_str());
					}
				}
				ImGui::EndDragDropTarget();
			}
		}

		void ContentBrowserWindow::ContentBrowserPopup()
		{
			if (m_OpenRenamePopup)
			{
				std::string popupName = "RenameFolder##" + m_SelectedDirectory.directoryNameString;

				if (!m_RenamePopupOpened)
				{
					ImGui::OpenPopup(popupName.c_str());
				}

				if (ImGui::BeginPopup(popupName.c_str()))
				{
					m_RenamePopupOpened = true;
					if (ImGui::MenuItem("Rename##RenameDirectory"))
					{
						m_DirectoryToRename.path = m_SelectedDirectory.path;
						m_DirectoryToRename.directoryNameString = m_SelectedDirectory.directoryNameString;
					}
					ImGui::EndPopup();
				}
				else if (m_RenamePopupOpened)
				{
					m_OpenRenamePopup = false;
					m_RenamePopupOpened = false;
				}
			}
			else
			{
				if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					ImGui::OpenPopup("ContentOptionPopup");
				}
				if (ImGui::BeginPopup("ContentOptionPopup"))
				{
					if (ImGui::MenuItem("Add Folder"))
					{
						std::filesystem::create_directory(std::filesystem::path(m_CurrentDirectory) / "NewFolder");
						m_UpdateContent = true;
						m_UpdateContentBrowser = true;
					}
					ImGui::EndPopup();
				}
			}
		}

		void ContentBrowserWindow::RecursiveDeleteFolders(IrisDirectory* _directory)
		{
			for (IrisDirectory* childDirectory : _directory->folderChilds)
			{
				if (childDirectory->folderChilds.size() > 0)
				{
					RecursiveDeleteFolders(childDirectory);
				}
				delete childDirectory;
			}
			_directory->folderChilds.clear();
		}

		void ContentBrowserWindow::ReloadContentBrowser()
		{
			DeleteContentBrowser();
			RecursiveTreeNode(m_AssetDirectory);
			m_UpdateContentBrowser = false;
		}

		void ContentBrowserWindow::DeleteContentBrowser()
		{
			RecursiveDeleteFolders(m_AssetDirectory);
		}

		void ContentBrowserWindow::UpdateCurrentContent()
		{
			ReloadCurrentContent();

			for (std::filesystem::directory_entry directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
			{
				auto path = directoryEntry.path();
				auto relativePath = std::filesystem::relative(path, m_Path);
				std::string filenameString = relativePath.filename().string();
				std::string extension = path.extension().string();

				if (directoryEntry.is_directory())
				{
					IrisDirectory directoryStruct(path.string(), filenameString);
					m_CurrentFolders.push_back(directoryStruct);
				}
				else
				{
					Engine::Texture* texture = Engine::ResourceManager::GetInstance()->Get<Engine::Texture>(filenameString);

					IrisFile fileStruct(path.string(), filenameString, extension, texture ? (texture->GetID() != 0 ? (texture->GetImage()->GetTextureType() == Engine::RHITextureType::IE_RHI_TEXTURE_2D ? texture->GetID() : m_FileIcon) : m_FileIcon) : (extension == ".cs" ? m_CSFileIcon : m_FileIcon));
					m_CurrentFiles.push_back(fileStruct);
				}
			}

			m_UpdateContent = false;
		}

		void ContentBrowserWindow::ReloadCurrentContent()
		{
			m_CurrentFolders.clear();
			m_CurrentFiles.clear();
		}

		void ContentBrowserWindow::DragDropFromWindows()
		{
			if (!m_DroppedFiles.empty())
			{
				for (const auto& file : m_DroppedFiles)
				{
					CopyDroppedFileToProject(file);
				}
				m_DroppedFiles.clear();
				ReloadContentBrowser();
			}
		}

		void ContentBrowserWindow::LoadNotLoadedTextures()
		{
			size_t size = m_CurrentFiles.size();
			for (size_t i = 0; i < size; i++)
			{
				std::vector<std::string> textureExtention = FileExtension::GetTextureExtention();

				auto texture = std::find(textureExtention.begin(), textureExtention.end(), m_CurrentFiles[i].extension);

				if (texture != textureExtention.end())
				{
					Engine::TextureLoadingParameters parameters{};

					if (m_CurrentFiles[i].extension == ".hdr")
						parameters.ColorFormat = Engine::RHIColorFormat::IE_RHI_FORMAT_R32G32B32A32_SFLOAT;

					if (!Engine::ResourceManager::GetInstance()->Get<Engine::Texture>(m_CurrentFiles[i].filenameString))
						Engine::ResourceManager::GetInstance()->Create<Engine::Texture>(FIND_PATH(m_CurrentFiles[i].filenameString), true, parameters);
				}
			}
		}

		void ContentBrowserWindow::CheckAssetRootFileStatus()
		{
			HANDLE hChangeEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			OVERLAPPED overlapped = {};
			overlapped.hEvent = hChangeEvent;

			HANDLE handles[2] = { hChangeEvent, m_StopWatchingEvent };

			while (true)
			{
				BOOL success = ReadDirectoryChangesW(
					hDir,
					buffer,
					sizeof(buffer),
					TRUE,
					FILE_NOTIFY_CHANGE_FILE_NAME |
					FILE_NOTIFY_CHANGE_DIR_NAME |
					FILE_NOTIFY_CHANGE_LAST_WRITE |
					FILE_NOTIFY_CHANGE_CREATION,
					&bytesReturned,
					&overlapped,
					NULL
				);

				if (!success)
				{
					DEBUG_LOG("ReadDirectoryChangesW failed");
					break;
				}

				DWORD waitResult = WaitForMultipleObjects(2, handles, FALSE, INFINITE);

				if (waitResult == WAIT_OBJECT_0)
				{
					FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
					do {
						m_UpdateContentBrowser = true;
						m_UpdateContent = true;

						fni = fni->NextEntryOffset == 0
							? nullptr
							: reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
								reinterpret_cast<BYTE*>(fni) + fni->NextEntryOffset);
					} while (fni != nullptr);
				}
				else if (waitResult == WAIT_OBJECT_0 + 1)
				{
					DEBUG_LOG("File Watcher Stoped.");
					break;
				}
			}

			CloseHandle(hChangeEvent);
			m_LaunchedThread = false;
		}
	}
}