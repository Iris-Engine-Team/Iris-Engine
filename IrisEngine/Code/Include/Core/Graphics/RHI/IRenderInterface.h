#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"
#include "Core/Graphics/RHI/CommonTypes.h"

#include <vector>

namespace Iris
{
	namespace Engine
	{
		class IWindow;
		class IDevice;
		class ISwapChain;
		class IRenderPass;
		class IPipeline;
		class ICommandAllocator;
		class IDescriptorAllocator;
		class IFramebuffer;
		class IBuffer;
		class IShaderModule;
		class ISampler;
		class IImage;

		class IRenderInterface
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			virtual ~IRenderInterface() = default;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_window"></param>
			/// <returns></returns>
			virtual IDevice* InstantiateDevice(IWindow* _window) = 0;
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <returns></returns>
			virtual RHIResult DestroyDevice(IDevice* _device);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_window"></param>
			/// <param name="_device"></param>
			/// <returns></returns>
			virtual ISwapChain* InstantiateSwapChain(IWindow* _window, IDevice* _device) = 0;
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_swapChain"></param>
			/// <param name="_device"></param>
			/// <returns></returns>
			virtual RHIResult DestroySwapChain(ISwapChain* _swapChain, IDevice* _device);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_swapChain"></param>
			/// <param name="_attachments"></param>
			/// <returns></returns>
			virtual IRenderPass* InstantiateRenderPass(IDevice* _device, ISwapChain* _swapChain, const RHIRenderPass& _renderPassInfo) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_renderPass"></param>
			/// <param name="_device"></param>
			/// <returns></returns>
			virtual RHIResult DestroyRenderPass(IRenderPass* _renderPass, IDevice* _device);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_swapChain"></param>
			/// <param name="_renderPass"></param>
			/// <param name="_shaderInfos"></param>
			/// <param name="_faceFormat"></param>
			/// <param name="_depthWrite"></param>
			/// <param name="_sampleCount"></param>
			/// <returns></returns>
			virtual IPipeline* InstantiatePipeline(IDevice* _device, IRenderPass* _renderPass, const RHIPipeline& _pipelineInfo) = 0;
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_pipeline"></param>
			/// <param name="_device"></param>
			/// <returns></returns>
			virtual RHIResult DestroyPipeline(IPipeline* _pipeline, IDevice* _device);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <returns></returns>
			virtual ICommandAllocator* InstantiateCommandAllocator(IDevice* _device) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_commandAllocator"></param>
			/// <param name="_device"></param>
			/// <returns></returns>
			virtual RHIResult DestroyCommandAllocator(ICommandAllocator* _commandAllocator, IDevice* _device);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_Device"></param>
			/// <returns></returns>
			virtual IDescriptorAllocator* InstantiateDescriptorAllocator(IDevice* _device) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_descriptorAllocator"></param>
			/// <param name="_device"></param>
			/// <returns></returns>
			virtual RHIResult DestroyDescriptorAllocator(IDescriptorAllocator* _descriptorAllocator, IDevice* _device);


			virtual IFramebuffer* CreateFramebuffer(IDevice* _device, IRenderPass* _renderPass, unsigned int _width, unsigned int _height, const std::vector<IImage*>& _attachments) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_renderPass"></param>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			/// <param name="_attachments"></param>
			/// <returns></returns>
			virtual IFramebuffer* CreateFramebuffer(IDevice* _device, IRenderPass* _renderPass, unsigned int _width, unsigned int _height, const std::vector<RHIAttachment>& _attachments) = 0;
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_framebuffer"></param>
			/// <returns></returns>
			virtual RHIResult DestroyFrameBuffer(IFramebuffer* _framebuffer, IDevice* _device);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_commandAllocator"></param>
			/// <param name="_bufferType"></param>
			/// <param name="_bufferSize"></param>
			/// <param name="_data"></param>
			/// <returns></returns>
			virtual IBuffer* CreateBuffer(IDevice* _device, ICommandAllocator* _commandAllocator, RHIBufferType _bufferType, size_t _bufferSize, void* _data) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_buffer"></param>
			/// <param name="_device"></param>
			/// <returns></returns>
			virtual RHIResult DestroyBuffer(IBuffer* _buffer, IDevice* _device);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_shaderCode"></param>
			/// <returns></returns>
			virtual IShaderModule* CreateShaderModule(IDevice* _device, std::vector<uint32_t>& _shaderCode) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_shaderModule"></param>
			/// <param name="_device"></param>
			/// <returns></returns>
			virtual RHIResult DestroyShaderModule(IShaderModule* _shaderModule, IDevice* _device);

			virtual IImage* CreateImage(IDevice* _device, ICommandAllocator* _commandAllocator, void* _data, unsigned int _width, unsigned int _height, unsigned int _channelCount, const RHIImage& _imageInfo) = 0;

			virtual RHIResult DestroyImage(IImage* _image, IDevice* _device);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_data"></param>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			/// <param name="_channelCount"></param>
			/// <returns></returns>
			virtual ISampler* CreateSampler(IDevice* _device, unsigned int _mipLevels, const RHISampler& _samplerInfo) = 0;

			virtual ISampler* CreateSampler(IDevice* _device, IFramebuffer* _frameBuffer, const RHISampler& _samplerInfo) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_sampler"></param>
			/// <param name="_device"></param>
			/// <returns></returns>
			virtual RHIResult DestroySampler(ISampler* _sampler, IDevice* _device);
		};
	}
}