#include "SECS/IrisObject.h"

namespace Iris
{
	namespace Engine
	{
		template<class T>
		void ComponentHandle<T>::SetEnableComponent(bool _enable)
		{
			UseComponent()->SetEnable(_enable);
		}

		template<class T>
		bool ComponentHandle<T>::IsEnableComponent()
		{
			return UseComponent()->IsEnable();
		}

		template<typename T>
		ComponentHandle<T>::ComponentHandle(T* _component)
		{
			p_Obj = _component->GetIrisObject();
			p_Type = T::GetID();
			p_ComponentName = T::GetName();
		}

		template<class T>
		ComponentHandle<T>::ComponentHandle(IrisObject* _irisObject)
		{
			p_Obj = _irisObject;
			p_Type = T::GetID();
			p_ComponentName = T::GetName();
		}

		template<class T>
		void ComponentHandle<T>::RemoveComponent()
		{
			p_Obj->RemoveComponent<T>();
		}

		template<class T>
		T* ComponentHandle<T>::UseComponent()
		{
			if(p_Obj)
				return p_Obj->GetComponent<T>();
			return nullptr;
		}
	}
}