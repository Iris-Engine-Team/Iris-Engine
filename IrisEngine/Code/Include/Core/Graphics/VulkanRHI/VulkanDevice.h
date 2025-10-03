#pragma once

#include "Core/Graphics/RHI/IDevice.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <optional>

namespace Iris
{
	namespace Engine
	{
		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;
		};

		class VulkanDevice : public IDevice
		{
		public:
			///////////////////////////////////////////////////////////////////////
			///																	///
			/// 		Constructor and destructors								///
			///																	///
			///////////////////////////////////////////////////////////////////////

			/// <summary>
			/// Default VulkanDevice's destructor
			/// </summary>
			IRIS_ENGINE_API ~VulkanDevice() override = default;

			///////////////////////////////////////////////////////////////////////
			///																	///
			/// 		Getters													///
			///																	///
			///////////////////////////////////////////////////////////////////////

			/// <summary>
			/// Casts an IDevice to Vulkan device
			/// </summary>
			/// <returns>A pointer to the Vulkan device</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VulkanDevice* CastToVulkan() override final { return this; }

			/// <summary>
			/// Gets the raw type of the Vulkan instance
			/// </summary>
			/// <returns>Raw Vulkan instance object</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VkInstance GetInstance() const { return m_Instance; }

			/// <summary>
			/// Gets the raw type of the Vulkan physical device
			/// </summary>
			/// <returns>Raw Vulkan physical device object</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }

			/// <summary>
			/// Gets the raw type of the Vulkan device
			/// </summary>
			/// <returns>Raw Vulkan device object</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VkDevice GetLogicalDevice() const { return m_LogicalDevice; }

			/// <summary>
			/// Gets the raw type of the Vulkan surface
			/// </summary>
			/// <returns>Raw Vulkan surface object</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VkSurfaceKHR GetSurface() const { return m_Surface; }

			/// <summary>
			/// Gets the raw type of the Vulkan graphics queue
			/// </summary>
			/// <returns>Raw Vulkan graphics queue object</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }

			/// <summary>
			/// Gets the raw type of the Vulkan presentation queue
			/// </summary>
			/// <returns>Raw Vulkan presentation queue object</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VkQueue GetPresentationQueue() const { return m_PresentQueue; }

			/// <summary>
			/// Gets the raw type of the Vulkan sample count
			/// </summary>
			/// <returns>Raw Vulkan sample count object</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VkSampleCountFlagBits GetMSAASamplesCount() const { return m_MSAASamples; }

#ifndef NDEBUG
			IE_FORCE_INLINE PFN_vkCmdBeginDebugUtilsLabelEXT GetBegingDebugLabel() const { return m_BeginDebugUtilsLabelEXT ? m_BeginDebugUtilsLabelEXT : VK_NULL_HANDLE; }
			IE_FORCE_INLINE PFN_vkCmdEndDebugUtilsLabelEXT GetEndDebugLabel() const { return  m_EndDebugUtilsLabelEXT ? m_EndDebugUtilsLabelEXT : VK_NULL_HANDLE; }
#endif

			///////////////////////////////////////////////////////////////////////
			///																	///
			/// 		Initialization and Termination methods					///
			///																	///
			///////////////////////////////////////////////////////////////////////

			/// <summary>
			/// Initializes the device
			/// </summary>
			/// <param name="_Window">: Window of the application</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API RHIResult Initialize(IWindow* _window) override final;

			/// <summary>
			/// Terminates the device
			/// </summary>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API RHIResult Terminate() override final;

			/// <summary>
			/// Gets all queue families available and required for the application
			/// </summary>
			/// <param name="_device">: Physical device where the queues are</param>
			/// <param name="_surface">: Surface to get presentation queue</param>
			/// <returns>A struct containing the different queue families</returns>
			IRIS_ENGINE_API static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice _device, VkSurfaceKHR _surface);

			/// <summary>
			/// Waits the device idle state
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API void WaitIdle() override final;

			IRIS_ENGINE_API void WaitGraphicsQueueIdle() override final;

		private:

			///////////////////////////////////////////////////////////////////////
			///																	///
			/// 		Methods													///
			///																	///
			///////////////////////////////////////////////////////////////////////

			/// <summary>
			/// Creates the Vulkan instance of the application
			/// </summary>
			/// <param name="_Window">: Current window context</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			RHIResult CreateInstance(IWindow* _window);

			/// <summary>
			/// Destroys the Vulkan instance of the application
			/// </summary>
			void DestroyInstance();

			/// <summary>
			/// Creates the Vulkan physical device of the application
			/// </summary>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			RHIResult PickPhysicalDevice();

			/// <summary>
			/// Checks if the current physical device corresponds to what the application needs
			/// </summary>
			/// <param name="_PhysicalDevice">: Physical device that is checked</param>
			/// <returns>A bool, true if the device is suitable, false if not</returns>
			bool IsPhysicalDeviceSuitable(VkPhysicalDevice _physicalDevice);

			/// <summary>
			/// Checks if the physical device supports all required extensions
			/// </summary>
			/// <param name="_PhysicalDevice">: Physical device that is checked</param>
			/// <returns>A bool, true if the device is supports extensions, false if not</returns>
			bool CheckPhysicalDeviceExtensionSupport(VkPhysicalDevice _physicalDevice);

