#include "Core/Graphics/VulkanRHI/VulkanDevice.h"

#include "Core/Graphics/VulkanRHI/VulkanSwapChain.h"

#include "Core/GLFWWindow.h"

#include <set>
#include <string>

namespace Iris
{
	namespace Engine
	{
		RHIResult VulkanDevice::Initialize(IWindow* _window)
		{
			CreateInstance(_window);
			CreateDebugMessenger();
			CreateSurface(_window);
			PickPhysicalDevice();
			CreateLogicalDevice();

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanDevice::Terminate()
		{
			DestroyLogicalDevice();
			DestroyDebugMessenger();
			DestroySurface();
			DestroyInstance();

			return RHIResult::IE_RHI_SUCCESS;
		}

		QueueFamilyIndices VulkanDevice::FindQueueFamilies(VkPhysicalDevice _device, VkSurfaceKHR _surface)
		{
			QueueFamilyIndices indices;

			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, queueFamilies.data());

			for (uint32_t i = 0; i < queueFamilyCount; ++i)
			{
				if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
					indices.graphicsFamily = i;

				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(_device, i, _surface, &presentSupport);

				if (presentSupport)
				{
					indices.presentFamily = i;
				}
				// Can get out earlier from the loop by checking if all the queues are found
			}

			return indices;
		}
		
		void VulkanDevice::WaitIdle()
		{
			vkDeviceWaitIdle(m_LogicalDevice);
		}

		void VulkanDevice::WaitGraphicsQueueIdle()
		{
			vkQueueWaitIdle(m_GraphicsQueue);
		}

		RHIResult VulkanDevice::CreateInstance(IWindow* _window)
		{
			if (m_EnableValidationLayers && !CheckValidationLayersSupport())
			{
				DEBUG_ERROR("Vulkan: Failed to initialize Vulkan instance! Validation layers asked but not supported");
			}

			// Sets all the information of the application
			VkApplicationInfo appInfos{};
			appInfos.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfos.pApplicationName = "Iris Renderer";
			appInfos.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfos.pEngineName = "Iris Engine";
			appInfos.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfos.apiVersion = VK_API_VERSION_1_0;

			// Gets the required extensions for Vulkan
			std::vector<const char*> extensions = GetRequiredExtensions(_window);

			// Creates all the informations to create an instance
			VkInstanceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfos;
			createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			createInfo.ppEnabledExtensionNames = extensions.data();

			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

			// Sets validation layers infos if enabled
			if (m_EnableValidationLayers)
			{
				createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
				createInfo.ppEnabledLayerNames = m_ValidationLayers.data();

				PopulateDebugMessengerCreateInfo(debugCreateInfo);
				createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
			}
			else
			{
				createInfo.enabledLayerCount = 0;
				createInfo.pNext = nullptr;
			}

			// Creates the instance
			VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to create Vulkan instance! Error code: %i", result);
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			m_BeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(m_Instance, "vkCmdBeginDebugUtilsLabelEXT");
			m_EndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(m_Instance, "vkCmdEndDebugUtilsLabelEXT");

			return RHIResult::IE_RHI_SUCCESS;
		}

		void VulkanDevice::DestroyInstance()
		{
			// Simply destroys the instance
			vkDestroyInstance(m_Instance, nullptr);
			m_Instance = VK_NULL_HANDLE;
		}

