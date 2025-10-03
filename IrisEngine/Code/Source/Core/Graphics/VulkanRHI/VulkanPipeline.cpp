#include "Core/Graphics/VulkanRHI/VulkanPipeline.h"

#include "Core/Graphics/VulkanRHI/VulkanDevice.h"
#include "Core/Graphics/VulkanRHI/VulkanSwapChain.h"
#include "Core/Graphics/VulkanRHI/VulkanRenderPass.h"
#include "Core/Graphics/VulkanRHI/VulkanRenderInterface.h"
#include "Core/Graphics/VulkanRHI/VulkanDescriptorLayout.h"
#include "Core/Graphics/VulkanRHI/VulkanShaderModule.h"
#include "Resources/Shader.h"

#include "Core/Graphics/RHI/Vertex.h"

#include <array>

#include "Loader/ShaderReflection/SPIRVShaderInspector.h"

namespace Iris
{
	namespace Engine
	{
		RHIResult VulkanPipeline::Initialize(IDevice* _device, IRenderPass* _renderPass, const RHIPipeline& _pipelineInfo)
		{
			IPipeline::Initialize(_device, _renderPass, _pipelineInfo);

			VulkanDevice* vkDevice = _device->CastToVulkan();
			VulkanRenderPass* vkRenderPass = _renderPass->CastToVulkan();

			std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

			size_t shaderCount = _pipelineInfo.Template.Shaders.size();

			std::vector<VkShaderModule> shaderModules{};
			shaderModules.resize(shaderCount);
	
			for (size_t i = 0; i < shaderCount; ++i)
			{
				const RHIShaderInfo& info = _pipelineInfo.Template.Shaders[i];

				VkPipelineShaderStageCreateInfo shaderStageInfo{};
				shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				switch (info.Shader->GetType())
				{
				case RHIShaderType::IE_RHI_SHADER_TYPE_VERTEX: default:
					shaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
					break;
				case RHIShaderType::IE_RHI_SHADER_TYPE_GEOMETRY:
					shaderStageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
					break;
				case RHIShaderType::IE_RHI_SHADER_TYPE_FRAGMENT:
					shaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
					break;
				}

				std::vector<uint32_t> sourceCode = info.Shader->GetShaderCode();
				IShaderModule* shaderModule = info.Shader->GetShaderModule();
				VulkanShaderModule* vkSM = shaderModule->CastToVulkan();

				VkShaderModuleCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.codeSize = sourceCode.size() * sizeof(uint32_t);
				createInfo.pCode = sourceCode.data();
				shaderStageInfo.module = vkSM->GetRawType();
				shaderStageInfo.pName = info.EntryFunction.c_str();

				shaderStages.emplace_back(shaderStageInfo);
			}

			std::vector<VkDynamicState> dynamicStates = {
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR
			};

			VkPipelineDynamicStateCreateInfo dynamicState{};
			dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
			dynamicState.pDynamicStates = dynamicStates.data();

			////////////////////////////////////////////////////////////////////////////////// Code for binding descriptions

			// TODO: MOVE THIS

			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			// TODO: MOVE THIS

			std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions{};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, Vertex::Position);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, Vertex::Normal);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, Vertex::TextCoord);

			attributeDescriptions[3].binding = 0;
			attributeDescriptions[3].location = 3;
			attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[3].offset = offsetof(Vertex, Vertex::Tangent);

			attributeDescriptions[4].binding = 0;
			attributeDescriptions[4].location = 4;
			attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[4].offset = offsetof(Vertex, Vertex::BiTangent);

			//////////////////////////////////////////////////////////////////////////////////

			VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
			if (_pipelineInfo.ContainsInput)
			{
				vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
				vertexInputInfo.vertexBindingDescriptionCount = 1;
				vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
				vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
				vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
			}
			else
			{
				vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
				vertexInputInfo.vertexBindingDescriptionCount = 0;
				vertexInputInfo.pVertexBindingDescriptions = nullptr;
				vertexInputInfo.vertexAttributeDescriptionCount = 0;
				vertexInputInfo.pVertexAttributeDescriptions = nullptr;
			}

			VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
			inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssembly.primitiveRestartEnable = VK_FALSE;

			// Default Values
			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = static_cast<float>(1920);
			viewport.height = static_cast<float>(1080);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			VkExtent2D extent{};
			extent.width = 1920;
			extent.height = 1080;

			VkRect2D scissor{};
			scissor.offset = { 0, 0 };
			scissor.extent = extent;

			VkPipelineViewportStateCreateInfo viewportState{};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.viewportCount = 1;
			viewportState.scissorCount = 1;

			VkPipelineRasterizationStateCreateInfo rasterizer{};
			rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizer.depthClampEnable = VK_FALSE;
			rasterizer.rasterizerDiscardEnable = VK_FALSE;
			rasterizer.polygonMode = VulkanRenderInterface::ConvertPolygonMode(_pipelineInfo.PolygonMode);
			rasterizer.lineWidth = 1.0f;
			rasterizer.cullMode = VulkanRenderInterface::ConvertFaceCulling(_pipelineInfo.FaceCulling);
			rasterizer.frontFace = VulkanRenderInterface::ConvertFrontFace(_pipelineInfo.FrontFace);
			rasterizer.depthBiasEnable = VK_FALSE;
			rasterizer.depthBiasConstantFactor = 0.0f;
			rasterizer.depthBiasClamp = 0.0f;
			rasterizer.depthBiasSlopeFactor = 0.0f;

			VkPipelineMultisampleStateCreateInfo multisampling{};
			multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling.sampleShadingEnable = VK_FALSE;
			multisampling.rasterizationSamples = VulkanRenderInterface::ConvertSampleCount(_pipelineInfo.SampleCount);
			multisampling.minSampleShading = 1.0f;
			multisampling.pSampleMask = nullptr;
			multisampling.alphaToCoverageEnable = VK_FALSE;
			multisampling.alphaToOneEnable = VK_FALSE;

			VkPipelineDepthStencilStateCreateInfo depthStencil{};
			depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencil.depthTestEnable = static_cast<VkBool32>(_pipelineInfo.EnableDepthTest);
			depthStencil.depthWriteEnable = static_cast<VkBool32>(_pipelineInfo.EnableDepthWrite);
			depthStencil.depthCompareOp = VulkanRenderInterface::ConvertCompareOperation(_pipelineInfo.CompareOperation);
			depthStencil.depthBoundsTestEnable = VK_FALSE;
			depthStencil.minDepthBounds = 0.0f;
			depthStencil.maxDepthBounds = 1.0f;
			depthStencil.stencilTestEnable = VK_FALSE;
			depthStencil.front = {};
			depthStencil.back = {};

			VkPipelineColorBlendAttachmentState colorBlendAttachment{};

			switch (_pipelineInfo.BlendType)
			{
			case RHIBlendType::IE_RHI_BLEND_TYPE_NONE: default:
				colorBlendAttachment = CreateBlendAttachmentNone();
				break;
			case RHIBlendType::IE_RHI_BLEND_TYPE_ALPHA_BLEND:
				colorBlendAttachment = CreateBlendAttachmentAlphaBlend();
				break;
			case RHIBlendType::IE_RHI_BLEND_TYPE_ADDITIVE:
				colorBlendAttachment = CreateBlendAttachmentAdditive();
				break;
			}

			std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments(_pipelineInfo.AttachmentCount, colorBlendAttachment);

			VkPipelineColorBlendStateCreateInfo colorBlending{};
			colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlending.logicOpEnable = VK_FALSE;
			colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
			colorBlending.attachmentCount = static_cast<uint32_t>(_pipelineInfo.AttachmentCount);
			colorBlending.pAttachments = colorBlendAttachments.data();

			////////////////////////////////////////////////////////////////////////////////// Descriptors binding descriptions

			std::vector<VkDescriptorSetLayout> descriptorLayouts{};
			std::vector<VkPushConstantRange> pushConstantRange{};

			for (size_t i = 0; i < shaderCount; ++i)
			{
				const RHIShaderInfo& info = _pipelineInfo.Template.Shaders[i];

				ShaderLayout dResult = CreateShaderDescriptorLayouts(vkDevice, info);
				descriptorLayouts.insert(descriptorLayouts.end(), dResult.DescriptorLayout.begin(), dResult.DescriptorLayout.end());
				pushConstantRange.insert(pushConstantRange.end(), dResult.PushConstantRange.begin(), dResult.PushConstantRange.end());
			}

			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorLayouts.size());
			pipelineLayoutInfo.pSetLayouts = descriptorLayouts.data();
			pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRange.size());
			pipelineLayoutInfo.pPushConstantRanges = pushConstantRange.data();

			VkResult result = vkCreatePipelineLayout(vkDevice->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to pipeline layout, error code: %i", result);
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			VkGraphicsPipelineCreateInfo pipelineInfo{};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
			pipelineInfo.pStages = shaderStages.data();
			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &inputAssembly;
			pipelineInfo.pViewportState = &viewportState;
			pipelineInfo.pRasterizationState = &rasterizer;
			pipelineInfo.pMultisampleState = &multisampling;
			pipelineInfo.pDepthStencilState = &depthStencil;
			pipelineInfo.pColorBlendState = &colorBlending;
			pipelineInfo.pDynamicState = &dynamicState;
			pipelineInfo.layout = m_PipelineLayout;
			pipelineInfo.renderPass = vkRenderPass->GetRenderPass();
			pipelineInfo.subpass = _pipelineInfo.Subpass;
			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // From here optional
			pipelineInfo.basePipelineIndex = -1;

			result = vkCreateGraphicsPipelines(vkDevice->GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to create graphics pipeline, error code: %i", result);
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			for (size_t k = 0; k < shaderModules.size(); ++k)
			{
				vkDestroyShaderModule(vkDevice->GetLogicalDevice(), shaderModules[k], nullptr);
			}

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanPipeline::Terminate(IDevice* _device)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			for (uint32_t i = 0; i < p_DescriptorLayouts.size(); ++i)
			{
				vkDestroyDescriptorSetLayout(vkDevice->GetLogicalDevice(), p_DescriptorLayouts[i]->CastToVulkan()->GetDescriptorLayout(), nullptr);
				delete p_DescriptorLayouts[i];
			}

			vkDestroyPipeline(vkDevice->GetLogicalDevice(), m_GraphicsPipeline, nullptr);
			vkDestroyPipelineLayout(vkDevice->GetLogicalDevice(), m_PipelineLayout, nullptr);

			return RHIResult::IE_RHI_SUCCESS;
		}

		ShaderLayout VulkanPipeline::CreateShaderDescriptorLayouts(IDevice* _device, RHIShaderInfo _shader)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			SPIRVShaderInspector inspector{};

			InspectedShader reflectedShader{};

			std::vector<uint32_t> code = _shader.Shader->GetShaderCode();

			inspector.InspectShader(code, RHIType::IE_RHI_TYPE_VULKAN, _shader.Shader->GetType(), &reflectedShader);

			ShaderLayout layoutResult{};

			// Convert to abstract reflect p_Type to vulkan
			// TODO: Rework
			for (const InspectedDescriptor& descriptor : reflectedShader.Descriptors)
			{
				std::vector<VkDescriptorSetLayoutBinding> layoutBindings{};

				for (const InspectedDescriptorBinding& binding : descriptor.DescriptorBindings)
				{
					VkDescriptorSetLayoutBinding layoutBinding = {};
					layoutBinding.binding = binding.Binding;
					layoutBinding.descriptorType = static_cast<VkDescriptorType>(binding.Type);
					layoutBinding.descriptorCount = binding.DescriptorCount;

					switch (_shader.Shader->GetType())
					{
					default: case RHIShaderType::IE_RHI_SHADER_TYPE_VERTEX:
						layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
						break;
					case RHIShaderType::IE_RHI_SHADER_TYPE_GEOMETRY:
						layoutBinding.stageFlags = VK_SHADER_STAGE_GEOMETRY_BIT;
						break;
					case RHIShaderType::IE_RHI_SHADER_TYPE_FRAGMENT:
						layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
						break;
					}

					layoutBinding.pImmutableSamplers = nullptr;

					layoutBindings.emplace_back(layoutBinding);
				}

				VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
				layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
				layoutCreateInfo.pBindings = layoutBindings.data();

				VkDescriptorSetLayout newLayout;

				VkResult result = vkCreateDescriptorSetLayout(vkDevice->GetLogicalDevice(), &layoutCreateInfo, nullptr, &newLayout);

				layoutBindings.clear();

				if (result != VK_SUCCESS)
				{
					DEBUG_ERROR("Vulkan: Failed to create descriptor set layout, error code %i", result);
				}

				IDescriptorLayout* dl = new VulkanDescriptorLayout();
				dl->CastToVulkan()->SetDescriptorLayout(newLayout);

				p_DescriptorLayouts.emplace_back(dl);

				layoutResult.DescriptorLayout.push_back(newLayout);
			}

			std::vector<VkPushConstantRange> layoutPushConstants{};

			for (const InspectedBlockVariable& pushConstant : reflectedShader.BlockVariable)
			{
				VkPushConstantRange newPushConstant{};

				newPushConstant.offset = pushConstant.Offset;
				newPushConstant.size = pushConstant.Size;

				switch (_shader.Shader->GetType())
				{
				default: case RHIShaderType::IE_RHI_SHADER_TYPE_VERTEX:
					newPushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
					break;
				case RHIShaderType::IE_RHI_SHADER_TYPE_FRAGMENT:
					newPushConstant.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
					break;
				}

				layoutResult.PushConstantRange.push_back(newPushConstant);
			}

			return layoutResult;
		}

		VkPipelineColorBlendAttachmentState VulkanPipeline::CreateBlendAttachmentNone()
		{
			VkPipelineColorBlendAttachmentState colorBlendAttachment{};

			colorBlendAttachment.blendEnable = VK_FALSE;
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;

			return colorBlendAttachment;
		}

		VkPipelineColorBlendAttachmentState VulkanPipeline::CreateBlendAttachmentAlphaBlend()
		{
			VkPipelineColorBlendAttachmentState colorBlendAttachment{};

			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;
			colorBlendAttachment.blendEnable = VK_TRUE;
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

			return colorBlendAttachment;
		}

		VkPipelineColorBlendAttachmentState VulkanPipeline::CreateBlendAttachmentAdditive()
		{
			VkPipelineColorBlendAttachmentState colorBlendAttachment{};

			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;
			colorBlendAttachment.blendEnable = VK_TRUE;
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

			return colorBlendAttachment;
		}

		//VkPipelineColorBlendAttachmentState VulkanPipeline::CreateBlendAttachmentMultiply()
		//{
		//	VkPipelineColorBlendAttachmentState colorBlendAttachment{};

		//	colorBlendAttachment.blendEnable = VK_TRUE;
		//	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		//	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
		//	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_MULTIPLY_EXT; // Extension required
		//	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		//	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		//	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_MULTIPLY_EXT;

		//	return colorBlendAttachment;
		//}
	}

}