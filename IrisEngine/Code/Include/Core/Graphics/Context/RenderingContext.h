#pragma once

#include "Core/Graphics/Context/IContext.h"
#include <vector>
#include <unordered_map>

namespace Iris
{
	namespace Engine
	{
		class IDevice;
		class ISwapChain;
		class IPipeline;
		class IRenderPass;
		class ICommandAllocator;
		class ICommandBuffer;
		class IDescriptorAllocator;
		class IDescriptor;
		class IBuffer;
		class IFramebuffer;
		class ISampler;
		class IImage;

		constexpr int IE_PIPELINE_TEMPLATE_SHIFT = 0;
		constexpr int IE_PIPELINE_TRANSPARENCY_SHIFT = 8;
		constexpr int IE_PIPELINE_DRAW_SHIFT = 16;

		constexpr int IE_PIPELINE_TEMPLATE_MASK = 0xFF << IE_PIPELINE_TEMPLATE_SHIFT;
		constexpr int IE_PIPELINE_TRANSPARENCY_MASK = 0xFF << IE_PIPELINE_TRANSPARENCY_SHIFT;
		constexpr int IE_PIPELINE_DRAW_MASK = 0xFF << IE_PIPELINE_DRAW_SHIFT;

		class RenderingContext : public IContext
		{
		public:

			IRIS_ENGINE_API RenderingContext() = default;

			/// <summary>
			/// Default RenderingContext's destructor
			/// </summary>
			IRIS_ENGINE_API ~RenderingContext() override final = default;

			/// <summary>
			/// SwapChain getter
			/// </summary>
			/// <returns>: A pointer to the SwapChain</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE ISwapChain* GetSwapChain() { return p_SwapChainReference; }

			/// <summary>
			/// Command allocator getter
			/// </summary>
			/// <returns>: A pointer to the command allocator of the rendering context</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE ICommandAllocator* GetCommandAllocator() { return m_CommandAllocator; }

			/// <summary>
			/// Command buffer getter
			/// </summary>
			/// <param name="_currentFrame">: Index of the frame in flight of rendering</param>
			/// <returns>: A pointer to a command buffer corresponding to the index</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE ICommandBuffer* GetCommandBuffer(unsigned int _currentFrame) { return m_CommandBuffers[_currentFrame]; }

			/// <summary>
			/// Initializes the rendering context variables
			/// </summary>
			/// <param name="_window">: Window of the application</param>
			/// <param name="_device">: Device of the application</param>
			/// <param name="_swapChain">: SwapChain of the application</param>
			/// <param name="_renderInterface">: Render interface of the application</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API RHIResult Initialize(IWindow* _window, IDevice* _device, ISwapChain* _swapChain, IRenderInterface* _renderInterface) override final;

			/// <summary>
			/// Terminates the rendering context variables
			/// </summary>
			/// <param name="_renderInterface">: Render interface of the application</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API RHIResult Terminate(IRenderInterface* _renderInterface) override final;

			/// <summary>
			/// Instantiates a renderpass with the appropriates parameters
			/// </summary>
			/// <param name="_renderpassInfo">: Renderpass parameters</param>
			/// <returns>A new renderpass</returns>
			IRIS_ENGINE_API IRenderPass* InstantiateRenderPass(const RHIRenderPass& _renderpassInfo);

			/// <summary>
			/// Destroys a renderpass
			/// </summary>
			/// <param name="_renderPass">: Renderpass to destroy</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API RHIResult DestroyRenderPass(IRenderPass* _renderPass);

			IRIS_ENGINE_API IFramebuffer* CreateFramebuffer(IRenderPass* _renderPass, unsigned int _width, unsigned int _height, const std::vector<IImage*>& _attachments);

			IRIS_ENGINE_API IFramebuffer* CreateFramebuffer(IRenderPass* _renderPass, unsigned int _width, unsigned int _height, const std::vector<RHIAttachment>& _attachments);

			/// <summary>
			/// Creates a list of framebuffers according to the number of frames in flight
			/// </summary>
			/// <param name="_renderPass">: Render pass that writes to the framebuffers</param>
			/// <param name="_width">: Width of the framebuffers</param>
			/// <param name="_height">: Height of the framebuffers</param>
			/// <param name="_attachments">: Attachments parameters of the framebuffers</param>
			/// <returns>A list of framebuffers</returns>
			IRIS_ENGINE_API std::vector<IFramebuffer*> CreateFramebuffers(IRenderPass* _renderPass, unsigned int _width, unsigned int _height, const std::vector<RHIAttachment>& _attachments);
			
			/// <summary>
			/// Destroys a list of framebuffers
			/// </summary>
			/// <param name="_framebuffers">: Framebuffers to destroy</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API RHIResult DestroyFramebuffers(std::vector<IFramebuffer*>& _framebuffers);

			/// <summary>
			/// Instantiates a pipeline with different parameters
			/// </summary>
			/// <param name="_renderPass">: Renderpass where the pipeline is used</param>
			/// <param name="_pipelineInfo">: Parameters of the pipeline</param>
			/// <returns>The created pipeline</returns>
			IRIS_ENGINE_API IPipeline* InstantiatePipeline(IRenderPass* _renderPass, const RHIPipeline& _pipelineInfo) const;
		
			/// <summary>
			/// Destroys a pipeline
			/// </summary>
			/// <param name="_pipeline">: Pipeline to destroy</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API RHIResult DestroyPipeline(IPipeline* _pipeline);
			
			IRIS_ENGINE_API RHIResult DestroyFramebuffer(IFramebuffer* _framebuffer);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_pipeline"></param>
			/// <param name="_id"></param>
			/// <returns></returns>
			IRIS_ENGINE_API IPipeline* AddToCache(IPipeline* _pipeline, uint32_t _id) const;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_id"></param>
			/// <returns></returns>
			bool ExistsInCache(uint32_t _id) const;

			bool ExistsInCache(IPipeline* _pipeline) const;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_id"></param>
			/// <returns></returns>
			IPipeline* GetFromCache(uint32_t _id) const;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_pipeline"></param>
			/// <param name="_id"></param>
			/// <returns></returns>
			IRIS_ENGINE_API bool RemoveFromCache(IPipeline* _pipeline);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_id"></param>
			/// <returns></returns>
			IRIS_ENGINE_API bool RemoveFromCache(uint32_t _id);

			RHIPipelineTemplate* AddTemplate(const RHIPipelinePass& _pipelinePass, const RHIPipelineTemplate& _template);

			RHIPipelineTemplate* GetTemplate(const RHIPipelinePass& _pipelinePass) const;

			bool RemoveTemplate(const RHIPipelinePass& _pipelinePass);

			private:
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_templateType"></param>
			/// <param name="_transparencyMode"></param>
			/// <param name="_drawMode"></param>
			/// <returns></returns>
			uint32_t EncodePipelineID(uint32_t _templateType, uint32_t _transparencyMode, uint32_t _drawMode) const;

		private:
			ICommandAllocator* m_CommandAllocator = nullptr;

			std::vector<ICommandBuffer*> m_CommandBuffers{};

			mutable std::unordered_map<uint32_t, IPipeline*> m_PipelineCache{};
			mutable std::unordered_map<RHIPipelinePass, RHIPipelineTemplate> m_PipelineTemplates{};
		};
	}
}