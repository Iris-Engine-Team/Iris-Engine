#pragma once
#include <string>
#include <vector>
#include <string>
#include "Core/Tools/Macros.h"

#include <array>
#include "SECS/Components/ComponentID.h"

namespace Iris
{
	namespace Engine
	{
		class ITransform;
		class Scene;
		struct InitEditorContext;
		
		class IComponentHandle;

		template<class T>
		class ComponentHandle;

		class IComponent;

		class IMetadata;
		
		class IrisObject
		{
		public:

			IRIS_ENGINE_API IrisObject(Scene* _scene);
			IrisObject(Scene* _scene, IMetadata* _sceneData, std::string _id, InitEditorContext& _initializer);
			IRIS_ENGINE_API ~IrisObject();

			/// <summary>
			/// Write data in the given metadata
			/// </summary>
			/// <param name="_metadata"></param>
			void Serialize(IMetadata* _metadata);

			IRIS_ENGINE_API void SetActive(bool _activate);
			IRIS_ENGINE_API IE_FORCE_INLINE bool IsActive() const { return m_Active; }

			/// <summary>
			/// Removes all the component and add the iris object to the 'to destroy list' of the scene
			/// </summary>
			IRIS_ENGINE_API void Destroy(); // CS

			/// <summary>
			/// Duplicate the irisObject (and his children)
			/// </summary>
			IRIS_ENGINE_API void Duplicate(InitEditorContext* _initializer, ITransform* _parent);
			
			/// <returns>If object is already destroyed</returns>
			IE_FORCE_INLINE bool IsDestroyed() const { return m_Destroyed; }

			/// <returns>The unique id of the object</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE uint64_t GetID() const { return m_UniqueID; }

			/// <summary>
			/// Modify the name of the object
			/// </summary>
			/// <param name="_name">: new name</param>
			IRIS_ENGINE_API IE_FORCE_INLINE void SetName(const std::string& _name) { m_Name = _name; }

			/// <returns>The name with the id of the object</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE  std::string GetName() const { return m_Name; }
			IE_FORCE_INLINE  const char* GetNamePtr() const { return m_Name.c_str(); }

			/// <returns>The transform component of the object</returns>
			IRIS_ENGINE_API ITransform* GetTransform();

			/// <typeparam name="T">: type of component</typeparam>
			/// <returns>The type T component</returns>
			template <class T>
			IRIS_ENGINE_API IE_FORCE_INLINE T* GetComponent();

			/// <returns>The component or nullptr if the object doesn't have this component</returns>
			IRIS_ENGINE_API IComponent* GetComponent(ComponentID _componentId);

			/// <summary>
			/// Adds a component of the given type to the object
			/// </summary>
			/// <typeparam name="T">: type of component</typeparam>
			/// <returns>The new component added</returns>
			template <class T>
			IRIS_ENGINE_API IE_FORCE_INLINE T* AddComponent();

			/// <summary>
			/// Removes a component of the given type
			/// </summary>
			template <class T>
			IRIS_ENGINE_API IE_FORCE_INLINE void RemoveComponent();

			IRIS_ENGINE_API IE_FORCE_INLINE void SetShowInHierarchy(bool show) { m_ShowInHierarchy = show; }
			IRIS_ENGINE_API IE_FORCE_INLINE bool GetShowInHierarchy() const { return m_ShowInHierarchy; }

			/// <summary>
			/// Adds a handle of the new component added
			/// </summary>
			template<class T>
			IE_FORCE_INLINE void AddHandle(ComponentHandle<T>* _newHandle);

			/// <summary>
			/// Get a handle pointer of the specified component
			/// </summary>
			/// <typeparam name="T">: type of return handle</typeparam>
			/// <typeparam name="F">: type of needed component</typeparam>
			/// <returns>The pointer of th cpecified component</returns>
			template<class T, class F>
			IE_FORCE_INLINE T* GetHandle();

			/// <returns>A list of all components handle</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE std::array<IComponentHandle*, static_cast<size_t>(ComponentID::IE_NUM_COMPONENT)> GetAllComponents() const { return m_Components; }

		private:
			/// <summary>
			/// Destroy child object
			/// </summary>
			IRIS_ENGINE_API void DestroyChildObject();
		
		private:
			/// <summary>
			/// Handles of all components
			/// </summary>
			//std::vector<IComponentHandle*> m_Components;
			std::array<IComponentHandle*, static_cast<size_t>(ComponentID::IE_NUM_COMPONENT)> m_Components;

			/// <summary>
			/// Scene ref
			/// </summary>
			Scene* m_SceneRef = nullptr;

			/// <summary>
			/// Name displayed in the editor
			/// </summary>
			std::string m_Name = "IrisObject";

			/// <summary>
			/// Unique ID
			/// </summary>
			uint64_t m_UniqueID = 0;

			/// <summary>
			/// true -> update object, false -> don't update
			/// </summary>
			bool m_Active = true;

			/// <summary>
			/// If the object appears in the hierarchy
			/// </summary>
			bool m_ShowInHierarchy = true;

			/// <summary>
			/// If object is destroyed
			/// </summary>
			bool m_Destroyed = false;
		};
	}
}

#include "SECS/IrisObject.inl"