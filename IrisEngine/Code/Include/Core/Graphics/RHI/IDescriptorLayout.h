#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"
#include "Core/Graphics/RHI/CommonTypes.h"


namespace Iris
{
	namespace Engine
	{
		class VulkanDescriptorLayout;
		class IDevice;

		class IDescriptorLayout
		{
		public:
			virtual ~IDescriptorLayout() = default;

			virtual VulkanDescriptorLayout* CastToVulkan() = 0;

			virtual IDescriptorLayout* CreateDescriptorLayout(IDevice* _Device) = 0;
			virtual RHIResult DestroyDescriptorLayout(IDevice* _Device) = 0;
		};

	}
}