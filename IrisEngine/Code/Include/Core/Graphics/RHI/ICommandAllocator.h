#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"
#include "Core/Graphics/RHI/CommonTypes.h"

namespace Iris
{
	namespace Engine
	{
		class VulkanCommandAllocator;
		class IDevice;
		class ICommandBuffer;

		class IRIS_ENGINE_API ICommandAllocator
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			virtual ~ICommandAllocator() = default;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			virtual IE_FORCE_INLINE VulkanCommandAllocator* CastToVulkan() { return nullptr; }

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <returns></returns>
			virtual RHIResult Initialize(IDevice* _device) = 0;
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <returns></returns>
			virtual RHIResult Terminate(IDevice* _device) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <returns></returns>
			virtual ICommandBuffer* AllocateCommandBuffer(IDevice* _device) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <returns></returns>
			virtual ICommandBuffer* BeginSingleTimeCommands(IDevice* _device) = 0;
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_commandBuffer"></param>
			/// <returns></returns>
			virtual RHIResult EndSingleTimeCommands(IDevice* _device, ICommandBuffer* _commandBuffer) = 0;
		};
	}
}