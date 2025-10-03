#include "Core/Graphics/VulkanRHI/VulkanDescriptorAllocator.h"

#include "Core/Graphics/VulkanRHI/VulkanDevice.h"
#include "Core/Graphics/VulkanRHI/VulkanSwapChain.h"
#include "Core/Graphics/VulkanRHI/VulkanDescriptor.h"
#include "Core/Graphics/VulkanRHI/VulkanBuffer.h"
#include "Core/Graphics/VulkanRHI/VulkanFrameBuffer.h"
#include "Core/Graphics/VulkanRHI/VulkanDescriptorLayout.h"
#include "Core/Graphics/VulkanRHI/VulkanSampler.h"
#include "Resources/Texture.h"
#include "Resources/Material.h"
#include "SECS/LightData/LightData.h"
#include "Core/Graphics/Renderer/Shadows/ShadowMap.h"
#include "Core/Graphics/VulkanRHI/VulkanImage.h"
#include "Resources/ResourceManager.h"

#include <array>
#include "Resources/ResourceManager.h"
#include "Core/Tools/FilePath.h"

namespace Iris
{
	namespace Engine
	{
		RHIResult VulkanDescriptorAllocator::Initialize(IDevice* _device)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			std::array<VkDescriptorPoolSize, 3> poolSizes{};
			poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSizes[0].descriptorCount = 2000;
			poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			poolSizes[1].descriptorCount = 2000;
			poolSizes[2].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			poolSizes[2].descriptorCount = 2000;

			VkDescriptorPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
			poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			poolInfo.pPoolSizes = poolSizes.data();
			poolInfo.maxSets = 6000;

			VkResult result = vkCreateDescriptorPool(vkDevice->GetLogicalDevice(), &poolInfo, nullptr, &m_DescriptorAllocator);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to create descriptor allocator! Error code: %i", result);
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanDescriptorAllocator::Terminate(IDevice* _device)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			vkDestroyDescriptorPool(vkDevice->GetLogicalDevice(), m_DescriptorAllocator, nullptr);

			return RHIResult::IE_RHI_SUCCESS;
		}

		IDescriptor* VulkanDescriptorAllocator::AllocateDescriptorSet(IDevice* _device, IDescriptorLayout* _descriptorLayout, IBuffer* _buffer, size_t _size, unsigned int _binding)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();
			VulkanBuffer* vkBuffer = _buffer->CastToVulkan();

			IDescriptor* newDescriptor = new VulkanDescriptor();

			VulkanDescriptor* vkDescriptor = newDescriptor->CastToVulkan();
			VkDescriptorSet vkDescriptorSet = vkDescriptor->GetDescriptor();
			VulkanDescriptorLayout* vkDescriptorLayout = _descriptorLayout->CastToVulkan();
			VkDescriptorSetLayout vkDLRaw = vkDescriptorLayout->GetDescriptorLayout();

			// Allocation
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = m_DescriptorAllocator;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &vkDLRaw;

