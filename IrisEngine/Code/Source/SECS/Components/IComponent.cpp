#include "SECS/Components/IComponent.h"
#include "SECS/IrisObject.h"

namespace Iris
{
	namespace Engine
	{
		void IComponent::SetOwner(IrisObject* _owner)
		{
			p_IrisObject = _owner;
		}
	}
}