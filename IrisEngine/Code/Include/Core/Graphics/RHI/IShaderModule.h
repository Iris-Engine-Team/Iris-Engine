#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"
#include "Core/Graphics/RHI/CommonTypes.h"

#include <vector>

namespace Iris
{
	namespace Engine
	{
		class IDevice;
		class ICommandAllocator;
		class VulkanShaderModule;
		class IFramebuffer;

		class IShaderModule
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			virtual ~IShaderModule() = default;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API virtual IE_FORCE_INLINE VulkanShaderModule* CastToVulkan() { return nullptr; }

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_Device"></param>
			/// <param name="_CommandAllocator"></param>
			/// <param name="_BufferType"></param>
			/// <param name="_BufferSize"></param>
			/// <param name="_Data"></param>
			/// <returns></returns>
			IRIS_ENGINE_API virtual RHIResult CreateShaderModule(IDevice* _device, std::vector<uint32_t>& _shaderCode) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_Device"></param>
			/// <returns></returns>
			IRIS_ENGINE_API virtual RHIResult DestroyShaderModule(IDevice* _device) = 0;

		};
	}
}