			VkResult result = vkAllocateDescriptorSets(vkDevice->GetLogicalDevice(), &allocInfo, &vkDescriptorSet);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to allocate descriptor sets! Error code %i", result);
				return nullptr;
			}

			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = vkBuffer->GetBuffer();
			bufferInfo.offset = 0;
			bufferInfo.range = static_cast<uint32_t>(_size);

			// Population
			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = vkDescriptorSet;
			descriptorWrite.dstBinding = static_cast<uint32_t>(_binding);
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(vkDevice->GetLogicalDevice(), 1, &descriptorWrite, 0, nullptr);

			vkDescriptor->SetDescriptor(vkDescriptorSet);

			return newDescriptor;
		}

		IDescriptor* VulkanDescriptorAllocator::AllocateDescriptorSet(IDevice* _device, IDescriptorLayout* _descriptorLayout, Texture* _texture)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			IDescriptor* newDescriptor = new VulkanDescriptor();

			VulkanSampler* vkTexture = _texture->GetSampler()->CastToVulkan();
			VulkanImage* vkImage = _texture->GetImage()->CastToVulkan();

			VulkanDescriptor* vkDescriptor = newDescriptor->CastToVulkan();
			VkDescriptorSet vkDescriptorSet = vkDescriptor->GetDescriptor();
			VulkanDescriptorLayout* vkDescriptorLayout = _descriptorLayout->CastToVulkan();
			VkDescriptorSetLayout vkDLRaw = vkDescriptorLayout->GetDescriptorLayout();

			// Allocation
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = m_DescriptorAllocator;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &vkDLRaw;

			VkResult result = vkAllocateDescriptorSets(vkDevice->GetLogicalDevice(), &allocInfo, &vkDescriptorSet);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to allocate descriptor sets! Error code: %i", result);
				return nullptr;
			}

			VkDescriptorImageInfo imagesInfo{};
			imagesInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imagesInfo.imageView = vkImage->GetImageView();
			imagesInfo.sampler = vkTexture->GetSampler();

			// Population
			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = vkDescriptorSet;
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pImageInfo = &imagesInfo;

			vkUpdateDescriptorSets(vkDevice->GetLogicalDevice(), 1, &descriptorWrite, 0, nullptr);

			vkDescriptor->SetDescriptor(vkDescriptorSet);

			return newDescriptor;
		}

		IDescriptor* VulkanDescriptorAllocator::AllocateDescriptorSet(IDevice* _device, IDescriptorLayout* _descriptorLayout, Material* _material)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			IDescriptor* newDescriptor = new VulkanDescriptor();

			LitPBRInputs* inputs = dynamic_cast<LitPBRInputs*>(_material->GetInputVariables());

			const Texture* defaultText = ResourceManager::GetInstance()->Create<Texture>(FIND_PATH("DefaultTexture.jpg"));

			const Texture* albedo = inputs->GetAlbedoTexture();

			if (!albedo)
				albedo = defaultText;

			VulkanSampler* vkDiffuse = albedo->GetSampler()->CastToVulkan();
			VulkanImage* vkDiffuseImage = albedo->GetImage()->CastToVulkan();

			const Texture* rough = inputs->GetRoughnessTexture();
			if (!rough)
				rough = defaultText;

			VulkanSampler* vkRoughness = rough->GetSampler()->CastToVulkan();
			VulkanImage* vkRoughnessImage = rough->GetImage()->CastToVulkan();

			const Texture* met = inputs->GetMetallicTexture();
			if (!met)
				met = defaultText;

			VulkanSampler* vkMetallic = met->GetSampler()->CastToVulkan();
			VulkanImage* vkMetallicImage = met->GetImage()->CastToVulkan();

			const Texture* nor = inputs->GetNormalTexture();
			if (!nor)
				nor = ResourceManager::GetInstance()->Create<Texture>(FIND_PATH("DefaultNormal.jpg"));

			VulkanSampler* vkNormal = nor->GetSampler()->CastToVulkan();
			VulkanImage* vkNormalImage = nor->GetImage()->CastToVulkan();

			VulkanDescriptor* vkDescriptor = newDescriptor->CastToVulkan();
			VkDescriptorSet vkDescriptorSet = vkDescriptor->GetDescriptor();
			VulkanDescriptorLayout* vkDescriptorLayout = _descriptorLayout->CastToVulkan();
			VkDescriptorSetLayout vkDLRaw = vkDescriptorLayout->GetDescriptorLayout();

			// Allocation
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = m_DescriptorAllocator;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &vkDLRaw;

			VkResult result = vkAllocateDescriptorSets(vkDevice->GetLogicalDevice(), &allocInfo, &vkDescriptorSet);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to allocate descriptor sets! Error code: %i", result);
				return nullptr;
			}

			std::array<VkDescriptorImageInfo, 4> imagesInfo{};
			imagesInfo[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imagesInfo[0].imageView = vkDiffuseImage->GetImageView();
			imagesInfo[0].sampler = vkDiffuse->GetSampler();

			imagesInfo[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imagesInfo[1].imageView = vkRoughnessImage->GetImageView();
			imagesInfo[1].sampler = vkRoughness->GetSampler();

			imagesInfo[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imagesInfo[2].imageView = vkMetallicImage->GetImageView();
			imagesInfo[2].sampler = vkMetallic->GetSampler();

			imagesInfo[3].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imagesInfo[3].imageView = vkNormalImage->GetImageView();
			imagesInfo[3].sampler = vkNormal->GetSampler();

			// Population
			std::array<VkWriteDescriptorSet, 4> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = vkDescriptorSet;
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pImageInfo = &imagesInfo[0];

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = vkDescriptorSet;
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imagesInfo[1];

			descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[2].dstSet = vkDescriptorSet;
			descriptorWrites[2].dstBinding = 2;
			descriptorWrites[2].dstArrayElement = 0;
			descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[2].descriptorCount = 1;
			descriptorWrites[2].pImageInfo = &imagesInfo[2];

			descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[3].dstSet = vkDescriptorSet;
			descriptorWrites[3].dstBinding = 3;
			descriptorWrites[3].dstArrayElement = 0;
			descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[3].descriptorCount = 1;
			descriptorWrites[3].pImageInfo = &imagesInfo[3];

			vkUpdateDescriptorSets(vkDevice->GetLogicalDevice(), 4, descriptorWrites.data(), 0, nullptr);

			vkDescriptor->SetDescriptor(vkDescriptorSet);

			return newDescriptor;
		}

		IDescriptor* VulkanDescriptorAllocator::AllocateDescriptorSet(IDevice* _device, IDescriptorLayout* _descriptorLayout, IFramebuffer* _frameBuffer, unsigned int _firstAttachment, unsigned int _lastAttachment)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			IDescriptor* newDescriptor = new VulkanDescriptor();

			VulkanDescriptor* vkDescriptor = newDescriptor->CastToVulkan();
			VkDescriptorSet vkDescriptorSet = vkDescriptor->GetDescriptor();
			VulkanDescriptorLayout* vkDescriptorLayout = _descriptorLayout->CastToVulkan();
			VkDescriptorSetLayout vkDLRaw = vkDescriptorLayout->GetDescriptorLayout();

			uint32_t attachmentCount = _lastAttachment - _firstAttachment + 1;

			std::vector<VkDescriptorImageInfo> descriptors(attachmentCount);
			std::vector<VkWriteDescriptorSet> writeDescriptorSets(attachmentCount);

			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = m_DescriptorAllocator;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &vkDLRaw;

			VkResult result = vkAllocateDescriptorSets(vkDevice->GetLogicalDevice(), &allocInfo, &vkDescriptorSet);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to allocate descriptor sets! Error code: %i", result);
				return nullptr;
			}

			for (size_t i = 0; i < attachmentCount; ++i)
			{
				descriptors[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				descriptors[i].imageView = _frameBuffer->CastToVulkan()->GetAttachments()[i + _firstAttachment].GetImageView();
				descriptors[i].sampler = VK_NULL_HANDLE;

				writeDescriptorSets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writeDescriptorSets[i].dstSet = vkDescriptorSet;
				writeDescriptorSets[i].dstBinding = static_cast<uint32_t>(i);
				writeDescriptorSets[i].dstArrayElement = 0;
				writeDescriptorSets[i].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
				writeDescriptorSets[i].descriptorCount = 1;
				writeDescriptorSets[i].pImageInfo = &descriptors[i];
			}

			vkUpdateDescriptorSets(vkDevice->GetLogicalDevice(), attachmentCount, writeDescriptorSets.data(), 0, nullptr);

			vkDescriptor->SetDescriptor(vkDescriptorSet);

			return newDescriptor;
		}

		IDescriptor* VulkanDescriptorAllocator::AllocateDescriptorSet(IDevice* _device, IDescriptorLayout* _descriptorLayout, ISampler* _sampler, IFramebuffer* _frameBuffer, unsigned int _attachment)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			IDescriptor* newDescriptor = new VulkanDescriptor();

			VulkanDescriptor* vkDescriptor = newDescriptor->CastToVulkan();
			VkDescriptorSet vkDescriptorSet = vkDescriptor->GetDescriptor();
			VulkanDescriptorLayout* vkDescriptorLayout = _descriptorLayout->CastToVulkan();
			VkDescriptorSetLayout vkDLRaw = vkDescriptorLayout->GetDescriptorLayout();
			VkSampler vkSamp = _sampler->CastToVulkan()->GetSampler();

			VkDescriptorImageInfo descriptor{};
			VkWriteDescriptorSet writeDescriptorSet{};

			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = m_DescriptorAllocator;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &vkDLRaw;

			VkResult result = vkAllocateDescriptorSets(vkDevice->GetLogicalDevice(), &allocInfo, &vkDescriptorSet);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to allocate descriptor sets! Error code: %i", result);
				return nullptr;
			}

			descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			descriptor.imageView = _frameBuffer->CastToVulkan()->GetAttachments()[_attachment].GetImageView();
			descriptor.sampler = vkSamp;

			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstSet = vkDescriptorSet;
			writeDescriptorSet.dstBinding = 0;
			writeDescriptorSet.dstArrayElement = 0;
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writeDescriptorSet.descriptorCount = 1;
			writeDescriptorSet.pImageInfo = &descriptor;

			vkUpdateDescriptorSets(vkDevice->GetLogicalDevice(), 1, &writeDescriptorSet, 0, nullptr);

			vkDescriptor->SetDescriptor(vkDescriptorSet);

			return newDescriptor;
		}

		IDescriptor* VulkanDescriptorAllocator::AllocateDescriptorSet(IDevice* _device, IDescriptorLayout* _descriptorLayout, Texture* _irradianceTexture, Texture* _prefilteredTexture, Texture* _lookUpTexture)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();


			VulkanSampler* vkIrradianceSamp = _irradianceTexture->GetSampler()->CastToVulkan();
			VulkanImage* vkIrradianceImg = _irradianceTexture->GetImage()->CastToVulkan();

			VulkanSampler* vkPrefilteredSamp = _prefilteredTexture->GetSampler()->CastToVulkan();
			VulkanImage* vkPrefilteredImg = _prefilteredTexture->GetImage()->CastToVulkan();

			VulkanSampler* vkLookUpSamp = _lookUpTexture->GetSampler()->CastToVulkan();
			VulkanImage* vkLookUpImg = _lookUpTexture->GetImage()->CastToVulkan();

			IDescriptor* newDescriptor = new VulkanDescriptor();
			VulkanDescriptor* vkDescriptor = newDescriptor->CastToVulkan();
			VkDescriptorSet vkDescriptorSet = vkDescriptor->GetDescriptor();
			VulkanDescriptorLayout* vkDescriptorLayout = _descriptorLayout->CastToVulkan();
			VkDescriptorSetLayout vkDLRaw = vkDescriptorLayout->GetDescriptorLayout();

			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = m_DescriptorAllocator;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &vkDLRaw;

			VkResult result = vkAllocateDescriptorSets(vkDevice->GetLogicalDevice(), &allocInfo, &vkDescriptorSet);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to allocate descriptor sets! Error code: %i", result);
				return nullptr;
			}

			std::array<VkDescriptorImageInfo, 3> imagesInfo{};
			imagesInfo[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imagesInfo[0].imageView = vkIrradianceImg->GetImageView();
			imagesInfo[0].sampler = vkIrradianceSamp->GetSampler();

			imagesInfo[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imagesInfo[1].imageView = vkPrefilteredImg->GetImageView();
			imagesInfo[1].sampler = vkPrefilteredSamp->GetSampler();

			imagesInfo[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imagesInfo[2].imageView = vkLookUpImg->GetImageView();
			imagesInfo[2].sampler = vkLookUpSamp->GetSampler();

			std::array<VkWriteDescriptorSet, 3> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = vkDescriptorSet;
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pImageInfo = &imagesInfo[0];

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = vkDescriptorSet;
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imagesInfo[1];

			descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[2].dstSet = vkDescriptorSet;
			descriptorWrites[2].dstBinding = 2;
			descriptorWrites[2].dstArrayElement = 0;
			descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[2].descriptorCount = 1;
			descriptorWrites[2].pImageInfo = &imagesInfo[2];

			vkUpdateDescriptorSets(vkDevice->GetLogicalDevice(), 3, descriptorWrites.data(), 0, nullptr);

			vkDescriptor->SetDescriptor(vkDescriptorSet);

			return newDescriptor;
		}

		IDescriptor* VulkanDescriptorAllocator::AllocateDescriptorSet(IDevice* _device, IDescriptorLayout* _descriptorLayout, const std::vector<ShadowMap*>& _dirShadowMaps, const std::vector<ShadowMap*>& _spotShadowMaps, const std::vector<ShadowMap*>& _pointShadowMaps, unsigned int _currentDescriptor)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			IDescriptor* newDescriptor = new VulkanDescriptor();
			VulkanDescriptor* vkDescriptor = newDescriptor->CastToVulkan();
			VkDescriptorSet vkDescriptorSet = vkDescriptor->GetDescriptor();

			VulkanDescriptorLayout* vkDescriptorLayout = _descriptorLayout->CastToVulkan();
			VkDescriptorSetLayout vkDLRaw = vkDescriptorLayout->GetDescriptorLayout();

			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = m_DescriptorAllocator;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &vkDLRaw;

			VkResult result = vkAllocateDescriptorSets(vkDevice->GetLogicalDevice(), &allocInfo, &vkDescriptorSet);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to allocate descriptor sets! Error code: %i", result);
				return nullptr;
			}

			std::array<VkDescriptorImageInfo, IE_MAX_DIRECTIONAL_LIGHTS_SHADOWS> dirLightInfo{};
			std::array<VkDescriptorImageInfo, IE_MAX_SPOT_LIGHTS_SHADOWS> spotLightInfo{};
			std::array<VkDescriptorImageInfo, IE_MAX_POINT_LIGHTS_SHADOWS> pointLightInfo{};

			std::array<VkWriteDescriptorSet, 3> shadowDescriptorWrites{};

			Texture* defaultTexture = ResourceManager::GetInstance()->Get<Texture>("DefaultTexture.jpg");
			Texture* defaultCubemapTexture = ResourceManager::GetInstance()->Get<Texture>("DefaultCubeMapTexture.jpg");

			VkSampler defaultSampler = defaultTexture->GetSampler()->CastToVulkan()->GetSampler();
			VkImageView defaultImageView = defaultTexture->GetImage()->CastToVulkan()->GetImageView();

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
			shadowDescriptorWrites[0].dstSet = vkDescriptorSet;
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
			shadowDescriptorWrites[1].dstSet = vkDescriptorSet;
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
			shadowDescriptorWrites[2].dstSet = vkDescriptorSet;
			shadowDescriptorWrites[2].dstBinding = 2;
			shadowDescriptorWrites[2].dstArrayElement = 0;
			shadowDescriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			shadowDescriptorWrites[2].descriptorCount = static_cast<uint32_t>(pointLightInfo.size());
			shadowDescriptorWrites[2].pImageInfo = pointLightInfo.data();

			vkUpdateDescriptorSets(vkDevice->GetLogicalDevice(), 3, shadowDescriptorWrites.data(), 0, nullptr);

			vkDescriptor->SetDescriptor(vkDescriptorSet);

			return newDescriptor;
		}

		RHIResult VulkanDescriptorAllocator::FreeDescriptorSet(IDevice* _device, IDescriptor* _descriptor)
		{
			if (!_descriptor)
				return RHIResult::IE_RHI_SUCCESS;

			VkDevice device = _device->CastToVulkan()->GetLogicalDevice();
			VkDescriptorSet descriptorSet = _descriptor->CastToVulkan()->GetDescriptor();

			vkFreeDescriptorSets(device, m_DescriptorAllocator, 1, &descriptorSet);

			delete _descriptor;
			_descriptor = nullptr;

			if (!_descriptor)
				return RHIResult::IE_RHI_SUCCESS;

			return RHIResult::IE_RHI_FAIL_UNKNOWN;
		}
	}
}