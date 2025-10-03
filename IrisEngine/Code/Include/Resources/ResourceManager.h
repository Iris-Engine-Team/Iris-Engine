#pragma once

#include "Resources/IResource.h"
#include "Resources/IResourceContainer.h"

#include "Loader/IrisLoader.h"
#include "Core/Graphics/ContextInitializer.h"

#include "Core/Graphics/Context/MaterialSystem.h"

#include <unordered_map>
#include <typeindex>
#include <filesystem>

#include "Core/MultiThread/ThreadManager.h"

namespace Iris
{
	namespace Engine
	{
		class ScriptManager;

		class IRenderer;
		class MetaDataManager;

		/// <summary>
		/// Structure used to store the unordered map used in the DLL
		/// </summary>
		struct ResourceMap
		{
			std::unordered_map<std::type_index, std::unordered_map<std::string, IResource*>> m_ResourceMap{};
		};

		class IRIS_ENGINE_API ResourceManager
		{
		public:
			/// <summary>
			/// Create the ResourceManager instance
			/// </summary>
			static void Init(ResourceContext& _resourceContext, const RenderingContext* _renderingContext, const IRenderer* _renderer, const ThreadManager* _threadManager, MetaDataManager* _metadataManager);

			static void LoadMetadata();

			/// <summary>
			/// Get the ResourceManager instance
			/// </summary>
			/// <returns>The ResourceManager instance</returns>
			IE_FORCE_INLINE static ResourceManager* GetInstance() { if (!s_Instance) DEBUG_LOG("ResourceManager instance not created."); return s_Instance; }
			
			/// <summary>
			/// Clear the resource manager
			/// </summary>
			static void Terminate();

			/// <summary>
			/// Creates a new resource in the manager
			/// </summary>
			/// <typeparam name="T">: Type of the new resource</typeparam>
			/// <param name="_resourcePath">: Path of the resource</param>
			/// <param name="_loadingParameters">: Loading parameters according to the resource</param>
			/// <returns>The new resource created or the already loaded resource</returns>
			template<typename T>
			T* Create(std::filesystem::path&& _resourcePath, bool _waitLoad = true, const LoadingParameters& _loadingParamaters = LoadingParameters())
			{
				if (_resourcePath.empty())
					return nullptr;

				std::filesystem::path resourcePath = _resourcePath;

				return Create<T>(resourcePath, _waitLoad, _loadingParamaters);
			}

			/// <summary>
			/// Creates a new resource in the manager
			/// </summary>
			/// <typeparam name="T">: Type of the new resource</typeparam>
			/// <param name="_resourcePath">: Path of the resource</param>
			/// <param name="_loadingParameters">: Loading parameters according to the resource</param>
			/// <returns>The new resource created or the already loaded resource</returns>
			template<typename T>
			T* Create(std::filesystem::path& _resourcePath,  bool _waitLoad = true, const LoadingParameters& _loadingParamaters = LoadingParameters())
			{
				std::string resourceName = _resourcePath.filename().string();

				if (!std::is_base_of<IResource, T>::value)
				{
					DEBUG_WARN("Can't add the resource %s because it has a wrong type.", resourceName.c_str());
					return nullptr;
				}

				std::type_index type = typeid(T);

				if (m_Map->m_ResourceMap[type].find(resourceName) != m_Map->m_ResourceMap[type].end())
				{
					return Get<T>(resourceName);
				}

				T* newResource = new T();
				LoadedData* newLoadedData = CreateResourceData<T>();

				if (newLoadedData)
				{
					std::future<void> loadTask = m_ThreadManagerRef->SubmitTask([this, ResourcePath = _resourcePath, &_loadingParamaters, newResource, LoadedData = newLoadedData, &resourceName]()
						{
							m_Loader->GetLoader<T>()->Load(LoadedData, ResourcePath.string(), _loadingParamaters);
							newResource->LoadCompiledData(LoadedData, m_ResourceContext, _loadingParamaters);
							m_Loader->GetLoader<T>()->Unload(LoadedData);
							DeleteResourceData(LoadedData);
						});

					if (_waitLoad)
						loadTask.wait();
				}

				if (newLoadedData || (!newLoadedData && _resourcePath.filename().extension() == ".cs"))
				{
					newResource->SetName(resourceName);
					AddResource<T>(newResource, resourceName);
					return newResource;
				}

				bool result = m_Loader->GetLoader<T>()->Load(newLoadedData, _resourcePath.string(), _loadingParamaters);

				if (!result)
				{
					delete newLoadedData;
					delete newResource;
					
					return nullptr;
				}

				newResource->LoadCompiledData(newLoadedData, m_ResourceContext, _loadingParamaters);
				m_Loader->GetLoader<T>()->Unload(newLoadedData);
				DeleteResourceData(newLoadedData);
				return newResource;
				

			}

