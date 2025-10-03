#define NOMINMAX

#include "Core/Graphics/VulkanRHI/VulkanSwapChain.h"
#include "Core/Graphics/VulkanRHI/VulkanDevice.h"
#include "Core/Graphics/VulkanRHI/VulkanCommandBuffer.h"
#include "Core/Graphics/VulkanRHI/VulkanRenderPass.h"
#include "Core/Graphics/VulkanRHI/VulkanFramebuffer.h"
#include "Core/Graphics/VulkanRHI/VulkanImage.h"
#include "Core/IWindow.h"

#include <algorithm>

namespace Iris
{
	namespace Engine
	{
		RHIResult VulkanSwapChain::Initialize(IWindow* _window, IDevice* _device)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			CreateSwapChain(_window, vkDevice);

			CreateSynchronisationObjects(vkDevice);

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanSwapChain::Terminate(IDevice* _device)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			CleanSwapChain(vkDevice);

			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
			{
				vkDestroySemaphore(vkDevice->GetLogicalDevice(), m_ImageAvailableSemaphore[i], nullptr);
				vkDestroySemaphore(vkDevice->GetLogicalDevice(), m_RenderFinishedSemaphore[i], nullptr);
				vkDestroyFence(vkDevice->GetLogicalDevice(), m_InFlightFence[i], nullptr);
			}

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanSwapChain::RecreateSwapChain(IWindow* _window, IDevice* _device, IRenderPass* _renderPass)
		{	
			VulkanDevice* vkDevice = _device->CastToVulkan();
			VulkanRenderPass* vkRenderPass = _renderPass->CastToVulkan();

			vkDeviceWaitIdle(vkDevice->GetLogicalDevice());

			CleanSwapChain(vkDevice);

			CreateSwapChain(_window, vkDevice);
			CreateDepthResources(vkDevice);
			CreateSwapChainFramebuffers(vkDevice, vkRenderPass);

			return RHIResult::IE_RHI_SUCCESS;
		}

		unsigned int VulkanSwapChain::StartRenderingFrame(IWindow* _window, IDevice* _device, IRenderPass* _renderPass)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();
			VkDevice logicalDevice = vkDevice->GetLogicalDevice();

			vkWaitForFences(logicalDevice, 1, &m_InFlightFence[p_CurrentFrame], VK_TRUE, UINT64_MAX);

			uint32_t imageIndex;
			VkResult result = vkAcquireNextImageKHR(logicalDevice, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphore[p_CurrentFrame], VK_NULL_HANDLE, &imageIndex);

			p_CurrentSwapChainImage = imageIndex;

			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				RecreateSwapChain(_window, _device, _renderPass);
				return p_CurrentSwapChainImage;
			}
			else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			{
				DEBUG_ERROR("Vulkan: Failed to start frame. Error Code: %i", result);
			}

			vkResetFences(logicalDevice, 1, &m_InFlightFence[p_CurrentFrame]);

