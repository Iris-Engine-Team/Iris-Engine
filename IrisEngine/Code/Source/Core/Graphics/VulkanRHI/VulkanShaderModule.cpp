#include "Core/Graphics/VulkanRHI/VulkanShaderModule.h"

#include "Core/Graphics/VulkanRHI/VulkanDevice.h"

namespace Iris
{
	namespace Engine
	{
		RHIResult VulkanShaderModule::CreateShaderModule(IDevice* _device, std::vector<uint32_t>& _shaderCode)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			VkShaderModuleCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = _shaderCode.size() * sizeof(uint32_t);
			createInfo.pCode = _shaderCode.data();

			VkResult result = vkCreateShaderModule(vkDevice->GetLogicalDevice(), &createInfo, nullptr, &m_ShaderModule);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to create shader module, error code: %i", result);
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanShaderModule::DestroyShaderModule(IDevice* _device)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			vkDestroyShaderModule(vkDevice->GetLogicalDevice(), m_ShaderModule, nullptr);

			return RHIResult::IE_RHI_SUCCESS;
		}
	}
}