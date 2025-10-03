#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"
#include "Core/Graphics/RHI/CommonTypes.h"

namespace Iris
{
	namespace Engine
	{
		class IDevice;
		class ICommandAllocator;
		class VulkanSampler;
		class IFramebuffer;

		class ISampler
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			virtual ~ISampler() = default;

			/// <summary>
			/// 
			/// </summary>w
			/// <returns></returns>
			IRIS_ENGINE_API virtual IE_FORCE_INLINE VulkanSampler* CastToVulkan() { return nullptr; }

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_Device"></param>
			/// <param name="_CommandAllocator"></param>
			/// <param name="_BufferType"></param>
			/// <param name="_BufferSize"></param>
			/// <param name="_Data"></param>
			/// <returns></returns>
			IRIS_ENGINE_API virtual RHIResult CreateSampler(IDevice* _device, unsigned int _mipLevels, const RHISampler& _samplerInfo) = 0;

			IRIS_ENGINE_API virtual RHIResult CreateSampler(IDevice* _device, IFramebuffer* _framebuffer, const RHISampler& _samplerInfo) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_Device"></param>
			/// <returns></returns>
			IRIS_ENGINE_API virtual RHIResult DestroySampler(IDevice* _device) = 0;

		};
	}
}