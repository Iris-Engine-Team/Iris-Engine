#pragma once

#include "Core/Graphics/RHI/IDescriptorLayout.h"

#include <vulkan/vulkan.h>

namespace Iris
{
	namespace Engine
	{
		class VulkanDescriptorLayout : public IDescriptorLayout
		{
		public:
			inline IRIS_ENGINE_API VulkanDescriptorLayout* CastToVulkan() override final { return this; }

			inline IRIS_ENGINE_API VkDescriptorSetLayout GetDescriptorLayout() const { return m_DescriptorLayout; }
			inline IRIS_ENGINE_API void SetDescriptorLayout(VkDescriptorSetLayout _descriptorLayout) { m_DescriptorLayout = _descriptorLayout; }

			IRIS_ENGINE_API IDescriptorLayout* CreateDescriptorLayout(IDevice* _Device) override final;
			IRIS_ENGINE_API RHIResult DestroyDescriptorLayout(IDevice* _Device) override final;

		private:
			VkDescriptorSetLayout m_DescriptorLayout = VK_NULL_HANDLE;
		};
	}
}