			return p_CurrentSwapChainImage;
		}

		void VulkanSwapChain::EndRenderingFrame(IWindow* _window, IDevice* _device, ICommandBuffer* _commandBuffer, IRenderPass* _renderPass, unsigned int _imageIndex)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();
			VulkanCommandBuffer* vkCmdBuffer = _commandBuffer->CastToVulkan();
			VkCommandBuffer cmdB = vkCmdBuffer->GetCommandBuffer();


			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

			VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphore[p_CurrentFrame] };
			VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = waitStages;

			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &cmdB;

			VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphore[p_CurrentFrame] };
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphores;

			VkResult result = vkQueueSubmit(vkDevice->GetGraphicsQueue(), 1, &submitInfo, m_InFlightFence[p_CurrentFrame]);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to submit command buffer to queue. Error Code %i", result);
			}

			VkPresentInfoKHR presentInfo{};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = signalSemaphores;

			VkSwapchainKHR swapChains[] = { m_SwapChain };
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = swapChains;
			presentInfo.pImageIndices = &_imageIndex;
			presentInfo.pResults = nullptr;

			result = vkQueuePresentKHR(vkDevice->GetPresentationQueue(), &presentInfo);

			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || p_FrameResized)
			{
				p_FrameResized = false;
				RecreateSwapChain(_window, _device, _renderPass);
			}
			else if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to present frame to screen. Error Code %i", result);
			}

			p_CurrentFrame = (p_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
		}

		SwapChainSupportDetails VulkanSwapChain::QuerySwapChainSupport(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface)
		{
			SwapChainSupportDetails details;

			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, _surface, &details.capabilities);

			uint32_t formatCount;
			vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &formatCount, nullptr);

			if (formatCount != 0)
			{
				details.formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &formatCount, details.formats.data());
			}

			uint32_t presentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &presentModeCount, nullptr);

			if (presentModeCount != 0)
			{
				details.presentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &presentModeCount, details.presentModes.data());
			}

			return details;

		}

		VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& _availableFormats)
		{
			for (const VkSurfaceFormatKHR& availableFormat : _availableFormats)
			{
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					return availableFormat;
				}
			}

			return _availableFormats[0];

		}

		VkPresentModeKHR VulkanSwapChain::ChooseSwapChainPresentationMode(const std::vector<VkPresentModeKHR>& _availablePresentModes)
		{
			for (const VkPresentModeKHR& availablePresent : _availablePresentModes)
			{
				if (availablePresent == VK_PRESENT_MODE_IMMEDIATE_KHR)
				{
					return availablePresent;
				}
			}

			return VK_PRESENT_MODE_FIFO_KHR;
		}

		VkExtent2D VulkanSwapChain::ChooseSwapChainExtent(IWindow* _window, const VkSurfaceCapabilitiesKHR& _capabilities)
		{
			if (_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				return _capabilities.currentExtent;
			}

			int width = 0, height = 0;
			_window->GetWindowSize(&width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, _capabilities.minImageExtent.width, _capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, _capabilities.minImageExtent.height, _capabilities.maxImageExtent.height);

			return actualExtent;
		}

		RHIResult VulkanSwapChain::CreateSynchronisationObjects(VulkanDevice* _device)
		{
			m_ImageAvailableSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
			m_RenderFinishedSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
			m_InFlightFence.resize(MAX_FRAMES_IN_FLIGHT);

			VkSemaphoreCreateInfo semaphoreInfo{};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo fenceInfo{};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
			{
				VkResult result = vkCreateSemaphore(_device->GetLogicalDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore[i]);

				if (result != VK_SUCCESS)
				{
					DEBUG_ERROR("Vulkan: Failed to create Vulkan semaphore. Error code: %i", result);
					return RHIResult::IE_RHI_FAIL_UNKNOWN;
				}

				result = vkCreateSemaphore(_device->GetLogicalDevice(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore[i]);

				if (result != VK_SUCCESS)
				{
					DEBUG_ERROR("Vulkan: Failed to create Vulkan semaphore. Error code: %i", result);
					return RHIResult::IE_RHI_FAIL_UNKNOWN;
				}

				result = vkCreateFence(_device->GetLogicalDevice(), &fenceInfo, nullptr, &m_InFlightFence[i]);

				if (result != VK_SUCCESS)
				{
					DEBUG_ERROR("Vulkan: Failed to create Vulkan fence. Error code: %i", result);
					return RHIResult::IE_RHI_FAIL_UNKNOWN;
				}
			}

			return RHIResult::IE_RHI_SUCCESS;

		}

		RHIResult VulkanSwapChain::CreateSwapChain(IWindow* _window, VulkanDevice* _device)
		{
			SwapChainSupportDetails supportDetails = QuerySwapChainSupport(_device->GetPhysicalDevice(), _device->GetSurface());

			VkSurfaceFormatKHR surfaceFormat = ChooseSwapChainSurfaceFormat(supportDetails.formats);
			VkPresentModeKHR presentationMode = ChooseSwapChainPresentationMode(supportDetails.presentModes);
			VkExtent2D extent = ChooseSwapChainExtent(_window, supportDetails.capabilities);

			uint32_t imageCount = supportDetails.capabilities.minImageCount + 1;

			if (supportDetails.capabilities.maxImageCount > 0 && imageCount > supportDetails.capabilities.maxImageCount)
			{
				imageCount = supportDetails.capabilities.maxImageCount;
			}

			VkSwapchainCreateInfoKHR createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			createInfo.surface = _device->GetSurface();
			createInfo.minImageCount = imageCount;
			createInfo.imageFormat = surfaceFormat.format;
			createInfo.imageColorSpace = surfaceFormat.colorSpace;
			createInfo.imageExtent = extent;
			createInfo.imageArrayLayers = 1;
			createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

			QueueFamilyIndices indices = VulkanDevice::FindQueueFamilies(_device->GetPhysicalDevice(), _device->GetSurface());
			uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

			if (indices.graphicsFamily != indices.presentFamily)
			{
				createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				createInfo.queueFamilyIndexCount = 2;
				createInfo.pQueueFamilyIndices = queueFamilyIndices;
			}
			else
			{
				createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				createInfo.queueFamilyIndexCount = 0;
				createInfo.pQueueFamilyIndices = nullptr;
			}

			createInfo.preTransform = supportDetails.capabilities.currentTransform;
			createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			createInfo.presentMode = presentationMode;
			createInfo.clipped = VK_TRUE;
			createInfo.oldSwapchain = VK_NULL_HANDLE;

			VkResult result = vkCreateSwapchainKHR(_device->GetLogicalDevice(), &createInfo, nullptr, &m_SwapChain);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to create SwapChain. Error code: %i", result);
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			m_SwapChainImageFormat = surfaceFormat.format;
			m_SwapChainExtent = extent;

			std::vector<VkImage> vkImages;

			vkGetSwapchainImagesKHR(_device->GetLogicalDevice(), m_SwapChain, &imageCount, nullptr);

			vkImages.resize(imageCount);
			m_SwapChainImages.resize(imageCount);

			vkGetSwapchainImagesKHR(_device->GetLogicalDevice(), m_SwapChain, &imageCount, vkImages.data());

			for (size_t i = 0; i < imageCount; ++i)
			{
				m_SwapChainImages[i].SetImage(vkImages[i]);
				m_SwapChainImages[i].CreateVulkanImageView(_device, m_SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 0);
			}

			m_ImageCount = imageCount;

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanSwapChain::CreateSwapChainFramebuffers(VulkanDevice* _device, VulkanRenderPass* _renderPass)
		{
			p_SwapChainFramebuffers.resize(m_ImageCount);

			for (size_t i = 0; i < m_ImageCount; i++)
			{
				p_SwapChainFramebuffers[i] = new VulkanFramebuffer;
				VulkanFramebuffer* vkFramebuffer = p_SwapChainFramebuffers[i]->CastToVulkan();

				std::vector<VkImageView> attachmentsView(2);
				attachmentsView[0] = m_SwapChainImages[i].GetImageView();
				attachmentsView[1] = m_DepthImage.GetImageView();

				vkFramebuffer->CreateFramebuffer(_device, _renderPass, static_cast<unsigned int>(m_SwapChainExtent.width), static_cast<unsigned int>(m_SwapChainExtent.height), 1, attachmentsView);
			}

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanSwapChain::DestroySwapChainFramebuffers(IDevice* _device)
		{
			if (p_SwapChainFramebuffers.size() <= 0)
				return RHIResult::IE_RHI_SUCCESS;

			for (size_t i = 0; i < m_ImageCount; i++)
			{
				p_SwapChainFramebuffers[i]->DestroyFramebuffer(_device);
				delete p_SwapChainFramebuffers[i];
			}

			p_SwapChainFramebuffers.clear();

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanSwapChain::CleanSwapChain(VulkanDevice* _device)
		{
			vkDestroySwapchainKHR(_device->GetLogicalDevice(), m_SwapChain, nullptr);

			for (VulkanImage& image : m_SwapChainImages)
			{
				image.DestroyImageView(_device);
			}

			m_SwapChainImages.clear();

			DestroySwapChainFramebuffers(_device);
			
			m_DepthImage.DestroyImage(_device);

			return RHIResult::IE_RHI_SUCCESS;

		}

		RHIResult VulkanSwapChain::CreateDepthResources(VulkanDevice* _device)
		{
			VkFormat depthFormat = VulkanImage::FindDepthFormat(_device);

			RHIColorFormat f = RHIColorFormat::IE_RHI_FORMAT_D32_SFLOAT;
			
			// TODO: REMOVE THIS
			switch (depthFormat)
			{
			case VK_FORMAT_D32_SFLOAT:
				f = RHIColorFormat::IE_RHI_FORMAT_D32_SFLOAT;
				break;
			case VK_FORMAT_D32_SFLOAT_S8_UINT:
				f = RHIColorFormat::IE_RHI_FORMAT_D32_S8_SFLOAT;
				break;
			case VK_FORMAT_D24_UNORM_S8_UINT:
				f = RHIColorFormat::IE_RHI_FORMAT_D24_S8_UNORM;
				break;
			}

			m_DepthImage.CreateImage(_device, m_SwapChainExtent.width, m_SwapChainExtent.height, 1, f, RHITextureType::IE_RHI_TEXTURE_2D, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			return RHIResult::IE_RHI_SUCCESS;

		}
	}
}