			/// <summary>
			/// Checks the max numbers of samples the physical device can uses
			/// </summary>
			/// <param name="_device">: Physical device that is checked</param>
			/// <returns>The number of samples usable</returns>
			VkSampleCountFlagBits GetMaxUsableSampleCount(VkPhysicalDevice _device);

			/// <summary>
			/// Creates the Vulkan logical device
			/// </summary>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			RHIResult CreateLogicalDevice();

			/// <summary>
			/// Destroys the Vulkan logical device
			/// </summary>
			void DestroyLogicalDevice();

			/// <summary>
			/// Creates the debug messenger for validation layers
			/// </summary>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			RHIResult CreateDebugMessenger();

			/// <summary>
			/// Destroys the debug messenger
			/// </summary>
			void DestroyDebugMessenger();

			/// <summary>
			/// Creates the window surface for Vulkan rendering
			/// </summary>
			/// <param name="_window">: Window of the application</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			RHIResult CreateSurface(IWindow* _window);

			/// <summary>
			/// Destroys the surface for Vulkan rendering
			/// </summary>
			void DestroySurface();

			/// <summary>
			/// Populates the create info structure for the debug messenger
			/// </summary>
			/// <param name="createInfo">: Structure to fill</param>
			void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& _createInfo);

			/// <summary>
			/// Gets all required extensions of the window for Vulkan
			/// </summary>
			/// <param name="_Window">: Window of the application for extensions</param>
			/// <returns>A vector of all the required extensions of the window</returns>
			std::vector<const char*> GetRequiredExtensions(IWindow* _window);

			/// <summary>
			/// Checks if the requested validation layers are available with Vulkan
			/// </summary>
			/// <returns>A bool true if the validation layers are available, false if not</returns>
			bool CheckValidationLayersSupport();

			///////////////////////////////////////////////////////////////////////
			///																	///
			/// 		Debug methods											///
			///																	///
			///////////////////////////////////////////////////////////////////////

			/// <summary>
			/// Creates the debug messenger
			/// </summary>
			/// <param name="_instance">: The Vulkan instance that the messenger will be associated with</param>
			/// <param name="_createInfo">: Create info structure for the debug messenger</param>
			/// <param name="_allocator">: A pointer to custom allocation callbacks, or nullptr to use the default allocator</param>
			/// <param name="_debugMessenger">: A pointer to the debug messenger object</param>
			/// <returns></returns>
			VkResult CreateDebugUtilsMessengerEXT(VkInstance _instance, const VkDebugUtilsMessengerCreateInfoEXT* _createInfo, const VkAllocationCallbacks* _allocator, VkDebugUtilsMessengerEXT* _debugMessenger);

			/// <summary>
			/// Destroys the debug messenger
			/// </summary>
			/// <param name="_instance">: The Vulkan instance associated with the debug messenger</param>
			/// <param name="_debugMessenger">: A pointer to the debug messenger object</param>
			/// <param name="_allocator">: A pointer to custom allocation callbacks, or nullptr to use the default allocator</param>
			void DestroyDebugUtilsMessengerEXT(VkInstance _instance, VkDebugUtilsMessengerEXT _debugMessenger, const VkAllocationCallbacks* _allocator);

			/// <summary>
			/// Callback function for handling validation layer debug messages.
			/// </summary>
			/// <param name="_messageSeverity">: The severity of the message</param>
			/// <param name="_messageType">: The type of the message</param>
			/// <param name="_callBackData">: A pointer to a structure containing the details of the debug message, including the message string</param>
			/// <param name="_userData">: A pointer to user data, which can be used to pass additional context to the callback</param>
			/// <returns>VK_FALSE to indicate the callback has been processed (VK_FALSE prevents further propagation of the message)</returns>
			static VKAPI_ATTR VkBool32 VKAPI_CALL ValidationLayersDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT _messageSeverity, VkDebugUtilsMessageTypeFlagsEXT _messageType, const VkDebugUtilsMessengerCallbackDataEXT* _callBackData, void* _userData);

		private:
			VkInstance m_Instance = VK_NULL_HANDLE;

			VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
			VkDevice m_LogicalDevice = VK_NULL_HANDLE;

			VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;

			PFN_vkCmdBeginDebugUtilsLabelEXT m_BeginDebugUtilsLabelEXT = VK_NULL_HANDLE;
			PFN_vkCmdEndDebugUtilsLabelEXT m_EndDebugUtilsLabelEXT = VK_NULL_HANDLE;

			VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

			VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
			VkQueue m_PresentQueue = VK_NULL_HANDLE;

			VkSampleCountFlagBits m_MSAASamples = VK_SAMPLE_COUNT_1_BIT;

			std::vector<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_maintenance1" };

#ifdef NDEBUG
			std::vector<const char*>  m_ValidationLayers = { };
			const bool m_EnableValidationLayers = false;
#else
			std::vector<const char*>  m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
			const bool m_EnableValidationLayers = true;
#endif

		};
	}
}