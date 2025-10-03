#include "Core/Graphics/VulkanRHI/VulkanImage.h"

#include "Core/Graphics/VulkanRHI/VulkanDevice.h"
#include "Core/Graphics/VulkanRHI/VulkanRenderInterface.h"
#include "Core/Graphics/VulkanRHI/VulkanBuffer.h"
#include "Core/Graphics/VulkanRHI/VulkanCommandAllocator.h"
#include "Core/Graphics/VulkanRHI/VulkanCommandBuffer.h"

namespace Iris
{
	namespace Engine
	{
		RHIResult VulkanImage::CreateTextureImage(IDevice* _device, ICommandAllocator* _cmdAllocator, unsigned int _width, unsigned int _height, unsigned int _channelCount, void* _data, const RHIImage& _imageInfo)
		{
			p_AttachmentType = _imageInfo.Usage;
			p_TextureType = _imageInfo.Type;
			p_ColorFormat = _imageInfo.ColorFormat;
			p_SampleCount = _imageInfo.SampleCount;
			p_MipCount = _imageInfo.MipCount;

			(void)_channelCount;
			VkDeviceSize layerSize = 0;
			VkDeviceSize totalSize = 0;

			switch (_imageInfo.ColorFormat)
			{
			case RHIColorFormat::IE_RHI_FORMAT_R8G8B8A8_UNORM: case RHIColorFormat::IE_RHI_FORMAT_R8G8B8A8_SRGB: default:
				layerSize = _width * _height * 4;
				break;
			case RHIColorFormat::IE_RHI_FORMAT_R16G16B16A16_SFLOAT:
				layerSize = _width * _height * 8;
				break;
			case RHIColorFormat::IE_RHI_FORMAT_R32G32B32A32_SFLOAT:
				layerSize = _width * _height * 16;
				break;
			}

			unsigned int layerCount = 1;

			switch (p_TextureType)
			{
			case RHITextureType::IE_RHI_TEXTURE_2D: default:
				totalSize = layerSize;
				break;
			case RHITextureType::IE_RHI_TEXTURE_CUBEMAP:
				layerCount = 6;
				totalSize = layerSize * 6;
				break;
			}

			VulkanDevice* vkDevice = _device->CastToVulkan();
			VulkanCommandAllocator* vkCmdAlloc = _cmdAllocator->CastToVulkan();

			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;

			VulkanBuffer vkBuffer;
			vkBuffer.CreateVulkanBuffer(vkDevice, totalSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

			void* data;
			vkMapMemory(vkDevice->GetLogicalDevice(), stagingBufferMemory, 0, layerSize, 0, &data);
			memcpy(data, _data, layerSize);
			vkUnmapMemory(vkDevice->GetLogicalDevice(), stagingBufferMemory);

			CreateImage(vkDevice, _width, _height, _imageInfo.MipCount, _imageInfo.ColorFormat, _imageInfo.Type, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			VulkanImage::TransitionImageLayout(vkDevice, vkCmdAlloc, m_Image, _imageInfo.Type, _imageInfo.ColorFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, _imageInfo.MipCount);

			VulkanBuffer::CopyBufferToImage(vkDevice, vkCmdAlloc, stagingBuffer, _imageInfo.Type, m_Image, static_cast<uint32_t>(_width), static_cast<uint32_t>(_height));

			vkDestroyBuffer(vkDevice->GetLogicalDevice(), stagingBuffer, nullptr);
			vkFreeMemory(vkDevice->GetLogicalDevice(), stagingBufferMemory, nullptr);

			VulkanImage::GenerateMipmaps(vkDevice, vkCmdAlloc, GetImage(), _width, _height, _imageInfo.MipCount, layerCount);

			return RHIResult();
		}

		RHIResult VulkanImage::CreateFramebufferImage(IDevice* _device, ICommandAllocator* _cmdAllocator, unsigned int _width, unsigned int _height, const RHIImage& _imageInfo)
		{
			p_AttachmentType = _imageInfo.Usage;
			p_TextureType = _imageInfo.Type;
			p_ColorFormat = _imageInfo.ColorFormat;
			p_SampleCount = _imageInfo.SampleCount;
			p_MipCount = _imageInfo.MipCount;

			_cmdAllocator;

			switch (_imageInfo.Usage)
			{
			case RHIAttachmentType::IE_RHI_COLOR_ATTACHMENT: default:
				CreateImage(
					_device->CastToVulkan(),
					_width,
					_height,
					_imageInfo.MipCount,
					_imageInfo.ColorFormat,
					_imageInfo.Type,
					VulkanRenderInterface::ConvertSampleCount(_imageInfo.SampleCount),
					VulkanRenderInterface::ConvertUsageFlag(_imageInfo.Usage) | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
					VK_IMAGE_ASPECT_COLOR_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				);
				break;
			case RHIAttachmentType::IE_RHI_DEPTH_ATTACHMENT:
				CreateImage(
					_device->CastToVulkan(),
					_width,
					_height,
					_imageInfo.MipCount,
					_imageInfo.ColorFormat,
					_imageInfo.Type,
					VulkanRenderInterface::ConvertSampleCount(_imageInfo.SampleCount),
					VulkanRenderInterface::ConvertUsageFlag(_imageInfo.Usage) | VK_IMAGE_USAGE_SAMPLED_BIT,
					VK_IMAGE_ASPECT_DEPTH_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				);
				break;
			}

			return RHIResult::IE_RHI_SUCCESS;
		}

		std::vector<IImage*> VulkanImage::GenerateFramebufferMipMaps(IDevice* _device, unsigned int _count)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			std::vector<IImage*> mipMaps(_count);

			VkImageViewType type{};

			switch (p_TextureType)
			{
			case RHITextureType::IE_RHI_TEXTURE_2D: default:
				type = VK_IMAGE_VIEW_TYPE_2D;
				break;
			case RHITextureType::IE_RHI_TEXTURE_CUBEMAP:
				type = VK_IMAGE_VIEW_TYPE_CUBE;
				break;
			}

			for (size_t i = 0; i < _count; ++i)
			{
				VulkanImage* mip = new VulkanImage();

				mip->SetImage(m_Image);
				mip->CreateVulkanImageView(
					vkDevice,
					VulkanRenderInterface::ConvertColorFormat(p_ColorFormat),
					VK_IMAGE_ASPECT_COLOR_BIT,
					type,
					1,
					static_cast<unsigned int>(i)
				);

				mipMaps[i] = mip;
			}

			return mipMaps;
		}

		RHIResult VulkanImage::DestroyFramebufferMipMaps(IDevice* _device, std::vector<IImage*> _mipMaps)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			size_t mipCount = _mipMaps.size();
			for (size_t i = 0; i < mipCount; ++i)
			{
				_mipMaps[i]->CastToVulkan()->DestroyImageView(vkDevice);
				delete _mipMaps[i];
			}

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanImage::DestroyImage(IDevice* _device)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			vkDestroyImageView(vkDevice->GetLogicalDevice(), m_ImageView, nullptr);

			vkDestroyImage(vkDevice->GetLogicalDevice(), m_Image, nullptr);

			vkFreeMemory(vkDevice->GetLogicalDevice(), m_ImageMemory, nullptr);

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanImage::CreateImage(VulkanDevice* _device, unsigned int _width, unsigned int _height, unsigned int _mipLevels, RHIColorFormat _format, RHITextureType _type, VkSampleCountFlagBits _samplesCount, VkImageUsageFlags _usage, VkImageAspectFlags _aspectFlags, VkMemoryPropertyFlags _properties)
		{
			VkImageViewType type{};

			switch (_type)
			{
			case RHITextureType::IE_RHI_TEXTURE_2D: default:
				type = VK_IMAGE_VIEW_TYPE_2D;
				break;
			case RHITextureType::IE_RHI_TEXTURE_CUBEMAP:
				type = VK_IMAGE_VIEW_TYPE_CUBE;
				break;
			}

			CreateVulkanImage(_device, _width, _height, _mipLevels, _samplesCount, _type, _format, _usage);
			AllocateVulkanImage(_device, _properties);
			CreateVulkanImageView(_device, VulkanRenderInterface::ConvertColorFormat(_format), _aspectFlags, type, _mipLevels, 0);

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanImage::DestroyImageOnly(VulkanDevice* _device) const
		{
			vkDestroyImage(_device->GetLogicalDevice(), m_Image, nullptr);

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanImage::DestroyImageView(VulkanDevice* _device) const
		{
			vkDestroyImageView(_device->GetLogicalDevice(), m_ImageView, nullptr);

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanImage::FreeImageMemory(VulkanDevice* _device) const
		{
			vkFreeMemory(_device->GetLogicalDevice(), m_ImageMemory, nullptr);

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanImage::CreateVulkanImageView(VulkanDevice* _device, VkFormat _format, VkImageAspectFlags _aspectFlags, VkImageViewType _viewType, unsigned int _mipCount, unsigned int _mipLevel)
		{
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = m_Image;
			viewInfo.viewType = _viewType;
			viewInfo.format = _format;
			viewInfo.subresourceRange.aspectMask = _aspectFlags;
			viewInfo.subresourceRange.baseMipLevel = _mipLevel;
			viewInfo.subresourceRange.levelCount = _mipCount;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			switch (_viewType)
			{
			case VK_IMAGE_VIEW_TYPE_2D: default:
				viewInfo.subresourceRange.layerCount = 1;
				p_TextureType = RHITextureType::IE_RHI_TEXTURE_2D;
				break;
			case VK_IMAGE_VIEW_TYPE_CUBE:
				viewInfo.subresourceRange.layerCount = 6;
				p_TextureType = RHITextureType::IE_RHI_TEXTURE_CUBEMAP;
				break;
			}

			VkResult result = vkCreateImageView(_device->GetLogicalDevice(), &viewInfo, nullptr, &m_ImageView);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to create image view! Error code: %i", result);
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanImage::TransitionImageLayout(VulkanDevice* _device, VulkanCommandAllocator* _commandAllocator, VkImage _image, RHITextureType _imageFormat, const RHIColorFormat& _format, VkImageLayout _oldLayout, VkImageLayout _newLayout, unsigned int _mipLevels)
		{
			ICommandBuffer* cmdBuffer = _commandAllocator->BeginSingleTimeCommands(_device);
			VulkanCommandBuffer* vkCmdBuffer = cmdBuffer->CastToVulkan();

			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = _oldLayout;
			barrier.newLayout = _newLayout;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.image = _image;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.levelCount = _mipLevels;
			barrier.subresourceRange.baseArrayLayer = 0;
			switch (_imageFormat)
			{
			case RHITextureType::IE_RHI_TEXTURE_2D:		default:
				barrier.subresourceRange.layerCount = 1;
				break;
			case RHITextureType::IE_RHI_TEXTURE_CUBEMAP:
				barrier.subresourceRange.layerCount = 6;
				break;
			}
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = 0;

			VkPipelineStageFlags sourceStage;
			VkPipelineStageFlags destinationStage;

			// For depth buffer
			if (_newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			{
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

				if (HasStencilComponent(VulkanRenderInterface::ConvertColorFormat(_format)))
				{
					barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
				}
			}

			if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			}
			else if (_oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			{
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}
			else if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			{
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			}
			else
			{
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			vkCmdPipelineBarrier(vkCmdBuffer->GetCommandBuffer(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

			_commandAllocator->EndSingleTimeCommands(_device, vkCmdBuffer);

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanImage::GenerateMipmaps(VulkanDevice* _device, VulkanCommandAllocator* _commandAllocator, VkImage _image, unsigned int _width, unsigned int _height, unsigned int _mipLevels, unsigned int _layerCount)
		{
			ICommandBuffer* cmdBuffer = _commandAllocator->BeginSingleTimeCommands(_device);
			VulkanCommandBuffer* vkCmdBuffer = cmdBuffer->CastToVulkan();

			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.image = _image;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = _layerCount;
			barrier.subresourceRange.levelCount = 1;

			int32_t mipWidth = _width;
			int32_t mipHeight = _height;

			for (uint32_t i = 1; i < _mipLevels; ++i)
			{
				barrier.subresourceRange.baseMipLevel = i - 1;
				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

				vkCmdPipelineBarrier(vkCmdBuffer->GetCommandBuffer(),
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
					0, nullptr,
					0, nullptr,
					1, &barrier);

				VkImageBlit blit{};
				blit.srcOffsets[0] = { 0, 0, 0 };
				blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
				blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.srcSubresource.mipLevel = i - 1;
				blit.srcSubresource.baseArrayLayer = 0;
				blit.srcSubresource.layerCount = _layerCount;
				blit.dstOffsets[0] = { 0, 0, 0 };
				blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
				blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.dstSubresource.mipLevel = i;
				blit.dstSubresource.baseArrayLayer = 0;
				blit.dstSubresource.layerCount = _layerCount;

				vkCmdBlitImage(vkCmdBuffer->GetCommandBuffer(),
					_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1, &blit,
					VK_FILTER_LINEAR);

				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				vkCmdPipelineBarrier(vkCmdBuffer->GetCommandBuffer(),
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
					0, nullptr,
					0, nullptr,
					1, &barrier);

				if (mipWidth > 1) mipWidth /= 2;
				if (mipHeight > 1) mipHeight /= 2;
			}

			barrier.subresourceRange.baseMipLevel = _mipLevels - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(vkCmdBuffer->GetCommandBuffer(),
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			_commandAllocator->EndSingleTimeCommands(_device, cmdBuffer);

			return RHIResult::IE_RHI_SUCCESS;
		}

		VkFormat VulkanImage::FindSupportedFormat(VulkanDevice* _device, const std::vector<VkFormat>& _candidates, VkImageTiling _tiling, VkFormatFeatureFlags _features)
		{
			for (VkFormat format : _candidates)
			{
				VkFormatProperties props;
				vkGetPhysicalDeviceFormatProperties(_device->GetPhysicalDevice(), format, &props);

				if (_tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & _features) == _features)
				{
					return format;
				}
				else if (_tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & _features) == _features)
				{
					return format;
				}
			}

			// TODO: ERR

			return VkFormat();
		}

		VkFormat VulkanImage::FindDepthFormat(VulkanDevice* _device)
		{
			return FindSupportedFormat(_device, { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		}

		bool VulkanImage::HasStencilComponent(VkFormat _format)
		{
			return _format == VK_FORMAT_D32_SFLOAT_S8_UINT || _format == VK_FORMAT_D24_UNORM_S8_UINT;
		}

		RHIResult VulkanImage::CreateVulkanImage(VulkanDevice* _device, unsigned int _width, unsigned int _height, unsigned int _mipLevels, VkSampleCountFlagBits _samplesCount, RHITextureType _type, RHIColorFormat _format, VkImageUsageFlags _usage)
		{
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = _width;
			imageInfo.extent.height = _height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = _mipLevels;
			switch (_type)
			{
			case Iris::Engine::RHITextureType::IE_RHI_TEXTURE_2D: default:
				imageInfo.arrayLayers = 1;
				break;
			case Iris::Engine::RHITextureType::IE_RHI_TEXTURE_CUBEMAP:
				imageInfo.arrayLayers = 6;
				break;
			}
			imageInfo.format = VulkanRenderInterface::ConvertColorFormat(_format);
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = _usage;
			imageInfo.samples = _samplesCount;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			switch (_type)
			{
			case Iris::Engine::RHITextureType::IE_RHI_TEXTURE_CUBEMAP:
				imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
				break;
			}

			VkResult result = vkCreateImage(_device->GetLogicalDevice(), &imageInfo, nullptr, &m_Image);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to create image! Error code: %i", result);
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanImage::AllocateVulkanImage(VulkanDevice* _device, VkMemoryPropertyFlags _properties)
		{
			VkMemoryRequirements memRequirements;
			vkGetImageMemoryRequirements(_device->GetLogicalDevice(), m_Image, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = VulkanBuffer::FindMemoryType(_device, memRequirements.memoryTypeBits, _properties);

			VkResult result = vkAllocateMemory(_device->GetLogicalDevice(), &allocInfo, nullptr, &m_ImageMemory);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to allocate image! Error code: %i", result);
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			vkBindImageMemory(_device->GetLogicalDevice(), m_Image, m_ImageMemory, 0);

			return RHIResult::IE_RHI_SUCCESS;
		}
	}
}