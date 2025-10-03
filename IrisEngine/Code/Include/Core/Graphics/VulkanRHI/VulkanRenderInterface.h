#pragma once

#include "Core/Graphics/RHI/IRenderInterface.h"
#include "Core/Graphics/RHI/CommonTypes.h"

#include <vulkan/vulkan.h>

namespace Iris
{
	namespace Engine
	{
		class VulkanRenderInterface : public IRenderInterface
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			~VulkanRenderInterface() override final = default;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_window"></param>
			/// <returns></returns>
			IDevice* InstantiateDevice(IWindow* _window) override final;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_window"></param>
			/// <param name="_device"></param>
			/// <returns></returns>
			ISwapChain* InstantiateSwapChain(IWindow* _window, IDevice* _device) override final;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_swapChain"></param>
			/// <param name="_attachments"></param>
			/// <returns></returns>
			IRenderPass* InstantiateRenderPass(IDevice* _device, ISwapChain* _swapChain, const RHIRenderPass& _renderPassInfo) override final;

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
			IPipeline* InstantiatePipeline(IDevice* _device, IRenderPass* _renderPass, const RHIPipeline& _pipelineInfo) override final;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <returns></returns>
			ICommandAllocator* InstantiateCommandAllocator(IDevice* _device) override final;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_Device"></param>
			/// <returns></returns>
			IDescriptorAllocator* InstantiateDescriptorAllocator(IDevice* _device) override final;

			IFramebuffer* CreateFramebuffer(IDevice* _device, IRenderPass* _renderPass, unsigned int _width, unsigned int _height, const std::vector<IImage*>& _attachments) override final;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_renderPass"></param>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			/// <param name="_attachments"></param>
			/// <returns></returns>
			IFramebuffer* CreateFramebuffer(IDevice* _device, IRenderPass* _renderPass, unsigned int _width, unsigned int _height, const std::vector<RHIAttachment>& _attachments) override final;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_commandAllocator"></param>
			/// <param name="_bufferType"></param>
			/// <param name="_bufferSize"></param>
			/// <param name="_data"></param>
			/// <returns></returns>
			IBuffer* CreateBuffer(IDevice* _device, ICommandAllocator* _commandAllocator, RHIBufferType _bufferType, size_t _bufferSize, void* _data) override final;

			IShaderModule* CreateShaderModule(IDevice* _device, std::vector<uint32_t>& _shaderCode) override final;

			IImage* CreateImage(IDevice* _device, ICommandAllocator* _commandAllocator, void* _data, unsigned int _width, unsigned int _height, unsigned int _channelCount, const RHIImage& _imageInfo) override final;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_data"></param>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			/// <param name="_channelCount"></param>
			/// <returns></returns>
			ISampler* CreateSampler(IDevice* _device, unsigned int _mipLevels, const RHISampler& _samplerInfo) override final;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_frameBuffer"></param>
			/// <returns></returns>
			ISampler* CreateSampler(IDevice* _device, IFramebuffer* _frameBuffer, const RHISampler& _samplerInfo) override final;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_type"></param>
			/// <returns></returns>
			static VkImageUsageFlags ConvertUsageFlag(RHIAttachmentType _type);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_Format"></param>
			/// <returns></returns>
			static VkFormat ConvertColorFormat(RHIColorFormat _format);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_Samples"></param>
			/// <returns></returns>
			static VkSampleCountFlagBits ConvertSampleCount(RHISampleCount _samples);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_layout"></param>
			/// <returns></returns>
			static VkImageLayout ConvertLayout(RHILayoutType _layout);

			static VkAttachmentLoadOp ConvertLoadOperation(RHILoadOperation _operation);
			static VkAttachmentStoreOp ConvertStoreOperation(RHIStoreOperation _operation);

			static VkFrontFace ConvertFrontFace(RHIFrontFace _frontFace);
			static VkCullModeFlagBits ConvertFaceCulling(RHIFaceCulling _faceCulling);

			static VkFilter ConvertSamplerFilter(RHISamplerFilter _filter);
			static VkSamplerAddressMode ConvertSamplerAdress(RHISamplerAdress _adress);
			static VkBorderColor ConvertSamplerBorderColor(RHISamplerBorderColor _borderColor);

			static VkPolygonMode ConvertPolygonMode(RHIPolygonMode _polygonMode);
			static VkCompareOp ConvertCompareOperation(RHICompareOperation _compareOperation);
		};
	}
}