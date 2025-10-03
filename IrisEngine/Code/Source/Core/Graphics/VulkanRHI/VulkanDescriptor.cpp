#include "Core/Graphics/VulkanRHI/VulkanDescriptor.h"
#include "Core/Graphics/VulkanRHI/VulkanDevice.h"
#include "Core/Graphics/VulkanRHI/VulkanSampler.h"
#include "Core/Graphics/VulkanRHI/VulkanImage.h"
#include "Core/Graphics/VulkanRHI/VulkanFramebuffer.h"
#include "SECS/LightData/LightData.h"
#include "Resources/ResourceManager.h"
#include "Core/Graphics/Renderer/Shadows/ShadowMap.h"

#include "Resources/Texture.h"

#include <array>
#include <vector>

namespace Iris
{
	namespace Engine
	{
		void VulkanDescriptor::UpdateTextureDescriptor(IDevice* _device, const Texture* _texture, unsigned int _binding)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			VulkanSampler* vkSampler = _texture->GetSampler()->CastToVulkan();
			VulkanImage* vkImage = _texture->GetImage()->CastToVulkan();

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = vkImage->GetImageView();
			imageInfo.sampler = vkSampler->GetSampler();

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_Descriptor;
			descriptorWrite.dstBinding = _binding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(vkDevice->GetLogicalDevice(), 1, &descriptorWrite, 0, nullptr);
		}

		void VulkanDescriptor::UpdateShadowDescriptors(IDevice* _device, const std::vector<ShadowMap*>& _dirShadowMaps, const std::vector<ShadowMap*>& _spotShadowMaps, const std::vector<ShadowMap*>& _pointShadowMaps, unsigned int _currentDescriptor)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			std::array<VkDescriptorImageInfo, IE_MAX_DIRECTIONAL_LIGHTS_SHADOWS> dirLightInfo{};
			std::array<VkDescriptorImageInfo, IE_MAX_SPOT_LIGHTS_SHADOWS> spotLightInfo{};
			std::array<VkDescriptorImageInfo, IE_MAX_POINT_LIGHTS_SHADOWS> pointLightInfo{};

			std::array<VkWriteDescriptorSet, 3> shadowDescriptorWrites{};

			Texture* defaultTexture = ResourceManager::GetInstance()->Get<Texture>("DefaultTexture.jpg");
			VkSampler defaultSampler = defaultTexture->GetSampler()->CastToVulkan()->GetSampler();
			VkImageView defaultImageView = defaultTexture->GetImage()->CastToVulkan()->GetImageView();

			Texture* defaultCubemapTexture = ResourceManager::GetInstance()->Get<Texture>("DefaultCubeMapTexture.jpg");
			VkSampler defaultCubemapSampler = defaultCubemapTexture->GetSampler()->CastToVulkan()->GetSampler();
			VkImageView defaultCubemapImageView = defaultCubemapTexture->GetImage()->CastToVulkan()->GetImageView();

			for (size_t i = 0; i < IE_MAX_DIRECTIONAL_LIGHTS_SHADOWS; ++i)
			{
				dirLightInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				if (i < _dirShadowMaps.size() && _dirShadowMaps[i] != nullptr)
				{
					VulkanImage attachment = _dirShadowMaps[i]->GetFramebuffers()[_currentDescriptor]->CastToVulkan()->GetAttachments()[0];
					dirLightInfo[i].imageView = attachment.GetImageView();
					dirLightInfo[i].sampler = _dirShadowMaps[i]->GetSampler()->CastToVulkan()->GetSampler();
				}
				else
				{
					dirLightInfo[i].imageView = defaultImageView;
					dirLightInfo[i].sampler = defaultSampler;
				}
			}

			shadowDescriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			shadowDescriptorWrites[0].dstSet = m_Descriptor;
			shadowDescriptorWrites[0].dstBinding = 0;
			shadowDescriptorWrites[0].dstArrayElement = 0;
			shadowDescriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			shadowDescriptorWrites[0].descriptorCount = static_cast<uint32_t>(dirLightInfo.size());
			shadowDescriptorWrites[0].pImageInfo = dirLightInfo.data();

			for (size_t i = 0; i < IE_MAX_SPOT_LIGHTS_SHADOWS; ++i)
			{
				spotLightInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				if (i < _spotShadowMaps.size() && _spotShadowMaps[i] != nullptr)
				{
					VulkanImage attachment = _spotShadowMaps[i]->GetFramebuffers()[_currentDescriptor]->CastToVulkan()->GetAttachments()[0];
					spotLightInfo[i].imageView = attachment.GetImageView();
					spotLightInfo[i].sampler = _spotShadowMaps[i]->GetSampler()->CastToVulkan()->GetSampler();
				}
				else
				{
					spotLightInfo[i].imageView = defaultImageView;
					spotLightInfo[i].sampler = defaultSampler;
				}
			}

			shadowDescriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			shadowDescriptorWrites[1].dstSet = m_Descriptor;
			shadowDescriptorWrites[1].dstBinding = 1;
			shadowDescriptorWrites[1].dstArrayElement = 0;
			shadowDescriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			shadowDescriptorWrites[1].descriptorCount = static_cast<uint32_t>(spotLightInfo.size());
			shadowDescriptorWrites[1].pImageInfo = spotLightInfo.data();

			for (size_t i = 0; i < IE_MAX_POINT_LIGHTS_SHADOWS; ++i)
			{
				pointLightInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				if (i < _pointShadowMaps.size() && _pointShadowMaps[i] != nullptr)
				{
					VulkanImage attachment = _pointShadowMaps[i]->GetFramebuffers()[_currentDescriptor]->CastToVulkan()->GetAttachments()[0];
					pointLightInfo[i].imageView = attachment.GetImageView();
					pointLightInfo[i].sampler = _pointShadowMaps[i]->GetSampler()->CastToVulkan()->GetSampler();
				}
				else
				{
					pointLightInfo[i].imageView = defaultCubemapImageView;
					pointLightInfo[i].sampler = defaultCubemapSampler;
				}
			}

			shadowDescriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			shadowDescriptorWrites[2].dstSet = m_Descriptor;
			shadowDescriptorWrites[2].dstBinding = 2;
			shadowDescriptorWrites[2].dstArrayElement = 0;
			shadowDescriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			shadowDescriptorWrites[2].descriptorCount = static_cast<uint32_t>(pointLightInfo.size());
			shadowDescriptorWrites[2].pImageInfo = pointLightInfo.data();

			vkUpdateDescriptorSets(vkDevice->GetLogicalDevice(), 3, shadowDescriptorWrites.data(), 0, nullptr);
		}
	}
}