			/// <summary>
			/// Creates a container with resources (Ex: Material)
			/// </summary>
			/// <typeparam name="T">: Type of the container</typeparam>
			/// <param name="_resourceData">: Data stored in the container</param>
			/// <returns>The new resource container created or the already loaded resource</returns>
			template<typename T>
			T* CreateResourceContainer(ContainerData* _resourceData)
			{
				std::string resourceName = _resourceData->ContainerName;

				if (!std::is_base_of<IResourceContainer, T>::value)
				{
					DEBUG_WARN("Can't add the resource container %s because it has a wrong type.", resourceName.c_str());
					return nullptr;
				}

				std::type_index type = typeid(T);

				if (m_Map->m_ResourceMap[type].find(resourceName) != m_Map->m_ResourceMap[type].end())
				{
					return Get<T>(resourceName);
				}

				T* newResourceContainer = new T();
				
				newResourceContainer->CreateContainer(_resourceData, m_ResourceContext);

				AddResource<T>(newResourceContainer, resourceName);

				return newResourceContainer;
			}

			/// <summary>
			/// Gets a resource stocked in the manager specified by its name and its type
			/// </summary>
			/// <typeparam name="T"> : Type of the resource </typeparam>
			/// <param name="_resourceName"> : Name of the resource </param>
			/// <returns>The resource pointer or null if function or research failed</returns>
			template<typename T>
			T* Get(std::string& _resourceName)
			{
				std::type_index type = typeid(T);

				if (m_Map->m_ResourceMap.find(type) == m_Map->m_ResourceMap.end())
				{
					DEBUG_WARN("Can't get the resource %s because it has a wrong type or a type hasn't been loaded yet.", _resourceName.c_str());
					return nullptr;
				}

				for (const auto& resource : m_Map->m_ResourceMap[type])
				{
					if (resource.first == _resourceName)
						return dynamic_cast<T*>(resource.second);
				}

				DEBUG_WARN("Can't get resource %s try an other name or type.", _resourceName.c_str());
				return nullptr;
			}

			template<typename T>
			T* Get(std::string&& _resourceName)
			{
				std::string name = _resourceName;
				return Get<T>(name);
			}

			/// <summary>
			/// Gets all resources of a specific type stocked in the manager
			/// </summary>
			/// <typeparam name="T"> : Type of the resource </typeparam>
			/// <returns>A vector of resource pointers</returns>
			template<typename T>
			std::vector<T*> Get()
			{
				std::type_index type = typeid(T);
				std::vector<T*> values(0);

				if (m_Map->m_ResourceMap.find(type) == m_Map->m_ResourceMap.end())
				{
					DEBUG_WARN("Can't get the resources of type %s because it has a wrong type or a type hasn't been loaded yet.", type.name());
					return values;
				}

				for (const auto& resource : m_Map->m_ResourceMap[type])
					values.push_back(dynamic_cast<T*>(resource.second));

				return values;
			}

			/// <summary>
			/// Deletes a certain resource specified by the name
			/// </summary>
			/// <typeparam name="T"> : Type of the resource </typeparam>
			/// <param name="_resourceName"> : Name of the resource you want to delete </param>
			/// <returns>True if the resource has been destroyed and false otherwise</returns>
			template<typename T>
			const bool Delete(std::string& _resourceName)
			{
				std::type_index type = typeid(T);

				if (m_Map->m_ResourceMap.find(type) == m_Map->m_ResourceMap.end())
				{
					DEBUG_WARN("Can't delete the resource of type %s because it has a wrong type or a type hasn't been loaded yet.", type.name());
					return false;
				}

				for (auto& resource : m_Map->m_ResourceMap[type])
				{
					if (resource.first == _resourceName)
					{
						resource.second->UnloadCompiledData(m_ResourceContext);
						delete resource.second;
						m_Map->m_ResourceMap.erase(resource);

						DEBUG_LOG("Resource %s has been deleted to the resource manager.", _resourceName.c_str());
						return true;
					}
				}

				DEBUG_WARN("Can't delete the resource %s try an other name or type.", _resourceName.c_str())
					return false;
			}

