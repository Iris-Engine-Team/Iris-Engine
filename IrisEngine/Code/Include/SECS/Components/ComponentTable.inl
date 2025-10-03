#include "SECS/Components/IComponent.h"
#include "Resources/MetaDataManager.h"
#include "Resources/Metadata.h"
#include "SECS/IrisObject.h"

namespace Iris
{
	namespace Engine
	{
		template<class T>
		void ComponentTable<T>::Destroy()
		{
			size_t size = ComponentCount();

			for (size_t i = 0; i < size; ++i)
			{
				IComponent* c = &m_Table[i];
				c->Destroy();
			}
			m_Table.clear();
			m_ComponentIndex.clear();
		}

		
		template<class T>
		void ComponentTable<T>::Serialize(MetaDataManager* _metadataManager, IMetadata* _sceneMetadata)
		{
			std::string compId = std::to_string(static_cast<uint16_t>(T::GetID()));

			//To debug
			std::string compName = T::GetComponentName();

			std::vector<IMetadata*> componentsData;

			size_t size = ComponentCount();
			for (size_t i = 0; i < size; ++i)
			{
				IMetadata* componentData = _metadataManager->CreateMetadata();
				m_Table[i].Serialize(componentData);

				componentsData.push_back(componentData);

				std::vector<std::string> path = {"Scene Data", "Components", compId, compName, std::to_string(m_Table[i].GetIrisObject()->GetID())};
				_sceneMetadata->WriteMeta(path, componentData);
			}

			for (size_t j = 0; j < size; ++j)
				delete componentsData[j];
		}

		template<class T>
		void ComponentTable<T>::Deserialization(IMetadata* _metadata, InitEditorContext& _initializer)
		{
			size_t size = ComponentCount();
			for (size_t i = 0; i < size; ++i)
				m_Table[i].Deserialization(_metadata, _initializer);
		}

		template<class T>
		size_t ComponentTable<T>::ComponentCount()
		{
			int32_t gap = GetGapFirstDestroy();
			if (gap < 0)
				return m_Table.size();


			return static_cast<size_t>(m_Table.size() - gap);
		}

		template <class T>
		int32_t ComponentTable<T>::ContainsEntity(IrisObject* _irisObjectID)
		{
			if (auto search = m_ComponentIndex.find(_irisObjectID->GetID()); search != m_ComponentIndex.end())
				return static_cast<int32_t>(search->second);
			else
				return -1;
		}

		template <class T>
		int32_t ComponentTable<T>::GetGapFirstDestroy()
		{
			if (m_IndexFirstDestroy == -1)
				return -1;

			int32_t gap = static_cast<int32_t>(m_Table.size()) - m_IndexFirstDestroy;

			if (gap <= 0)
			{
				m_IndexFirstDestroy = -1;
				return -1;
			}

			return gap;
		}

		template <class T>
		IComponent* ComponentTable<T>::AddComponent(IrisObject* _irisObjectID)
		{
			size_t compIndex = ContainsEntity(_irisObjectID);

			if (compIndex == -1)
			{
				T* component = nullptr;
				if (m_IndexFirstDestroy == -1)
				{
					m_Table.push_back(T());
					component = &m_Table.back();
					m_ComponentIndex[_irisObjectID->GetID()] = m_Table.size() - 1;
				}
				else
				{
					m_Table[m_IndexFirstDestroy] = T();
					component = &m_Table[m_IndexFirstDestroy];
					m_ComponentIndex[_irisObjectID->GetID()] = m_IndexFirstDestroy;

					++m_IndexFirstDestroy;
					if (m_IndexFirstDestroy == static_cast<int32_t>(m_Table.size()))
						m_IndexFirstDestroy = -1;
				}

				component->SetOwner(_irisObjectID);
				_irisObjectID->AddHandle(new ComponentHandle<T>(component));

				return component;
			}
			else
			{
				//obj already have one
				return nullptr;
			}
		}

		template <class T>
		void ComponentTable<T>::RemoveComponent(IrisObject* _irisObjectID)
		{
			size_t compIndex = ContainsEntity(_irisObjectID);
			if (compIndex == -1)
			{
				//not found
				return;
			}

			T* comp = &m_Table[compIndex];
			comp->Destroy();
			const T componentToDelete = m_Table[compIndex];

			if (m_IndexFirstDestroy == -1)
			{
				//if no comp destroy in table, swap with last one
				const auto& endComponent = m_Table[m_Table.size() - 1];
				m_IndexFirstDestroy = static_cast<int32_t>(m_Table.size()) - 1;
				m_Table[compIndex] = endComponent;

			}
			else
			{
				//swap with last avalable one
				const auto& endComponent = m_Table[m_IndexFirstDestroy - 1];
				--m_IndexFirstDestroy;
				m_Table[compIndex] = endComponent;
			}

			IrisObject* objCompSwap = m_Table[compIndex].GetIrisObject();
			//if deleted component is the end component -> obj ref -> nullptr
			if (objCompSwap)
				m_ComponentIndex[objCompSwap->GetID()] = compIndex;

			m_Table[m_IndexFirstDestroy] = componentToDelete;
			m_ComponentIndex.erase(_irisObjectID->GetID());
		}

		template <class T>
		IComponent* ComponentTable<T>::GetComponent(IrisObject* _irisObjectID)
		{
			size_t componentIndex = ContainsEntity(_irisObjectID);

			if (componentIndex == -1)
				return nullptr;

			return &m_Table[componentIndex];
		}

		template <class T>
		std::vector<T>& ComponentTable<T>::GetAllComponents()
		{
			return m_Table;
		}
	}
}