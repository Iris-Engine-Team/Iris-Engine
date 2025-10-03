#pragma once
#include "Core/Tools/Macros.h"
#include "Resources/ResourceManager.h"
#include <string>

#include "SECS/Components/ComponentID.h"

namespace Iris
{
	namespace Engine
	{
		class IMetadata;
		class ScriptManager;

		class IrisObject;
		class IRenderer;
		class ResourceContext;
		class RenderingContext;
		class SceneManager;
		class Scene;
		class Mesh;
		class Material;
		class IPipeline;

		struct InitEditorContext
		{
			SceneManager* SceneManager;
			//InitCam & InitLight
			IRenderer* InitRenderer;
			ResourceContext* InitResourceContext;
			RenderingContext* InitRenderingContext;
			//Collider
			Scene* InitScene;
			//Model
			Mesh* InitMesh;
			Material* InitMat;
			IPipeline* InitPipeline;
		};

		//using ComponentId = uint16_t;

		class IComponent
		{
		public:
			IRIS_ENGINE_API virtual ~IComponent() = default;

			/// <summary>
			/// Writes data in the given metadata
			/// </summary>
			/// <param name="_metadata"></param>
			virtual void Serialize(IMetadata* _metadata) = 0;

			/// <summary>
			/// Init the components with the given data
			/// </summary>
			virtual void Deserialization(IMetadata* _metadata, InitEditorContext& _initializer) = 0;

			/// <summary>
			/// Init function called from the editor
			/// </summary>
			IRIS_ENGINE_API virtual void Init(InitEditorContext& _initializer) = 0;
			IRIS_ENGINE_API virtual void Copy(IComponent* _componentToCopy) { p_Enable = _componentToCopy->p_Enable; }
			IRIS_ENGINE_API virtual void Duplicate(IComponent* _componentToDuplicate, InitEditorContext* _initializer) {p_Enable = _componentToDuplicate->p_Enable; _initializer;}
			IRIS_ENGINE_API virtual void Destroy() = 0;
			IRIS_ENGINE_API IE_FORCE_INLINE virtual const ComponentID GetTypeID() const { return ComponentID::IE_NUM_COMPONENT; }
			IRIS_ENGINE_API IE_FORCE_INLINE IrisObject* GetIrisObject() { return p_IrisObject; }
			IRIS_ENGINE_API void SetOwner(IrisObject* _owner);

			IRIS_ENGINE_API IE_FORCE_INLINE virtual void SetEnable(bool _enable) { p_Enable = _enable; }
			IRIS_ENGINE_API IE_FORCE_INLINE bool IsEnable() const { return p_Enable; }

		protected:
			IrisObject* p_IrisObject = nullptr;
			bool p_Enable = true;
		};

		#define DEFINE_COMPONENT(T) \
		static const std::string GetComponentName() { return T::GetName();}  \
		IE_FORCE_INLINE const ComponentID GetTypeID() const override { return T::GetID(); }

		/// <summary>
		/// This class allows to keep a dynamic reference to the component to get it when ever you want
		/// </summary>
		class IComponentHandle
		{
		public:
			virtual ~IComponentHandle() = default;

			virtual void SetEnableComponent(bool _enable) = 0;
			virtual bool IsEnableComponent() = 0;

			IE_FORCE_INLINE ComponentID GetType() const { return p_Type; }
			IE_FORCE_INLINE IrisObject* GetIrisObject() const { return p_Obj; }
			IE_FORCE_INLINE std::string GetComponentName() const { return p_ComponentName; }
			virtual void RemoveComponent() = 0;

		protected:
			
			/// <summary>
			/// The owner of the component
			/// </summary>
			IrisObject* p_Obj = nullptr;

			/// <summary>
			/// The type id of the component
			/// </summary>
			ComponentID p_Type = ComponentID::IE_NUM_COMPONENT;

			/// <summary>
			/// 
			/// </summary>
			std::string p_ComponentName = "";
		};

		template <class T>
		class ComponentHandle : public IComponentHandle
		{
		public:
			~ComponentHandle() final override = default;

			IRIS_ENGINE_API IE_FORCE_INLINE void SetEnableComponent(bool _enable) final override;
			IRIS_ENGINE_API IE_FORCE_INLINE bool IsEnableComponent() final override;

			IRIS_ENGINE_API IE_FORCE_INLINE ComponentHandle(T* _component);
			IRIS_ENGINE_API IE_FORCE_INLINE ComponentHandle(IrisObject* _irisObject);

			/// <summary>
			/// Removes the component from the object
			/// </summary>
			IRIS_ENGINE_API IE_FORCE_INLINE void RemoveComponent() final override;

			/// <summary>
			/// Get the component pointer
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API IE_FORCE_INLINE T* UseComponent();
		};
	}
}

#include "SECS/Components/IComponent.inl"