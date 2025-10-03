#pragma once
#include <unordered_map>

#include "Vector/Vector3.h"
#include "Quaternion/Quaternion.h"
#include "Core/Tools/Macros.h"
#include "SECS/LightData/LightData.h"

#include "Core/MultiThread/ThreadManager.h"
#include "SECS/Components/ComponentID.h"

namespace Iris
{
	namespace Engine
	{
		class MetaDataManager;
		class IMetadata;

		// ~~~ Forward declarations render ~~~
		class IRenderer;
		class LightComponent;
		class IBuffer;
		class IDescriptor;
		class ResourceContext;
		class IDescriptorLayout;
		class ContextInitializer;
		//

		// ~~~ Forward declarations ECS ~~~
		class IrisObject;
		class ITransform;
		class CameraComponent;
	
		class IComponent;
		class IComponentTable;

		template<class T>
		class ComponentTable;

		class IComponentHandle;

		template<class T>
		class ComponentHandle;

		class RenderSystem;
		class CameraSystem;
		class IPhysicsSystem;
		class ScriptSystem;

		struct InitEditorContext;

		class Scene
		{
		public:
			IRIS_ENGINE_API Scene(std::string _name);
			
			void Load(InitEditorContext& _initializer);

			void ReLoad(InitEditorContext& _initializer);

			void Save(MetaDataManager* _metadataManager);

			IRIS_ENGINE_API ~Scene();

			IRIS_ENGINE_API IE_FORCE_INLINE std::string GetSceneName() const { return m_Name; }
			IE_FORCE_INLINE const char* GetSceneNamePtr() { return m_Name.c_str(); }

			/// <summary>
			/// Init function
			/// </summary>
			/// <param name="_descriptorLayout"></param>
			/// <param name="_resourceContext"></param>
			/// <returns></returns>
			IRIS_ENGINE_API void Init(IMetadata* _sceneMetadata, const ThreadManager* _threadManager, ResourceContext* _resourceContext
				, CameraSystem* _cameraSystem, RenderSystem* _renderSystem, IPhysicsSystem* _physicsSystem, ScriptSystem* _scriptSystem);

			/// <summary>
			/// Instantiate a new object in the scene
			/// </summary>
			/// <param name="_position">: global position of the object</param>
			/// <param name="_parent">: parent that will be set to the object</param>
			/// <param name="_name">: name in the editor</param>
			/// <returns>The instantiated object</returns>
			IRIS_ENGINE_API IrisObject* Instantiate(const Vector3& _position, ITransform* _parent = nullptr, std::string _name = "");

			/// <summary>
			/// Instantiate a new object in the scene
			/// </summary>
			/// <param name="_position">: global position of the object</param>
			/// <param name="_rotation">: global rotation of the object</param>
			/// <param name="_scale">: global scale of the object</param>
			/// <param name="_parent">: parent that will be set to the object</param>
			/// <param name="_name">: name in the editor</param>
			/// <returns>The instantiated object</returns>
			IRIS_ENGINE_API IrisObject* Instantiate(const Vector3& _position, const Quaternion& _rotation, const Vector3& _scale, ITransform* _parent = nullptr, std::string _name = "");

			/// <summary>
			/// Instantiate a new object in the scene			
			/// </summary>
			/// <param name="_position">: global position of the object</param>
			/// <param name="_rotation">: global euler angles (degrees) of the object</param>
			/// <param name="_scale">: global scale of the object</param>
			/// <param name="_parent">: parent that will be set to the object</param>
			/// <param name="_name">: name in the editor</param>
			/// <returns>The instantiated object</returns>
			IRIS_ENGINE_API IrisObject* Instantiate(const Vector3& _position, const Vector3& _rotation, const Vector3& _scale, ITransform* _parent = nullptr, std::string _name = "");
			
			/// <summary>
			/// User can call this function to destroy a gameObject
			/// </summary>
			IRIS_ENGINE_API void DestroyIrisObject(IrisObject* _irisObject);

			/// <summary>
			/// Add the IrisObject to ""
			/// </summary>
			IE_FORCE_INLINE void RemoveIrisObject(IrisObject* _irisObject) { m_IrisObjectToDestroy.emplace_back(_irisObject); }
			
			IRIS_ENGINE_API IrisObject* GetObjectByID(uint64_t _objectID);

			IRIS_ENGINE_API IrisObject* GetObjectByName(std::string _name);

			IPhysicsSystem* GetPhysicsSystem() const { return m_PhysicsSystemRef; }
			RenderSystem* GetRenderSystem() const { return m_RenderSystemRef; }
			CameraSystem* GetCameraSystem() const { return m_CameraSystemRef; }
			ScriptSystem* GetScriptSystem() const { return m_ScriptSystemRef; }
			
			IRIS_ENGINE_API void Update(double _deltaTime);

