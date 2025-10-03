#pragma once

#include "Core/Graphics/RHI/IPipeline.h"

#include <vulkan/vulkan.h>

namespace Iris
{
	namespace Engine
	{
		struct ShaderLayout
		{
			std::vector<VkDescriptorSetLayout> DescriptorLayout{};
			std::vector<VkPushConstantRange> PushConstantRange{};
		};

		class VulkanPipeline : public IPipeline
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

			/// <summary>
			/// 
			/// </summary>
			IRIS_ENGINE_API ~VulkanPipeline() override = default;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VulkanPipeline* CastToVulkan() override { return this; }

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VkPipeline GetGraphicsPipeline() const { return m_GraphicsPipeline; }

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_swapChain"></param>
			/// <param name="_renderPass"></param>
			/// <param name="_shaderInfo"></param>
			/// <param name="_faceFormat"></param>
			/// <param name="_depthWrite"></param>
			/// <param name="_sampleCount"></param>
			/// <returns></returns>
			IRIS_ENGINE_API RHIResult Initialize(IDevice* _device, IRenderPass* _renderPass, const RHIPipeline& _pipelineInfo) override;
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <returns></returns>
			IRIS_ENGINE_API RHIResult Terminate(IDevice* _device) override;

		private:
			ShaderLayout CreateShaderDescriptorLayouts(IDevice* _device, RHIShaderInfo _shader);

			VkPipelineColorBlendAttachmentState CreateBlendAttachmentNone();
			VkPipelineColorBlendAttachmentState CreateBlendAttachmentAlphaBlend();
			VkPipelineColorBlendAttachmentState CreateBlendAttachmentAdditive();
			//VkPipelineColorBlendAttachmentState CreateBlendAttachmentMultiply();

		private:
			VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;
		};

	}
}