			/// <summary>
			/// Deletes the resource in the manager
			/// </summary>
			/// <typeparam name="T"> : Type of the resource </typeparam>
			/// <param name="_resource"> : Name of the resource </param>
			/// <returns>True if the resource has been destroyed and false otherwise</returns>
			template<typename T>
			const bool Delete(IResource* _resource)
			{
				std::type_index type = typeid(T);

				if (m_Map->m_ResourceMap.find(type) == m_Map->m_ResourceMap.end())
				{
					DEBUG_WARN("Can't delete the resources of type %s because it has a wrong type or a type hasn't been loaded yet.", type.name());
					return false;
				}

				for (auto& resource : m_Map->m_ResourceMap[type])
				{
					if (resource.second == _resource)
					{
						DEBUG_LOG("Resource %s has been deleted to the resource manager.", resource.first.c_str());

						resource.second->UnloadCompiledData(m_ResourceContext);
						delete resource.second;
						m_Map->m_ResourceMap.erase(resource);

						return true;
					}
				}

				std::stringstream ss;
				ss << _resource;
				std::string name = ss.str();

				DEBUG_WARN("Can't delete the resource pointer %s try an other type or pointer.", name.c_str())
					return false;
			}

			/// <summary>
			/// Deletes all resources
			/// </summary>
			/// <returns></returns>
			const void DeleteAll();

			MaterialSystem* GetMaterialSystem() const { return m_MaterialSystem; }
			MetaDataManager* GetMetadataManager() const { return m_MetadataManagerRef; }


			ScriptManager* GetScriptManager() { return m_ScriptManager; }

		private:
			ResourceManager() = default;
			/// <summary>
			/// ResourceManager destructor
			/// </summary>
			~ResourceManager() = default;

			/// <summary>
			/// Method used to add a new resource in the manager
			/// </summary>
			/// <typeparam name="T">: Type of the resource</typeparam>
			/// <param name="_resource">: Pointer to the resource</param>
			/// <param name="_resourceName">: Name of the resource</param>
			template<typename T>
			void AddResource(IResource* _resource, std::string& _resourceName)
			{
				std::type_index type = typeid(T);

				if (m_Map->m_ResourceMap.find(type) == m_Map->m_ResourceMap.end())
					m_Map->m_ResourceMap.emplace(type, std::unordered_map<std::string, IResource*>{});

				m_Map->m_ResourceMap[type].try_emplace(_resourceName, _resource);

				DEBUG_LOG("Resource %s has been added to the resource manager.", _resourceName.c_str());
			}

			/// <summary>
			/// Allocates memory for a LoadedData struct to easily pass loaded data
			/// </summary>
			/// <typeparam name="T">: Type of the loaded data</typeparam>
			/// <returns>The pointer on the allocated struct</returns>
			template<typename T>
			LoadedData* CreateResourceData()
			{
				std::type_index type = typeid(T);

				LoadedData* data = nullptr;

				if (type == typeid(Mesh))
				{
					data = new MeshLoadedData;
					return data;
				}
				else if (type == typeid(Texture))
				{
					data = new TextureLoadedData;
					return data;
				}
				else if (type == typeid(Shader))
				{
					data = new ShaderLoadedData;
					return data;
				}

				return nullptr;
			}

			/// <summary>
			/// Deletes an allocated pointer of the LoadedData struct
			/// </summary>
			/// <param name="_resourceData">: Pointer to delete</param>
			void DeleteResourceData(LoadedData* _resourceData)
			{
				delete _resourceData;
				_resourceData = nullptr;
			}

		private:
			static inline ResourceManager* s_Instance = nullptr;
			ResourceMap* m_Map = nullptr;
			IrisLoader* m_Loader = nullptr;
			ResourceContext* m_ResourceContext = nullptr;
			MaterialSystem* m_MaterialSystem = nullptr;
			ScriptManager* m_ScriptManager = nullptr;
			const ThreadManager* m_ThreadManagerRef = nullptr;
			
			MetaDataManager* m_MetadataManagerRef = nullptr;
			IMetadata* m_AllMaterialsToLoad = nullptr;
		};
	}
}