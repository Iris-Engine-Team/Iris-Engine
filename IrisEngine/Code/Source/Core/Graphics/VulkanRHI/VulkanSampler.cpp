#include "Core/Graphics/VulkanRHI/VulkanSampler.h"

#include "Core/Graphics/VulkanRHI/VulkanDevice.h"
#include "Core/Graphics/VulkanRHI/VulkanBuffer.h"
#include "Core/Graphics/VulkanRHI/VulkanCommandAllocator.h"
#include "Core/Graphics/VulkanRHI/VulkanFramebuffer.h"
#include "Core/Graphics/VulkanRHI/VulkanRenderInterface.h"

namespace Iris
{
	namespace Engine
	{
		RHIResult VulkanSampler::CreateSampler(IDevice* _device, unsigned int _mipLevels, const RHISampler& _samplerInfo)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();
			return CreateTextureSampler(vkDevice, _samplerInfo, _mipLevels);
		}

		RHIResult VulkanSampler::CreateSampler(IDevice* _device, IFramebuffer* _framebuffer, const RHISampler& _samplerInfo)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();
			VulkanFramebuffer* vkFb = _framebuffer->CastToVulkan();
			(void)vkFb;

			return CreateTextureSampler(vkDevice, _samplerInfo, 1);
		}

		RHIResult VulkanSampler::DestroySampler(IDevice* _device)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			vkDestroySampler(vkDevice->GetLogicalDevice(), m_TextureSampler, nullptr);

			return RHIResult();
		}

		RHIResult VulkanSampler::CreateTextureSampler(VulkanDevice* _device, const RHISampler& _samplerInfo, unsigned int _mipLevels)
		{
			VkPhysicalDeviceProperties properties{};
			vkGetPhysicalDeviceProperties(_device->GetPhysicalDevice(), &properties);

			VkSamplerCreateInfo samplerInfo{};
			samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = VulkanRenderInterface::ConvertSamplerFilter(_samplerInfo.MagFilter);
			samplerInfo.minFilter = VulkanRenderInterface::ConvertSamplerFilter(_samplerInfo.MinFilter);
			samplerInfo.addressModeU = VulkanRenderInterface::ConvertSamplerAdress(_samplerInfo.ModeU);
			samplerInfo.addressModeV = VulkanRenderInterface::ConvertSamplerAdress(_samplerInfo.ModeV);
			samplerInfo.addressModeW = VulkanRenderInterface::ConvertSamplerAdress(_samplerInfo.ModeW);
			samplerInfo.anisotropyEnable = VK_TRUE;
			samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
			samplerInfo.borderColor = VulkanRenderInterface::ConvertSamplerBorderColor(_samplerInfo.BorderColor);
			samplerInfo.unnormalizedCoordinates = VK_FALSE;
			samplerInfo.compareEnable = VK_FALSE;
			samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
			samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerInfo.mipLodBias = 0.0f;
			samplerInfo.minLod = 0.f;
			samplerInfo.maxLod = static_cast<float>(_mipLevels - 1);

			VkResult result = vkCreateSampler(_device->GetLogicalDevice(), &samplerInfo, nullptr, &m_TextureSampler);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to create texture sampler! Error code: %i", result);
				return RHIResult();
			}

			return RHIResult();
		}
	}
}