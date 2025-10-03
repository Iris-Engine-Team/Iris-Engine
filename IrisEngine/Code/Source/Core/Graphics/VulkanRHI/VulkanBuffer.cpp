#include "Core/Graphics/VulkanRHI/VulkanBuffer.h"

#include "Core/Graphics/VulkanRHI/VulkanDevice.h"
#include "Core/Graphics/VulkanRHI/VulkanCommandAllocator.h"
#include "Core/Graphics/VulkanRHI/VulkanCommandBuffer.h"

namespace Iris
{
	namespace Engine
	{
		RHIResult VulkanBuffer::CreateBuffer(IDevice* _device, ICommandAllocator* _commandAllocator, RHIBufferType _bufferType, size_t _bufferSize, void* _data)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			VkDeviceSize bufferSize = static_cast<VkDeviceSize>(_bufferSize);

			if (_bufferType == RHIBufferType::IE_RHI_UNIFORM_BUFFER)
			{
				RHIResult result = CreateVulkanUniformBuffer(vkDevice, bufferSize, _data);
				return result;
			}

			VkBuffer stagingBuffer = VK_NULL_HANDLE;
			VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

			CreateVulkanBuffer(vkDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

			void* tempData;
			vkMapMemory(vkDevice->GetLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &tempData);
			memcpy(tempData, _data, (size_t)bufferSize);
			vkUnmapMemory(vkDevice->GetLogicalDevice(), stagingBufferMemory);

			switch (_bufferType)
			{
			case RHIBufferType::IE_RHI_VERTEX_BUFFER: default:
				CreateVulkanBuffer(vkDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffer, m_BufferMemory);
				break;
			case RHIBufferType::IE_RHI_INDEX_BUFFER:
				CreateVulkanBuffer(vkDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffer, m_BufferMemory);
				break;
			}

			CopyBuffer(vkDevice, _commandAllocator->CastToVulkan(), stagingBuffer, m_Buffer, bufferSize);

			vkDestroyBuffer(vkDevice->GetLogicalDevice(), stagingBuffer, nullptr);
			vkFreeMemory(vkDevice->GetLogicalDevice(), stagingBufferMemory, nullptr);

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanBuffer::DestroyBuffer(IDevice* _device)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			vkDestroyBuffer(vkDevice->GetLogicalDevice(), m_Buffer, nullptr);
			vkFreeMemory(vkDevice->GetLogicalDevice(), m_BufferMemory, nullptr);

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanBuffer::CreateVulkanBuffer(VulkanDevice* _device, VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, VkBuffer& _buffer, VkDeviceMemory& _bufferMemory)
		{
			VkDevice vkLogicalDevice = _device->GetLogicalDevice();

			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = _size;
			bufferInfo.usage = _usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VkResult result = vkCreateBuffer(vkLogicalDevice, &bufferInfo, nullptr, &_buffer);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to create buffer, error code %i", result);
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(vkLogicalDevice, _buffer, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = FindMemoryType(_device, memRequirements.memoryTypeBits, _properties);

			result = vkAllocateMemory(vkLogicalDevice, &allocInfo, nullptr, &_bufferMemory);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to allocate memory, error code %i", result);
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			vkBindBufferMemory(vkLogicalDevice, _buffer, _bufferMemory, 0);

			return RHIResult::IE_RHI_SUCCESS;
		}

		unsigned int VulkanBuffer::FindMemoryType(VulkanDevice* _device, uint32_t _typeFilter, VkMemoryPropertyFlags _properties)
		{
			VkPhysicalDeviceMemoryProperties memProperties;
			vkGetPhysicalDeviceMemoryProperties(_device->GetPhysicalDevice(), &memProperties);

			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			{
				if (_typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & _properties) == _properties)
				{
					return i;
				}
			}

			// TODO: WARN
			return 0;
		}

		void VulkanBuffer::UpdateUniformBuffer(IDevice* _device, void* _data, size_t _dataSize)
		{
			VulkanDevice device = *_device->CastToVulkan();

			void* mappedMemory;
			VkResult result = vkMapMemory(device.GetLogicalDevice(), m_BufferMemory, 0, _dataSize, 0, &mappedMemory);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to map memory while updating uniform buffer !");
				return;
			}

			memcpy(mappedMemory, _data, _dataSize);

			vkUnmapMemory(device.GetLogicalDevice(), m_BufferMemory);
		}

		RHIResult VulkanBuffer::CopyBufferToImage(VulkanDevice* _device, VulkanCommandAllocator* _commandAllocator, VkBuffer _buffer, RHITextureType _imageType, VkImage _image, unsigned int _width, unsigned int _height)
		{
			ICommandBuffer* cmdBuffer = _commandAllocator->BeginSingleTimeCommands(_device);

			VkBufferImageCopy region{};
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;

			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			switch (_imageType)
			{
			case RHITextureType::IE_RHI_TEXTURE_2D:		default:
				region.imageSubresource.layerCount = 1;
				break;
			case RHITextureType::IE_RHI_TEXTURE_CUBEMAP:
				region.imageSubresource.layerCount = 6;
				break;
			}

			region.imageOffset = { 0, 0, 0 };
			region.imageExtent = { _width, _height, 1 };

			vkCmdCopyBufferToImage(cmdBuffer->CastToVulkan()->GetCommandBuffer(), _buffer, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

			_commandAllocator->EndSingleTimeCommands(_device, cmdBuffer);

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanBuffer::CreateVulkanUniformBuffer(VulkanDevice* _device, VkDeviceSize _size, void* _data)
		{
			_data;
			RHIResult result = CreateVulkanBuffer(_device, _size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_Buffer, m_BufferMemory);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to create uniform buffer, error code %i", result);
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			return result;
		}

		RHIResult VulkanBuffer::CopyBuffer(VulkanDevice* _device, VulkanCommandAllocator* _commandAllocator, VkBuffer _srcBuffer, VkBuffer _dstBuffer, VkDeviceSize _size)
		{
			ICommandBuffer* singleCmdBuffer = _commandAllocator->BeginSingleTimeCommands(_device);
			VulkanCommandBuffer* vkSingleCmdBuffer = singleCmdBuffer->CastToVulkan();

			VkBufferCopy copyRegion{};
			copyRegion.srcOffset = 0; // Optional
			copyRegion.dstOffset = 0; // Optional
			copyRegion.size = _size;

			VkCommandBuffer vkCmdBuffer = vkSingleCmdBuffer->GetCommandBuffer();

			vkCmdCopyBuffer(vkCmdBuffer, _srcBuffer, _dstBuffer, 1, &copyRegion);

			_commandAllocator->EndSingleTimeCommands(_device, singleCmdBuffer);

			return RHIResult::IE_RHI_SUCCESS;
		}
	}
}