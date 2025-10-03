#include "Core/Graphics/VulkanRHI/VulkanCommandBuffer.h"

#include "Core/Graphics/VulkanRHI/VulkanRenderPass.h"
#include "Core/Graphics/VulkanRHI/VulkanPipeline.h"
#include "Core/Graphics/VulkanRHI/VulkanSwapChain.h"
#include "Core/Graphics/VulkanRHI/VulkanBuffer.h"
#include "Core/Graphics/VulkanRHI/VulkanDescriptor.h"
#include "Core/Graphics/VulkanRHI/VulkanFramebuffer.h"
#include "Core/Graphics/VulkanRHI/VulkanDevice.h"

namespace Iris
{
	namespace Engine
	{
		RHIResult VulkanCommandBuffer::RecordCommandBuffer(unsigned int _imageIndex)
		{
			_imageIndex;

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0; // From here optional
			beginInfo.pInheritanceInfo = nullptr;

			VkResult result = vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);

			if (result != VK_SUCCESS)
			{
				DEBUG_WARN("Failed to start command buffer. Error code %i", result);
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			return RHIResult::IE_RHI_SUCCESS;
		}

		void VulkanCommandBuffer::BeginDebugLabel(IDevice* _device, std::string _name, Vector3 _color)
		{
			(void)_device; (void)_name; (void)_color;

#ifdef NDEBUG
			(void)_name; (void)_color;
#else
			VkDebugUtilsLabelEXT labelInfo = {};
			labelInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
			labelInfo.pLabelName = _name.c_str();
			labelInfo.color[0] = _color[0];
			labelInfo.color[1] = _color[1];
			labelInfo.color[2] = _color[2];
			labelInfo.color[3] = 1.0f;

			PFN_vkCmdBeginDebugUtilsLabelEXT cmdSDL = _device->CastToVulkan()->GetBegingDebugLabel();

			if (cmdSDL)
				cmdSDL(m_CommandBuffer, &labelInfo);
#endif
		}

		void VulkanCommandBuffer::EndDebugLabel(IDevice* _device)
		{
			(void)_device;

#ifndef NDEBUG
			PFN_vkCmdEndDebugUtilsLabelEXT cmdEDL = _device->CastToVulkan()->GetEndDebugLabel();

			if (cmdEDL)
				cmdEDL(m_CommandBuffer);
#endif
		}

		void VulkanCommandBuffer::StartRenderPass(IRenderPass* _renderPass, IFramebuffer* _framebuffer, unsigned int _width, unsigned int _height, std::vector<RHIClearColor>& _clearValues)
		{
			VulkanRenderPass* vkRenderPass = _renderPass->CastToVulkan();
			VulkanFramebuffer* vkFramebuffer = _framebuffer->CastToVulkan();

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = vkRenderPass->GetRenderPass();
			renderPassInfo.framebuffer = vkFramebuffer->GetFramebuffer();
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = { _width, _height };

			std::vector<VkClearValue> clearValues(_clearValues.size());

			for (size_t i = 0; i < _clearValues.size(); ++i)
			{
				if (_clearValues[i].ColorDepthValue.GetX() != 1.f)
					clearValues[i].color = { {0.0f, 0.0f, 0.0f, 1.f} };
				else
					clearValues[i].depthStencil = { 1.f, static_cast<unsigned int>(1.f)};
			}

			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(m_CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		}

		void VulkanCommandBuffer::NextSubPass()
		{
			vkCmdNextSubpass(m_CommandBuffer, VK_SUBPASS_CONTENTS_INLINE);
		}

		void VulkanCommandBuffer::BindPipeline(IPipeline* _pipeline)
		{
			VulkanPipeline* vkPipeline = _pipeline->CastToVulkan();

			vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline->GetGraphicsPipeline());
		}

