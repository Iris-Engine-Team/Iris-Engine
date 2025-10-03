#include "SECS/Components/ComponentTable.h"
#include "SECS/Components/IComponent.h"
#include "SECS/IrisObject.h"
#include <string>
#include "Core/Tools/Log.h"

namespace Iris
{
	namespace Engine
	{
		template <class T>
		ComponentTable<T>* Scene::AddTable()
		{
			IComponentTable* tablePtr = new ComponentTable<T>();
			m_AllComponents[T::GetID()] = tablePtr;

			return GetComponentTable<T>();
		}

		template<class T>
		bool Scene::ContainTable() const
		{
			auto table = m_AllComponents.find(T::GetID());
			if (table == m_AllComponents.end())
				return false;

			return true;
		}

		template <class T>
		ComponentTable<T>* Scene::GetComponentTable()
		{
			return reinterpret_cast<ComponentTable<T>*>(m_AllComponents[T::GetID()]);
		}

		template <class T>
		T* Scene::GetComponent(IrisObject* _irisObjectID)
		{
			auto table = m_AllComponents.find(T::GetID());
			if (table == m_AllComponents.end())
			{
				return nullptr;
			}
			T* component = reinterpret_cast<T*>(GetComponentTable<T>()->GetComponent(_irisObjectID));

			if (!component)
			{
				size_t size = m_ComponentsToAdd.size();
				for (size_t i = 0; i < size; ++i)
				{
					if (m_ComponentsToAdd[i]->GetIrisObject()->GetID() == _irisObjectID->GetID() && m_ComponentsToAdd[i]->GetTypeID() == T::GetID())
						return reinterpret_cast<T*>(m_ComponentsToAdd[i]);
				}
			}

			return component;
		}

		template <class T>
		T* Scene::AddComponent(IrisObject* _irisObjectID)
		{
			if (std::is_base_of<IComponent, T>::value)
			{
				auto table = m_AllComponents.find(T::GetID());
				if (table == m_AllComponents.end())
					AddTable<T>();

				T* component = _irisObjectID->GetComponent<T>();
				if (component)
					return component;
				
				component = new T();
				component->SetOwner(_irisObjectID);
				m_ComponentsToAdd.emplace_back(component);
				return component;
			}

			DEBUG_LOG("Not a component");
			return nullptr;
		}
		
		template<class T>
		void Scene::RemoveComponent(IrisObject* _irisObjectID)
		{
			m_ComponentsToDestroy.push_back(new ComponentHandle<T>(_irisObjectID));
		}
		
	}
}