		RHIResult VulkanDevice::PickPhysicalDevice()
		{
			uint32_t deviceCount = 0;
			vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

			if (deviceCount == 0)
			{
				DEBUG_ERROR("Vulkan: Failed to find physical devices!");
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			std::vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

			for (const VkPhysicalDevice& device : devices)
			{
				if (IsPhysicalDeviceSuitable(device))
				{
					m_PhysicalDevice = device;

					m_MSAASamples = GetMaxUsableSampleCount(m_PhysicalDevice);

					VkPhysicalDeviceProperties deviceProperties;
					vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);

					DEBUG_LOG("Vulkan: Successfully picked physical device: %s", deviceProperties.deviceName);
					break;
				}
			}

			if (m_PhysicalDevice == VK_NULL_HANDLE)
			{
				DEBUG_ERROR("Vulkan: Failed to find a suitable physical device!");
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			return RHIResult::IE_RHI_SUCCESS;
		}

		bool VulkanDevice::IsPhysicalDeviceSuitable(VkPhysicalDevice _physicalDevice)
		{
			VkPhysicalDeviceProperties deviceProperties;
			VkPhysicalDeviceFeatures deviceFeatures;

			vkGetPhysicalDeviceProperties(_physicalDevice, &deviceProperties);
			vkGetPhysicalDeviceFeatures(_physicalDevice, &deviceFeatures);

			QueueFamilyIndices indices = FindQueueFamilies(_physicalDevice, m_Surface);

			bool extensionsSupported = CheckPhysicalDeviceExtensionSupport(_physicalDevice);
			bool swapChainAdequate = false;

			if (extensionsSupported)
			{
				SwapChainSupportDetails details = VulkanSwapChain::QuerySwapChainSupport(_physicalDevice, m_Surface);
				swapChainAdequate = !details.formats.empty() && !details.presentModes.empty();
			}

			return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
				&& deviceFeatures.geometryShader
				&& extensionsSupported
				&& swapChainAdequate
				&& indices.graphicsFamily.has_value()
				&& indices.presentFamily.has_value()
				&& deviceFeatures.samplerAnisotropy
				&& deviceFeatures.fillModeNonSolid;
		}

		bool VulkanDevice::CheckPhysicalDeviceExtensionSupport(VkPhysicalDevice _physicalDevice)
		{
			uint32_t extensionCount;
			vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &extensionCount, nullptr);

			std::vector<VkExtensionProperties> availableExtensions(extensionCount);
			vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &extensionCount, availableExtensions.data());

			std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

			for (const VkExtensionProperties& extension : availableExtensions)
			{
				requiredExtensions.erase(extension.extensionName);
			}

			return requiredExtensions.empty();
		}

		RHIResult VulkanDevice::CreateLogicalDevice()
		{
			QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice, m_Surface);

			float queuePriority = 1.f;

			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
			std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

			for (uint32_t queueFamily : uniqueQueueFamilies)
			{
				VkDeviceQueueCreateInfo queueCreateInfo{};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = queueFamily;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &queuePriority;
				queueCreateInfos.push_back(queueCreateInfo);
			}

			VkPhysicalDeviceFeatures deviceFeatures{};
			deviceFeatures.samplerAnisotropy = VK_TRUE;
			deviceFeatures.fillModeNonSolid = VK_TRUE;
			deviceFeatures.geometryShader = VK_TRUE;

			VkDeviceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
			createInfo.pQueueCreateInfos = queueCreateInfos.data();
			createInfo.pEnabledFeatures = &deviceFeatures;
			createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
			createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

			if (m_EnableValidationLayers)
			{
				createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
				createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
			}
			else
			{
				createInfo.enabledLayerCount = 0;
			}

			VkResult result = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to create logical device. Error code %i", result);
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			vkGetDeviceQueue(m_LogicalDevice, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
			vkGetDeviceQueue(m_LogicalDevice, indices.presentFamily.value(), 0, &m_PresentQueue);

			return RHIResult::IE_RHI_SUCCESS;
		}

		void VulkanDevice::DestroyLogicalDevice()
		{
			vkDestroyDevice(m_LogicalDevice, nullptr);
		}

		RHIResult VulkanDevice::CreateDebugMessenger()
		{
			if (!m_EnableValidationLayers)
				return RHIResult::IE_RHI_DEBUG_TOOLS_DEACTIVATED;

			VkDebugUtilsMessengerCreateInfoEXT createInfo;
			PopulateDebugMessengerCreateInfo(createInfo);

			VkResult result = CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to create debug messenger. Error code: %i", result);
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			return RHIResult::IE_RHI_SUCCESS;
		}

