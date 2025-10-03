#pragma once

#include "Core/Graphics/RHI/ICommandBuffer.h"
#include <vulkan/vulkan.h>

namespace Iris
{
	namespace Engine
	{
		class IRIS_ENGINE_API VulkanCommandBuffer : public ICommandBuffer
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			~VulkanCommandBuffer() override = default;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IE_FORCE_INLINE VulkanCommandBuffer* CastToVulkan() { return this; }

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IE_FORCE_INLINE VkCommandBuffer GetCommandBuffer() { return m_CommandBuffer; }

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IE_FORCE_INLINE void SetCommandBuffer(VkCommandBuffer _commandBuffer) { m_CommandBuffer = _commandBuffer; }

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_imageIndex"></param>
			/// <returns></returns>
			RHIResult RecordCommandBuffer(unsigned int _imageIndex);

			void BeginDebugLabel(IDevice* _device, std::string _name, Vector3 _color) override final;
			void EndDebugLabel(IDevice* _device) override final;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_renderPass"></param>
			/// <param name="_framebuffer"></param>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			/// <param name="_clearValues"></param>
			void StartRenderPass(IRenderPass* _renderPass, IFramebuffer* _framebuffer, unsigned int _width, unsigned int _height, std::vector<RHIClearColor>& _clearValues);

			void NextSubPass() override;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_pipeline"></param>
			void BindPipeline(IPipeline* _pipeline);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			void SetViewport(unsigned int _width, unsigned int _height, bool _invert);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			void SetScissor(unsigned int _width, unsigned int _height);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_vertexBuffer"></param>
			void BindVertexBuffer(IBuffer* _vertexBuffer);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_indexBuffer"></param>
			void BindIndexBuffer(IBuffer* _indexBuffer);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_descriptor"></param>
			/// <param name="_pipeline"></param>
			/// <param name="_set"></param>
			void BindDescriptor(IDescriptor* _descriptor, IPipeline* _pipeline, unsigned int _set) override;

			void BindBlockVariable(IPipeline* _pipeline, size_t _size, void* _data, RHIShaderType _shaderBind) override;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_vertexCount"></param>
			void Draw(unsigned int _vertexCount);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_IndicesSize"></param>
			void DrawIndexed(size_t _IndicesSize);

			/// <summary>
			/// 
			/// </summary>
			void EndRenderPass() override;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			RHIResult EndCommandBuffer() override;

		private:
			VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
		};
	}
}