			IRIS_ENGINE_API void FixedUpdate(double _fixedDeltaTime);

			/// <summary>
			/// Destroy function
			/// </summary>
			IRIS_ENGINE_API void Destroy();

			template <class T>
			ComponentTable<T>* AddTable();

			IComponentTable* AddTable(ComponentID _componentId);

			template <class T>
			bool ContainTable() const;

			template <class T>
			ComponentTable<T>* GetComponentTable();

			template <class T>
			IRIS_ENGINE_API IE_FORCE_INLINE T* GetComponent(IrisObject* _irisObjectID);

			/// <summary>
			/// Better to use the templated function of GetComponent
			/// </summary>
			/// <param name="_irisObjectID"></param>
			/// <param name="_componentId"></param>
			/// <returns>The component if found or nullptr if not</returns>
			IRIS_ENGINE_API IComponent* GetComponent(IrisObject* _irisObjectID, ComponentID _componentId);

			template <class T>
			IRIS_ENGINE_API IE_FORCE_INLINE T* AddComponent(IrisObject* _irisObjectID);

			/// <summary>
			/// Better to use the templated function of AddComponent
			/// </summary>
			/// <param name="_irisObjectID"></param>
			/// <param name="_componentId"></param>
			/// <returns>The component if added or nullptr if not</returns>
			IRIS_ENGINE_API IComponent* AddComponent(IrisObject* _irisObjectID, ComponentID _componentId);

			template <class T>
			IRIS_ENGINE_API IE_FORCE_INLINE void RemoveComponent(IrisObject* _irisObjectID);

			/// <summary>
			///
			/// </summary>
			/// <returns>The root of the scene</returns>
			IRIS_ENGINE_API  IrisObject* GetRoot() const;

			IRIS_ENGINE_API IE_FORCE_INLINE  void SetSelectedObject(IrisObject* _selectedObject) { m_SelectedObject = _selectedObject; }
			IRIS_ENGINE_API IE_FORCE_INLINE IrisObject* GetSelectedObject() { return m_SelectedObject; }

			IE_FORCE_INLINE const ThreadManager* GetThreadManager() const { return m_ThreadManagerRef; }

			IE_FORCE_INLINE uint64_t GenIrisObjectID() { uint64_t id = s_IdCounter; ++s_IdCounter; return id; }

			IE_FORCE_INLINE bool IsLoaded() const { return m_IsLoaded; }
		private:
			/// <summary>
			/// Function called by the Instantiate funcs, creates and names an iris object
			/// </summary>
			/// <param name="_name">: name in the editor</param>
			/// <returns>The instantiated object</returns>
			IrisObject* BaseInstantiate(std::string _name = "");
			
			/// <summary>
			/// Check if there are any objects or components to destroy from the scene
			/// </summary>
			void CheckRemoveLists();

			/// <summary>
			/// Check if there are any objects or components to add in the scene
			/// </summary>
			void CheckAddLists();

		private:

			/// <summary>
			/// Counter for unique ID
			/// </summary>
			static uint64_t s_IdCounter;

			/// <summary>
			/// List of all iris object in scene
			/// </summary>
			std::vector<IrisObject*> m_AllIrisObjects;
			
			/// <summary>
			/// Root object of the scene
			/// </summary>
			IrisObject* m_Root = nullptr;

			/// <summary>
			/// List components to add
			/// </summary>
			std::vector<IComponent*> m_ComponentsToAdd;

			/// <summary>
			/// List of irisObject to add
			/// </summary>
			std::vector<IrisObject*> m_IrisObjectToAdd;

			/// <summary>Scene.inl
			/// List of components to destroy
			/// </summary>
			std::vector<IComponentHandle*> m_ComponentsToDestroy;

			/// <summary>
			/// List of irisObject to destroy
			/// </summary>
			std::vector<IrisObject*> m_IrisObjectToDestroy;

			/// <summary>
			/// <para>Map of all component tables</para>
			/// <para>-	string: type of Component</para>
			/// <para>-	IComponentTable*: tables of components</para>
			/// </summary>
			std::unordered_map <ComponentID, IComponentTable*> m_AllComponents;
			
			IrisObject* m_SelectedObject = nullptr;

			/// <summary>
			/// Scene name
			/// </summary>
			std::string m_Name = "";

			ResourceContext* m_ResourceContextRef = nullptr;

			CameraSystem* m_CameraSystemRef = nullptr;
			RenderSystem* m_RenderSystemRef = nullptr;
			IPhysicsSystem* m_PhysicsSystemRef = nullptr;
			ScriptSystem* m_ScriptSystemRef = nullptr;

			IMetadata* m_SceneMetadataRef = nullptr;
			const ThreadManager* m_ThreadManagerRef = nullptr;

			bool m_IsLoaded = false;
		};
	}
}

#include "SECS/Scene.inl"