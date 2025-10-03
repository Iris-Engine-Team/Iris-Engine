#include "Core/Graphics/Context/RenderingContext.h"

#include "Core/Graphics/RHI/IRenderInterface.h"
#include "Core/Graphics/RHI/IDevice.h"
#include "Core/Graphics/RHI/ISwapChain.h"
#include "Core/Graphics/RHI/IPipeline.h"
#include "Core/Graphics/RHI/IRenderPass.h"
#include "Core/Graphics/RHI/ICommandAllocator.h"
#include "Core/Graphics/RHI/ICommandBuffer.h"
#include "Core/Graphics/RHI/IDescriptorAllocator.h"
#include "Core/Graphics/RHI/IDescriptor.h"
#include "Core/Graphics/RHI/IBuffer.h"
#include "Core/Graphics/RHI/IFramebuffer.h"
#include "Core/Graphics/VulkanRHI/VulkanRenderPass.h"

#include "Core/Tools/FilePath.h"

#include "Resources/ResourceManager.h"

#include <vector>

namespace Iris
{
	namespace Engine
	{
		RHIResult RenderingContext::Initialize(IWindow* _window, IDevice* _device, ISwapChain* _swapChain, IRenderInterface* _renderInterface)
		{
			IContext::Initialize(_window, _device, _swapChain, _renderInterface);
			
			m_CommandAllocator = _renderInterface->InstantiateCommandAllocator(p_DeviceReference);

			m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
			{
				m_CommandBuffers[i] = m_CommandAllocator->AllocateCommandBuffer(p_DeviceReference);
			}

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult RenderingContext::Terminate(IRenderInterface* _renderInterface)
		{
			if (!_renderInterface)
			{
				// TODO: ERR
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			p_DeviceReference->WaitIdle();

			std::vector<IPipeline*> caches;
			for (auto& pipelinePair : m_PipelineCache)
			{
				caches.push_back(pipelinePair.second);
			}

			size_t size = caches.size();
			for (size_t i = 0; i < size; ++i)
				DestroyPipeline(caches[i]);

			_renderInterface->DestroySwapChain(p_SwapChainReference, p_DeviceReference);

			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
			{
				delete m_CommandBuffers[i];
			}

			_renderInterface->DestroyCommandAllocator(m_CommandAllocator, p_DeviceReference);

			return RHIResult::IE_RHI_SUCCESS;
		}

		IRenderPass* RenderingContext::InstantiateRenderPass(const RHIRenderPass& _renderpassInfo)
		{
			return p_RenderInterfaceReference->InstantiateRenderPass(p_DeviceReference, p_SwapChainReference, _renderpassInfo);
		}

		RHIResult RenderingContext::DestroyRenderPass(IRenderPass* _renderPass)
		{
			return p_RenderInterfaceReference->DestroyRenderPass(_renderPass, p_DeviceReference);
		}

		IFramebuffer* RenderingContext::CreateFramebuffer(IRenderPass* _renderPass, unsigned int _width, unsigned int _height, const std::vector<IImage*>& _attachments)
		{
			return p_RenderInterfaceReference->CreateFramebuffer(p_DeviceReference, _renderPass, _width, _height, _attachments);
		}

		IFramebuffer* RenderingContext::CreateFramebuffer(IRenderPass* _renderPass, unsigned int _width, unsigned int _height, const std::vector<RHIAttachment>& _attachments)
		{
			return p_RenderInterfaceReference->CreateFramebuffer(p_DeviceReference, _renderPass, _width, _height, _attachments);
		}

		std::vector<IFramebuffer*> RenderingContext::CreateFramebuffers(IRenderPass* _renderPass, unsigned int _width, unsigned int _height, const std::vector<RHIAttachment>& _attachments)
		{
			std::vector<IFramebuffer*> framebuffers(MAX_FRAMES_IN_FLIGHT);

			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
			{
				framebuffers[i] = p_RenderInterfaceReference->CreateFramebuffer(p_DeviceReference, _renderPass, _width, _height, _attachments);
			}

			return framebuffers;
		}

		IPipeline* RenderingContext::InstantiatePipeline(IRenderPass* _renderPass, const RHIPipeline& _pipelineInfo) const
		{
			uint32_t id = 0;

			if (!_pipelineInfo.Template.BackendTemplate)
			{
				id = EncodePipelineID(
					static_cast<uint32_t>(_pipelineInfo.Template.PassType),
					static_cast<uint32_t>(_pipelineInfo.BlendType),
					static_cast<uint32_t>(_pipelineInfo.FaceCulling)
				);

				if (ExistsInCache(id))
				{
					return GetFromCache(id);
				}
			}

			IPipeline* pipeline = p_RenderInterfaceReference->InstantiatePipeline(p_DeviceReference, _renderPass, _pipelineInfo);

			if (!_pipelineInfo.Template.BackendTemplate)
			{
				pipeline = AddToCache(pipeline, id);
			}

			return pipeline;
		}

		RHIResult RenderingContext::DestroyPipeline(IPipeline* _pipeline)
		{
			if (ExistsInCache(_pipeline))
			{
				RemoveFromCache(_pipeline);
			}

			return p_RenderInterfaceReference->DestroyPipeline(_pipeline, p_DeviceReference);
		}

		RHIResult RenderingContext::DestroyFramebuffer(IFramebuffer* _framebuffer)
		{
			p_RenderInterfaceReference->DestroyFrameBuffer(_framebuffer, p_DeviceReference);

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult RenderingContext::DestroyFramebuffers(std::vector<IFramebuffer*>& _framebuffers)
		{
			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
			{
				p_RenderInterfaceReference->DestroyFrameBuffer(_framebuffers[i], p_DeviceReference);
			}

			return RHIResult::IE_RHI_SUCCESS;
		}

		IPipeline* RenderingContext::AddToCache(IPipeline* _pipeline, uint32_t _id) const
		{
			if (ExistsInCache(_id))
			{
				DEBUG_WARN("A pipeline already exists at the position of the cache, returning the current value.")
				return m_PipelineCache[_id];
			}

			m_PipelineCache.try_emplace(_id, _pipeline);

			return _pipeline;
		}

		bool RenderingContext::ExistsInCache(uint32_t _id) const
		{
			return m_PipelineCache.find(_id) == m_PipelineCache.end() ? false : true;
		}

		bool RenderingContext::ExistsInCache(IPipeline* _pipeline) const
		{
			for (const auto& [key, value] : m_PipelineCache)
			{
				if (value == _pipeline)
					return true;
			}

			return false;
		}

		IPipeline* RenderingContext::GetFromCache(uint32_t _id) const
		{
			return m_PipelineCache[_id];
		}

		bool RenderingContext::RemoveFromCache(IPipeline* _pipeline)
		{
			for (auto& pipeline : m_PipelineCache)
			{
				if (pipeline.second != _pipeline)
					continue;

				m_PipelineCache.erase(pipeline.first);

				return true;
			}

			return false;
		}

		bool RenderingContext::RemoveFromCache(uint32_t _id)
		{
			m_PipelineCache.erase(_id);
			return true;
		}

		RHIPipelineTemplate* RenderingContext::AddTemplate(const RHIPipelinePass& _pipelinePass, const RHIPipelineTemplate& _template)
		{
			auto it = m_PipelineTemplates.find(_pipelinePass);

			if (it != m_PipelineTemplates.end())
			{
				DEBUG_WARN("A pipeline template already exists at the specified position, returning the current value.")
					return &it->second;
			}

			m_PipelineTemplates.try_emplace(_pipelinePass, _template);

			return &m_PipelineTemplates[_pipelinePass];
		}

		RHIPipelineTemplate* RenderingContext::GetTemplate(const RHIPipelinePass& _pipelinePass) const
		{
			return &m_PipelineTemplates[_pipelinePass];
		}

		bool RenderingContext::RemoveTemplate(const RHIPipelinePass& _pipelinePass)
		{
			m_PipelineTemplates.erase(_pipelinePass);
			return true;
		}

		uint32_t RenderingContext::EncodePipelineID(uint32_t _templateType, uint32_t _transparencyMode, uint32_t _drawMode) const
		{
			return	(_templateType		<<	IE_PIPELINE_TEMPLATE_SHIFT)
				|	(_transparencyMode	<<	IE_PIPELINE_TRANSPARENCY_SHIFT)
				|	(_drawMode			<<	IE_PIPELINE_DRAW_SHIFT);
		}
	}
}