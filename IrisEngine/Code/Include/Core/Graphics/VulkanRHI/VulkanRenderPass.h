#pragma once

#include "Core/Graphics/RHI/IRenderPass.h"

#include <vulkan/vulkan.h>

namespace Iris
{
	namespace Engine
	{
		class VulkanRenderPass : public IRenderPass
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			~VulkanRenderPass() override = default;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VulkanRenderPass* CastToVulkan() { return this; }

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IE_FORCE_INLINE VkRenderPass GetRenderPass() const { return m_RenderPass; }

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_swapChain"></param>
			/// <param name="_renderPassDescription"></param>
			/// <returns></returns>
			RHIResult Initialize(IDevice* _device, ISwapChain* _swapChain, const RHIRenderPass& _renderPassDescription) override;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_Device"></param>
			/// <returns></returns>
			RHIResult Terminate(IDevice* _device) override;

		private:
			VkRenderPass m_RenderPass = VK_NULL_HANDLE;
		};

	}
}