		void VulkanDevice::DestroyDebugMessenger()
		{
			if (!m_EnableValidationLayers)
				return;

			DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
		}

		RHIResult VulkanDevice::CreateSurface(IWindow* _window)
		{
			// TODO: Change this function to abstract it
			VkResult result = glfwCreateWindowSurface(m_Instance, _window->CastToGLFW()->GetRaw(), nullptr, &m_Surface);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to create window surface. Error code: %i", result);
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			return RHIResult::IE_RHI_SUCCESS;
		}

		void VulkanDevice::DestroySurface()
		{
			vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		}

		void VulkanDevice::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& _createInfo)
		{
			_createInfo = {};
			_createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			_createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			_createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			_createInfo.pfnUserCallback = ValidationLayersDebugCallback;
			_createInfo.pUserData = nullptr;
		}

		std::vector<const char*> VulkanDevice::GetRequiredExtensions(IWindow* _window)
		{
			uint32_t extensionsCount = 0;
			const char** glfwExtensions;

			// Gets the glfw extensions for Vulkan
			glfwExtensions = _window->GetRequiredExtensions(&extensionsCount);

			std::vector<const char*> extensions(glfwExtensions, glfwExtensions + extensionsCount);

			// Add the validation layer extension if enabled
			if (m_EnableValidationLayers)
			{
				extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			}

			return extensions;
		}

		bool VulkanDevice::CheckValidationLayersSupport()
		{
			uint32_t layerCount;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

			std::vector<VkLayerProperties> availableLayers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

			for (const char* layerName : m_ValidationLayers)
			{
				bool layerFound = false;

				for (const VkLayerProperties& layerProperties : availableLayers)
				{
					if (strcmp(layerName, layerProperties.layerName) == 0)
					{
						layerFound = true;
						break;
					}
				}

				if (!layerFound)
				{
					return false;
				}
			}

			return true;
		}

		VkSampleCountFlagBits VulkanDevice::GetMaxUsableSampleCount(VkPhysicalDevice _device)
		{
			VkPhysicalDeviceProperties physicalDeviceProperties;
			vkGetPhysicalDeviceProperties(_device, &physicalDeviceProperties);

			VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;

			if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
			if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
			if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
			if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
			if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
			if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

			return VK_SAMPLE_COUNT_1_BIT;
		}

		VkResult VulkanDevice::CreateDebugUtilsMessengerEXT(VkInstance _instance, const VkDebugUtilsMessengerCreateInfoEXT* _createInfo, const VkAllocationCallbacks* _allocator, VkDebugUtilsMessengerEXT* _debugMessenger)
		{
			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT");

			if (func != nullptr)
			{
				return func(_instance, _createInfo, _allocator, _debugMessenger);
			}
			else
			{
				return VK_ERROR_EXTENSION_NOT_PRESENT;
			}
		}

		void VulkanDevice::DestroyDebugUtilsMessengerEXT(VkInstance _instance, VkDebugUtilsMessengerEXT _debugMessenger, const VkAllocationCallbacks* _allocator)
		{
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkDestroyDebugUtilsMessengerEXT");

			if (func != nullptr)
			{
				func(_instance, _debugMessenger, _allocator);
			}
		}

		VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDevice::ValidationLayersDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT _messageSeverity, VkDebugUtilsMessageTypeFlagsEXT _messageType, const VkDebugUtilsMessengerCallbackDataEXT* _callBackData, void* _userData)
		{
			_messageType;
			_userData;

			switch (_messageSeverity)
			{
			default: case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				DEBUG_LOG("Vulkan Validation Layer: %s", _callBackData->pMessage);
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				DEBUG_WARN("Vulkan Validation Layer: %s", _callBackData->pMessage);
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
				DEBUG_ERROR("Vulkan Validation Layer: %s", _callBackData->pMessage);
				break;
			}

			return VK_FALSE;
		}
	}
}