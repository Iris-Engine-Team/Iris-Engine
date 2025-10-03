#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"
#include "Core/Graphics/RHI/CommonTypes.h"

#include "Vector/Vector3.h"

#include <vector>

namespace Iris
{
	namespace Engine
	{
		class VulkanCommandBuffer;
		class IRenderPass;
		class ISwapChain;
		class IFramebuffer;
		class IPipeline;
		class IDevice;
		class IBuffer;
		class IDescriptor;

		class IRIS_ENGINE_API ICommandBuffer
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			virtual ~ICommandBuffer() = default;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			virtual IE_FORCE_INLINE VulkanCommandBuffer* CastToVulkan() { return nullptr; }

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_imageIndex"></param>
			/// <returns></returns>
			virtual RHIResult RecordCommandBuffer(unsigned int _imageIndex) = 0;
			
			virtual void BeginDebugLabel(IDevice* _device, std::string _name, Vector3 _color) = 0;
			virtual void EndDebugLabel(IDevice* _device) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_renderPass"></param>
			/// <param name="_framebuffer"></param>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			/// <param name="_clearValues"></param>
			virtual void StartRenderPass(IRenderPass* _renderPass, IFramebuffer* _framebuffer, unsigned int _width, unsigned int _height, std::vector<RHIClearColor>& _clearValues) = 0;
			
			/// <summary>
			/// 
			/// </summary>
			virtual void NextSubPass() = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_pipeline"></param>
			virtual void BindPipeline(IPipeline* _pipeline) = 0;
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			virtual void SetViewport(unsigned int _width, unsigned int _height, bool _invert) = 0;
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			virtual void SetScissor(unsigned int _width, unsigned int _height) = 0;
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_vertexBuffer"></param>
			virtual void BindVertexBuffer(IBuffer* _vertexBuffer) = 0;
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_indexBuffer"></param>
			virtual void BindIndexBuffer(IBuffer* _indexBuffer) = 0;
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_descriptor"></param>
			/// <param name="_pipeline"></param>
			/// <param name="_set"></param>
			virtual void BindDescriptor(IDescriptor* _descriptor, IPipeline* _pipeline, unsigned int _set) = 0;

			virtual void BindBlockVariable(IPipeline* _pipeline, size_t _size, void* _data, RHIShaderType _shaderBind) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_vertexCount"></param>
			virtual void Draw(unsigned int _vertexCount) = 0;
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_IndicesSize"></param>
			virtual void DrawIndexed(size_t _indicesSize) = 0;
			
			/// <summary>
			/// 
			/// </summary>
			virtual void EndRenderPass() = 0;
			
			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			virtual RHIResult EndCommandBuffer() = 0;
		};
	}
}