#pragma once

#include "Core/Graphics/RHI/ISwapChain.h"
#include "Core/Graphics/VulkanRHI/VulkanImage.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace Iris
{
	namespace Engine
	{
		class VulkanDevice;
		class VulkanRenderPass;

		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities{};
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};

		class VulkanSwapChain : public ISwapChain
		{
		public:
			/// <summary>
			/// Default ISwapChain's destructor
			/// </summary>
			IRIS_ENGINE_API	~VulkanSwapChain() override final = default;

			/// <summary>
			/// Casts an ISwapChain into a Vulkan swapChain
			/// </summary>
			/// <returns>The Vulkan swapChain or nullptr</returns>
			IRIS_ENGINE_API VulkanSwapChain* CastToVulkan() override final { return this; }

			/// <summary>
			/// Gets the current frame width
			/// </summary>
			/// <returns>An int corresponding to the size of the frame width</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE unsigned int GetFrameWidth() const override final { return static_cast<unsigned int>(m_SwapChainExtent.width); }

			/// <summary>
			/// Gets the current frame height
			/// </summary>
			/// <returns>An int corresponding to the size of the frame height</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE unsigned int GetFrameHeight() const override final { return static_cast<unsigned int>(m_SwapChainExtent.height); }

			/// <summary>
			/// Gets the raw type of the SwapChain extent
			/// </summary>
			/// <returns>Raw Vulkan SwapChain extent object</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VkExtent2D GetExtent() const { return m_SwapChainExtent; }

			/// <summary>
			/// Gets the raw type of the SwapChain format
			/// </summary>
			/// <returns>Raw Vulkan SwapChain format object</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VkFormat GetFormat() const { return m_SwapChainImageFormat; }

			/// <summary>
			/// Initializes the SwapChain
			/// </summary>
			/// <param name="_window">: Window of the application</param>
			/// <param name="_device">: Device of the application</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API RHIResult Initialize(IWindow* _window, IDevice* _device);

			/// <summary>
			/// Terminates the SwapChain
			/// </summary>
			/// <param name="_Device">: Device of the application</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API RHIResult Terminate(IDevice* _device);

			/// <summary>
			/// Recreates the SwapChain
			/// </summary>
			/// <param name="_window">: Window of the application</param>
			/// <param name="_device">: Device of the application</param>
			/// <param name="_renderPass">: RenderPass of the final result</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API RHIResult RecreateSwapChain(IWindow* _window, IDevice* _device, IRenderPass* _renderPass);

			/// <summary>
			/// Makes the firsts operations of the frame
			/// </summary>
			/// <param name="_window">: Window of the application</param>
			/// <param name="_device">: Device of the application</param>
			/// <param name="_renderPass">: RenderPass of the final result</param>
			/// <returns>The index of the new swap chain image</returns>
			IRIS_ENGINE_API unsigned int StartRenderingFrame(IWindow* _window, IDevice* _device, IRenderPass* _renderPass);

			/// <summary>
			/// Makes the lasts operations of the frame
			/// </summary>
			/// <param name="_window">: Window of the application</param>
			/// <param name="_device">: Device of the application</param>
			/// <param name="_commandBuffer">: Command buffer for operations</param>
			/// <param name="_renderPass">: RenderPass of the final result</param>
			/// <param name="_imageIndex">: Index of the current swap chain image</param>
			IRIS_ENGINE_API void EndRenderingFrame(IWindow* _window, IDevice* _device, ICommandBuffer* _commandBuffer, IRenderPass* _renderPass, unsigned int _imageIndex);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_physicalDevice"></param>
			/// <param name="_surface"></param>
			/// <returns></returns>
			IRIS_ENGINE_API static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface);

		private:
			/// <summary>
			/// Chooses the swap chain format for presentation
			/// </summary>
			/// <param name="_availableFormats">: All available formats</param>
			/// <returns>One of the available formats</returns>
			VkSurfaceFormatKHR ChooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& _availableFormats);

			/// <summary>
			/// Chooses the swap chain presentation mode
			/// </summary>
			/// <param name="_availablePresentModes">: All available presentation modes</param>
			/// <returns>One of the available presentation modes</returns>
			VkPresentModeKHR ChooseSwapChainPresentationMode(const std::vector<VkPresentModeKHR>& _availablePresentModes);

			/// <summary>
			/// Chooses the swap chain extent
			/// </summary>
			/// <param name="_window">: Window of the application</param>
			/// <param name="_capabilities">: Capabilities of the window surface</param>
			/// <returns>The extent of the swap chain</returns>
			VkExtent2D ChooseSwapChainExtent(IWindow* _window, const VkSurfaceCapabilitiesKHR& _capabilities);

			/// <summary>
			/// Creates all synchronisation objects
			/// </summary>
			/// <param name="_device">: Device of the application for creation</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			RHIResult CreateSynchronisationObjects(VulkanDevice* _device);

			/// <summary>
			/// Creates the SwapChain
			/// </summary>
			/// <param name="_window">: Window of the application</param>
			/// <param name="_device">: Device of the application</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			RHIResult CreateSwapChain(IWindow* _window, VulkanDevice* _device);

			/// <summary>
			/// Creates all swap chain frame buffers
			/// </summary>
			/// <param name="_device">: Device of the application</param>
			/// <param name="_renderPass">: Final render pass result that will be rendered in the framebuffer</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			RHIResult CreateSwapChainFramebuffers(VulkanDevice* _device, VulkanRenderPass* _renderPass);

			/// <summary>
			/// Destroys all swap chain frame buffers
			/// </summary>
			/// <param name="_Device">: Device of the application</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			RHIResult DestroySwapChainFramebuffers(IDevice* _device);

			/// <summary>
			/// Cleans objects of the swap chain
			/// </summary>
			/// <param name="_device">: Device of the application</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			RHIResult CleanSwapChain(VulkanDevice* _device);

			/// <summary>
			/// Creates depth resources
			/// </summary>
			/// <param name="_device">: Device of the application</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			RHIResult CreateDepthResources(VulkanDevice* _device);

		private:
			VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

			VkFormat m_SwapChainImageFormat{};
			VkExtent2D m_SwapChainExtent{};
			uint32_t m_ImageCount = 1;

			std::vector<VkSemaphore> m_ImageAvailableSemaphore;
			std::vector<VkSemaphore> m_RenderFinishedSemaphore;
			std::vector<VkFence> m_InFlightFence;

			std::vector<VulkanImage> m_SwapChainImages;
			VulkanImage m_DepthImage;
		};
	}
}