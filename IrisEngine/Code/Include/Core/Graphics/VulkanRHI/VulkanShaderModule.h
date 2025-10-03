#pragma once

#include "Core/Graphics/RHI/IShaderModule.h"

#include <vulkan/vulkan.h>

namespace Iris
{
	namespace Engine
	{
		class VulkanShaderModule : public IShaderModule
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			~VulkanShaderModule() override final = default;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VulkanShaderModule* CastToVulkan() override final { return this; }

			IRIS_ENGINE_API IE_FORCE_INLINE VkShaderModule GetRawType() { return m_ShaderModule; }

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_Device"></param>
			/// <param name="_CommandAllocator"></param>
			/// <param name="_BufferType"></param>
			/// <param name="_BufferSize"></param>
			/// <param name="_Data"></param>
			/// <returns></returns>
			IRIS_ENGINE_API RHIResult CreateShaderModule(IDevice* _device, std::vector<uint32_t>& _shaderCode) override final;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_Device"></param>
			/// <returns></returns>
			IRIS_ENGINE_API RHIResult DestroyShaderModule(IDevice* _device) override final;

		private:

			VkShaderModule m_ShaderModule = VK_NULL_HANDLE;
		};
	}
}