#pragma once

#include "Core/Graphics/RHI/ISampler.h"
#include "Core/Graphics/VulkanRHI/VulkanImage.h"

#include <vulkan/vulkan.h>

namespace Iris
{
	namespace Engine
	{
		class VulkanDevice;

		class VulkanSampler : public ISampler
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			~VulkanSampler() override final = default;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VulkanSampler* CastToVulkan() override final { return this; }

			IRIS_ENGINE_API IE_FORCE_INLINE VkSampler GetSampler() { return m_TextureSampler; }

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_Device"></param>
			/// <param name="_CommandAllocator"></param>
			/// <param name="_BufferType"></param>
			/// <param name="_BufferSize"></param>
			/// <param name="_Data"></param>
			/// <returns></returns>
			IRIS_ENGINE_API RHIResult CreateSampler(IDevice* _device, unsigned int _mipLevels, const RHISampler& _samplerInfo) override final;


			IRIS_ENGINE_API RHIResult CreateSampler(IDevice* _device, IFramebuffer* _framebuffer, const RHISampler& _samplerInfo) override;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_Device"></param>
			/// <returns></returns>
			IRIS_ENGINE_API RHIResult DestroySampler(IDevice* _device) override final;
			
		private:
			RHIResult CreateTextureSampler(VulkanDevice* _device, const RHISampler& _samplerInfo, unsigned int _mipLevels);

		private:
			VkSampler m_TextureSampler = VK_NULL_HANDLE;
		};
	}
}