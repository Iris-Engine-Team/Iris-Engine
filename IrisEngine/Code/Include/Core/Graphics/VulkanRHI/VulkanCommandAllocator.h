#pragma once

#include "Core/Graphics/RHI/ICommandAllocator.h"
#include <vulkan/vulkan.h>

namespace Iris
{
	namespace Engine
	{
		class IRIS_ENGINE_API VulkanCommandAllocator : public ICommandAllocator
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			~VulkanCommandAllocator() override = default;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IE_FORCE_INLINE VulkanCommandAllocator* CastToVulkan() { return this; }

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <returns></returns>
			RHIResult Initialize(IDevice* _device) override;
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <returns></returns>
			RHIResult Terminate(IDevice* _device) override;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <returns></returns>
			ICommandBuffer* AllocateCommandBuffer(IDevice* _device) override;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <returns></returns>
			ICommandBuffer* BeginSingleTimeCommands(IDevice* _device) override;
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_commandBuffer"></param>
			/// <returns></returns>
			RHIResult EndSingleTimeCommands(IDevice* _device, ICommandBuffer* _commandBuffer) override;

		private:
			VkCommandPool m_CommandPool = VK_NULL_HANDLE;
		};
	}
}