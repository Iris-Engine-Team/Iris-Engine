#include "Core/Graphics/VulkanRHI/VulkanDescriptorLayout.h"

namespace Iris
{
	namespace Engine
	{
		IDescriptorLayout* VulkanDescriptorLayout::CreateDescriptorLayout(IDevice* _Device)
		{
			_Device;
			return nullptr;
		}

		RHIResult VulkanDescriptorLayout::DestroyDescriptorLayout(IDevice* _Device)
		{
			_Device;
			return RHIResult();
		}
	}
}