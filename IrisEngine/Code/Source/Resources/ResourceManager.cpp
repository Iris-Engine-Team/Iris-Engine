#include "Resources/ResourceManager.h"
#include "Resources/MetadataManager.h"
#include "CSharpScripting/ScriptManager.h"

namespace Iris
{
	namespace Engine
	{
		
		void ResourceManager::Init(ResourceContext& _resourceContext, const RenderingContext* _renderingContext, const IRenderer* _renderer, const ThreadManager* _threadManager, MetaDataManager* _metadataManager)
		{
			if (s_Instance)
			{
				DEBUG_WARN("Resource Manager instance already exist.");
				return;
			}
			
			s_Instance = new ResourceManager();
			s_Instance->m_Map = new ResourceMap();
			s_Instance->m_Loader = new IrisLoader();
			s_Instance->m_Loader->Init();
			s_Instance->m_ResourceContext = &_resourceContext;
			s_Instance->m_MaterialSystem = new MaterialSystem();
			s_Instance->m_MaterialSystem->Initialize(_renderer, _renderingContext, s_Instance->m_ResourceContext);
			
			s_Instance->m_ScriptManager = new ScriptManager();
			s_Instance->m_ThreadManagerRef = _threadManager;

			s_Instance->m_MetadataManagerRef = _metadataManager;

			
		}

		void ResourceManager::LoadMetadata()
		{
			std::filesystem::path path = FIND_PATH("Bin");
			std::filesystem::create_directory(path / "ResourceData");

			std::filesystem::path allMatPath = FIND_PATH("AllMaterials.meta");
			bool newCreate = false;
			if (allMatPath == "")
			{
				newCreate = true;
				allMatPath = "ResourceData\\AllMaterials.meta";
			}

			s_Instance->m_AllMaterialsToLoad = s_Instance->m_MetadataManagerRef->CreateMetadata(allMatPath, "AllMaterials");

			if (newCreate)
				s_Instance->m_AllMaterialsToLoad->Write("Data", std::vector<std::string>());

			s_Instance->m_AllMaterialsToLoad->Write("Type", MetadataResourceType::IE_MATERIAL_DATA);

			if (s_Instance->m_AllMaterialsToLoad->GetData("Data").size() == 0)
			{
				s_Instance->m_AllMaterialsToLoad->Update();
				return;
			}

			//load all materials
			std::vector<std::string> materialsName = s_Instance->m_AllMaterialsToLoad->GetData("Data");
			std::vector<std::string> removePath = { "Data" , "" };

			size_t size = materialsName.size();
			for (size_t i = 0; i < size; ++i)
			{
				path = FIND_PATH(materialsName[i] + ".meta");
				if (path == "")
				{
					s_Instance->m_AllMaterialsToLoad->RemoveAtInList("Data",i);
					
					materialsName.erase(materialsName.begin() + i);

					--size;
					--i;
				}
				else
				{
					Material* loadedMat = new Material();
					loadedMat->Deserialization(s_Instance->m_MetadataManagerRef->CreateMetadata(path, materialsName[i]));
					s_Instance->AddResource<Material>(loadedMat, materialsName[i]);
				}
			}

			s_Instance->m_AllMaterialsToLoad->Update();
		}

		void ResourceManager::Terminate()
		{
			if (s_Instance)
			{
				s_Instance->DeleteAll();
				delete s_Instance->m_Map;
				s_Instance->m_Map = nullptr;

				s_Instance->m_Loader->Terminate();
				delete s_Instance->m_Loader;
				s_Instance->m_Loader = nullptr;

				s_Instance->m_MaterialSystem->Terminate();
				delete s_Instance->m_MaterialSystem;
				
				delete s_Instance->m_ScriptManager;
				s_Instance->m_ScriptManager = nullptr;

				delete s_Instance;
				s_Instance = nullptr;
			}
		}

		const void ResourceManager::DeleteAll()
		{
			std::vector<Material*> mats = Get<Material>();

			for (const auto& mat : mats)
			{
				mat->Serialize();
			}

			for (auto& mapByType : m_Map->m_ResourceMap)
			{
				for (auto& resource : mapByType.second)
				{
					resource.second->UnloadCompiledData(m_ResourceContext);
					delete resource.second;
					DEBUG_LOG("The resource %s has been removed from the resource manager", resource.first.c_str());
				}
			}

			m_Map->m_ResourceMap.clear();
		}
	}
}