		void VulkanCommandBuffer::SetViewport(unsigned int _width, unsigned int _height, bool _invert)
		{
			VkViewport viewport{};
			viewport.x = 0.0f;

			if (_invert)
			{
				viewport.y = static_cast<float>(_height);
				viewport.width = static_cast<float>(_width);
				viewport.height = static_cast<float>(_height) * -1.f;
			}
			else
			{
				viewport.y = 0.f;
				viewport.width = static_cast<float>(_width);
				viewport.height = static_cast<float>(_height);
			}

			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);
		}

		void VulkanCommandBuffer::SetScissor(unsigned int _width, unsigned int _height)
		{
			VkExtent2D extent{};
			extent.width = static_cast<uint32_t>(_width);
			extent.height = static_cast<uint32_t>(_height);

			VkRect2D scissor{};
			scissor.offset = { 0, 0 };
			scissor.extent = extent;

			vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);
		}

		void VulkanCommandBuffer::BindVertexBuffer(IBuffer* _vertexBuffer)
		{
			VulkanBuffer* vkVertexBuffer = _vertexBuffer->CastToVulkan();

			VkBuffer vertexBuffers[] = { vkVertexBuffer->GetBuffer() };
			VkDeviceSize offsets[] = { 0 };

			vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, vertexBuffers, offsets);
		}

		void VulkanCommandBuffer::BindIndexBuffer(IBuffer* _indexBuffer)
		{
			VulkanBuffer* vkIndexBuffer = _indexBuffer->CastToVulkan();

			vkCmdBindIndexBuffer(m_CommandBuffer, vkIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}

		void VulkanCommandBuffer::BindDescriptor(IDescriptor* _descriptor, IPipeline* _pipeline, unsigned int _set)
		{
			VulkanDescriptor* vkDescriptor = _descriptor->CastToVulkan();
			VulkanPipeline* vkPipeline = _pipeline->CastToVulkan();
			
			VkDescriptorSet vkDescriptorSet = vkDescriptor->GetDescriptor();

			vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline->GetPipelineLayout(), _set, 1, &vkDescriptorSet, 0, nullptr);
		}

		void VulkanCommandBuffer::BindBlockVariable(IPipeline* _pipeline, size_t _size, void* _data, RHIShaderType _shaderBind)
		{
			VulkanPipeline* vkPipeline = _pipeline->CastToVulkan();

			VkShaderStageFlagBits stage{};

			switch (_shaderBind)
			{
			case Iris::Engine::RHIShaderType::IE_RHI_SHADER_TYPE_VERTEX: default:
				stage = VK_SHADER_STAGE_VERTEX_BIT;
				break;
			case Iris::Engine::RHIShaderType::IE_RHI_SHADER_TYPE_FRAGMENT:
				stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				break;
			case Iris::Engine::RHIShaderType::IE_RHI_SHADER_TYPE_GEOMETRY:
				stage = VK_SHADER_STAGE_GEOMETRY_BIT;
				break;
			}

			vkCmdPushConstants(m_CommandBuffer, vkPipeline->GetPipelineLayout(), stage, 0, static_cast<uint32_t>(_size), _data);
		}

		void VulkanCommandBuffer::Draw(unsigned int _vertexCount)
		{
			vkCmdDraw(m_CommandBuffer, _vertexCount, 1, 0, 0);
		}

		void VulkanCommandBuffer::DrawIndexed(size_t _indicesSize)
		{
			vkCmdDrawIndexed(m_CommandBuffer, static_cast<uint32_t>(_indicesSize), 1, 0, 0, 0);
		}

		void VulkanCommandBuffer::EndRenderPass()
		{
			vkCmdEndRenderPass(m_CommandBuffer);
		}

		RHIResult VulkanCommandBuffer::EndCommandBuffer()
		{
			VkResult result = vkEndCommandBuffer(m_CommandBuffer);

			if (result != VK_SUCCESS)
			{
				DEBUG_WARN("Failed to finish command buffer. Error code %i", result);
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			return RHIResult::IE_RHI_SUCCESS;
		}

	}
}