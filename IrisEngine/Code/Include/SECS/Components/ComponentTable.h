#pragma once
#include "Core/Tools/Macros.h"
#include "unordered_map"
#include <vector>
#include <string>

#include "IComponent.h"

namespace Iris
{
	namespace Engine
	{
		class MetaDataManager;
		class IMetadata;
		class IrisObject;

		class IComponentTable
		{
		public:
			virtual ~IComponentTable() = default;

			virtual void Destroy() = 0;

			/// <summary>
			/// Writes data in the given metadata
			/// </summary>
			/// <param name="_metadataManager">: allows to instantiate metadata</param>
			/// <param name="_sceneMetadata"></param>
			virtual void Serialize(MetaDataManager* _metadataManager, IMetadata* _sceneMetadata) = 0;

			/// <summary>
			/// Init the components with the given data
			/// </summary>
			virtual void Deserialization(IMetadata* _metadata, InitEditorContext& _initializer) = 0;

			/// <returns>The number of valid component</returns>
			virtual size_t ComponentCount() = 0;

			/// <summary>
			/// Adds a component to the given object
			/// </summary>
			/// <param name="_irisObjectID"></param>
			IRIS_ENGINE_API virtual IComponent* AddComponent(IrisObject* _irisObject) = 0;

			/// <summary>
			/// Removes the component of the given object 
			/// </summary>
			/// <param name="_irisObjectID"></param>
			IRIS_ENGINE_API virtual void RemoveComponent(IrisObject* _irisObject) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_irisObjectID"></param>
			/// <returns>The wanted component of the object specify, nullptr if the object doesn't have the component</returns>
			IRIS_ENGINE_API virtual IComponent* GetComponent(IrisObject* _irisObject) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_irisObject">: IrisObject to find</param>
			/// <returns>the index of the component, -1 if not found</returns>
			IRIS_ENGINE_API virtual int32_t ContainsEntity(IrisObject* _irisObject) = 0;
		};

		template <class T>
		class ComponentTable : public IComponentTable
		{
		public:

			IE_FORCE_INLINE void Destroy() override;

			~ComponentTable() override = default;

			/// <summary>
			/// Writes data in the given metadata
			/// </summary>
			/// <param name="_metadataManager">: allows to instantiate metadata</param>
			/// <param name="_sceneMetadata"></param>
			void Serialize(MetaDataManager* _metadataManager, IMetadata* _sceneMetadata) override;

			/// <summary>
			/// Init the components with the given data
			/// </summary>
			void Deserialization(IMetadata* _metadata, InitEditorContext& _initializer) final override;

			/// <returns>The number of valid component</returns>
			IE_FORCE_INLINE size_t ComponentCount() final override;

			/// <summary>
			/// Check if the iris object has this type of component
			/// </summary>
			/// <param name="_irisObject"></param>
			/// <returns>if iris object has one return the index in the table of component, -1 if not</returns>
			IE_FORCE_INLINE int32_t ContainsEntity(IrisObject* _irisObject) override;

			/// <summary>
			/// 
			/// </summary>
			/// <returns>The gap between the first index of deleted component and the end of the vector</returns>
			IE_FORCE_INLINE int32_t GetGapFirstDestroy();

			/// <summary>
			/// Adds a component to the given object
			/// </summary>
			/// <param name="_irisObjectID"></param>
			IE_FORCE_INLINE IComponent* AddComponent(IrisObject* _irisObject) override;

			/// <summary>
			/// Removes the component of the given object 
			/// </summary>
			/// <param name="_irisObjectID"></param>
			IE_FORCE_INLINE void RemoveComponent(IrisObject* _irisObject) override;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_irisObjectID"></param>
			/// <returns>The wanted component of the object specify, nullptr if the object doesn't have the component</returns>
			IE_FORCE_INLINE IComponent* GetComponent(IrisObject* _irisObject) override;

			/// <returns>A list of all the component of the table</returns>
			IE_FORCE_INLINE std::vector<T>& GetAllComponents();

		private:

			/// <summary>
			/// Table of components
			/// </summary>
			std::vector<T> m_Table;

			/// <summary>
			/// index of the first component destroy in m_Table (-1 if m_Table full of active component)
			/// </summary>
			int32_t m_IndexFirstDestroy = -1;

			/// <summary>
			/// <para>uint64_t: ownerObjectIndex</para>
			/// <para>size_t: componentIndex</para>
			/// </summary>
			std::unordered_map<uint64_t, size_t> m_ComponentIndex;
		};
	}
}

#include "SECS/Components/ComponentTable.inl"