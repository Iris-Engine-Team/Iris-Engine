#include "SECS/Components/Transform.h"
#include "SECS/Components/RigidBody.h"
#include "SECS/Scene.h"
#include "Core/Tools/Log.h"

#include "SECS/Components/ComponentTable.h"

namespace Iris
{
	namespace Engine
	{
		template <class T>
		T* IrisObject::GetComponent()
		{
			return m_SceneRef->GetComponent<T>(this);
		}

		template <class T>
		T* IrisObject::AddComponent()
		{
			return m_SceneRef->AddComponent<T>(this);
		}

		template <class T>
		void IrisObject::AddHandle(ComponentHandle<T>* _newHandle)
		{
			int index = static_cast<int>(_newHandle->GetType());
			if (!m_Components[index])
				m_Components[index] = _newHandle;
		}

		template <class T, class F>
		T* IrisObject::GetHandle()
		{
			return dynamic_cast<T*>(m_Components[static_cast<int>(F::GetID())]);
		}

		template<class T>
		void IrisObject::RemoveComponent()
		{
			ComponentID compId = T::GetID();

			if(compId == ComponentID::IE_TRANSFORM)
				DestroyChildObject();

			size_t compIndex = static_cast<size_t>(compId);

			if (m_Components[compIndex])
			{
				delete m_Components[compIndex];
				m_Components[compIndex] = nullptr;
				m_SceneRef->RemoveComponent<T>(this);
			}				
		}
	}
}