#include "Core/Graphics/VulkanRHI/VulkanCommandAllocator.h"

#include "Core/Graphics/VulkanRHI/VulkanDevice.h"
#include "Core/Graphics/VulkanRHI/VulkanCommandBuffer.h"

namespace Iris
{
	namespace Engine
	{
		RHIResult VulkanCommandAllocator::Initialize(IDevice* _device)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			QueueFamilyIndices indices = VulkanDevice::FindQueueFamilies(vkDevice->GetPhysicalDevice(), vkDevice->GetSurface());

			VkCommandPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			poolInfo.queueFamilyIndex = indices.graphicsFamily.value();

			VkResult result = vkCreateCommandPool(vkDevice->GetLogicalDevice(), &poolInfo, nullptr, &m_CommandPool);

			if (result != VK_SUCCESS)
			{
				// TODO: ERR
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanCommandAllocator::Terminate(IDevice* _device)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			vkDestroyCommandPool(vkDevice->GetLogicalDevice(), m_CommandPool, nullptr);

			return RHIResult::IE_RHI_SUCCESS;
		}

		ICommandBuffer* VulkanCommandAllocator::AllocateCommandBuffer(IDevice* _device)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			ICommandBuffer* vulkanCommandBuffer = new VulkanCommandBuffer;
			VulkanCommandBuffer* vkCmdBuffer = vulkanCommandBuffer->CastToVulkan();

			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = m_CommandPool;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = 1;

			VkCommandBuffer cmdBuffer = vkCmdBuffer->GetCommandBuffer();

			VkResult result = vkAllocateCommandBuffers(vkDevice->GetLogicalDevice(), &allocInfo, &cmdBuffer);

			vkCmdBuffer->SetCommandBuffer(cmdBuffer);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to allocate command buffer, error code %i", result);
				return nullptr;
			}

			return vulkanCommandBuffer;
		}

		ICommandBuffer* VulkanCommandAllocator::BeginSingleTimeCommands(IDevice* _device)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			ICommandBuffer* cmdBuffer = new VulkanCommandBuffer;
			VulkanCommandBuffer* vkCmdBuffer = cmdBuffer->CastToVulkan();

			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = m_CommandPool;
			allocInfo.commandBufferCount = 1;

			VkCommandBuffer vcmdBuffer = vkCmdBuffer->GetCommandBuffer();

			vkAllocateCommandBuffers(vkDevice->GetLogicalDevice(), &allocInfo, &vcmdBuffer);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			// TODO: Change parameters
			vkBeginCommandBuffer(vcmdBuffer, &beginInfo);

			vkCmdBuffer->SetCommandBuffer(vcmdBuffer);

			return vkCmdBuffer;
		}

		RHIResult VulkanCommandAllocator::EndSingleTimeCommands(IDevice* _device, ICommandBuffer* _commandBuffer)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();
			VulkanCommandBuffer* vkCmdBuffer = _commandBuffer->CastToVulkan();

			VkCommandBuffer vcmdBuffer = vkCmdBuffer->GetCommandBuffer();

			vkEndCommandBuffer(vcmdBuffer);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &vcmdBuffer;

			vkQueueSubmit(vkDevice->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);

			vkQueueWaitIdle(vkDevice->GetGraphicsQueue());

			vkFreeCommandBuffers(vkDevice->GetLogicalDevice(), m_CommandPool, 1, &vcmdBuffer);

			delete _commandBuffer;

			return RHIResult::IE_RHI_SUCCESS;
		}
	}
}