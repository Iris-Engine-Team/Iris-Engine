#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"
#include "Core/Graphics/RHI/CommonTypes.h"

namespace Iris
{
	namespace Engine
	{
		class IWindow;
		class VulkanDevice;

		class IDevice
		{
		public:
			/// <summary>
			/// Default IDevice's destructor
			/// </summary>
			virtual ~IDevice() = default;

			/// <summary>
			/// Casts an IDevice into a Vulkan device
			/// </summary>
			/// <returns>The Vulkan device or nullptr</returns>
			virtual VulkanDevice* CastToVulkan() { return nullptr; }

			/// <summary>
			/// Initializes the device
			/// </summary>
			/// <param name="_window">: Window of the application</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			virtual RHIResult Initialize(IWindow* _window) = 0;
			
			/// <summary>
			/// Terminates the device
			/// </summary>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			virtual RHIResult Terminate() = 0;

			/// <summary>
			/// Waits the device idle state
			/// </summary>
			virtual void WaitIdle() = 0;

			virtual void WaitGraphicsQueueIdle() = 0;
		};
	}
}