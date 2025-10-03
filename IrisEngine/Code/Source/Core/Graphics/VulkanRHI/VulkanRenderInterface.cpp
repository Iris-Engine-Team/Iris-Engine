#include "Core/Graphics/VulkanRHI/VulkanRenderInterface.h"
#include "Core/Graphics/VulkanRHI/VulkanDevice.h"
#include "Core/Graphics/VulkanRHI/VulkanSwapChain.h"
#include "Core/Graphics/VulkanRHI/VulkanRenderPass.h"
#include "Core/Graphics/VulkanRHI/VulkanPipeline.h"
#include "Core/Graphics/VulkanRHI/VulkanCommandAllocator.h"
#include "Core/Graphics/VulkanRHI/VulkanDescriptorAllocator.h"
#include "Core/Graphics/VulkanRHI/VulkanFramebuffer.h"
#include "Core/Graphics/VulkanRHI/VulkanBuffer.h"
#include "Core/Graphics/VulkanRHI/VulkanShaderModule.h"
#include "Core/Graphics/VulkanRHI/VulkanSampler.h"

#include <array>

namespace Iris
{
	namespace Engine
	{
		IDevice* VulkanRenderInterface::InstantiateDevice(IWindow* _window)
		{
			IDevice* device = new VulkanDevice;
			RHIResult result = device->Initialize(_window);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to instantiate device! Error code: %i", result);
				return nullptr;
			}

			return device;
		}

		ISwapChain* VulkanRenderInterface::InstantiateSwapChain(IWindow* _window, IDevice* _device)
		{
			ISwapChain* swapChain = new VulkanSwapChain;
			RHIResult result = swapChain->Initialize(_window, _device);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to swap chain device! Error code: %i", result);
				return nullptr;
			}

			return swapChain;
		}

		IRenderPass* VulkanRenderInterface::InstantiateRenderPass(IDevice* _device, ISwapChain* _swapChain, const RHIRenderPass& _renderPassInfo)
		{
			IRenderPass* renderPass = new VulkanRenderPass;
			RHIResult result = renderPass->Initialize(_device, _swapChain, _renderPassInfo);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to instantiate render pass! Error code: %i", result);
				return nullptr;
			}

			return renderPass;
		}

		IPipeline* VulkanRenderInterface::InstantiatePipeline(IDevice* _device, IRenderPass* _renderPass, const RHIPipeline& _pipelineInfo)
		{
			IPipeline* pipeline = new VulkanPipeline;
			RHIResult result = pipeline->Initialize(_device, _renderPass, _pipelineInfo);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to instantiate pipeline! Error code: %i", result);
				return nullptr;
			}

			return pipeline;
		}

		ICommandAllocator* VulkanRenderInterface::InstantiateCommandAllocator(IDevice* _device)
		{
			ICommandAllocator* commandAllocator = new VulkanCommandAllocator;
			RHIResult result = commandAllocator->Initialize(_device);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to instantiate command allocator! Error code: %i", result);
				return nullptr;
			}

			return commandAllocator;
		}

		IDescriptorAllocator* VulkanRenderInterface::InstantiateDescriptorAllocator(IDevice* _device)
		{
			IDescriptorAllocator* descriptorAllocator = new VulkanDescriptorAllocator;
			RHIResult result = descriptorAllocator->Initialize(_device);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to instantiate descriptor allocator! Error code: %i", result);
				return nullptr;
			}

			return descriptorAllocator;
		}

		IFramebuffer* VulkanRenderInterface::CreateFramebuffer(IDevice* _device, IRenderPass* _renderPass, unsigned int _width, unsigned int _height, const std::vector<IImage*>& _attachments)
		{
			IFramebuffer* framebuffer = new VulkanFramebuffer;
			RHIResult result = framebuffer->CreateFramebuffer(_device, _renderPass, _width, _height, _attachments);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to create framebuffer! Error code: %i", result);
				return nullptr;
			}

			return framebuffer;
		}

		IFramebuffer* VulkanRenderInterface::CreateFramebuffer(IDevice* _device, IRenderPass* _renderPass, unsigned int _width, unsigned int _height, const std::vector<RHIAttachment>& _attachments)
		{
			IFramebuffer* framebuffer = new VulkanFramebuffer;
			RHIResult result = framebuffer->CreateFramebuffer(_device, _renderPass, _width, _height, _attachments);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to create framebuffer! Error code: %i", result);
				return nullptr;
			}

			return framebuffer;
		}

		IBuffer* VulkanRenderInterface::CreateBuffer(IDevice* _device, ICommandAllocator* _commandAllocator, RHIBufferType _bufferType, size_t _bufferSize, void* _data)
		{
			IBuffer* buffer = new VulkanBuffer;
			RHIResult result = buffer->CreateBuffer(_device, _commandAllocator, _bufferType, _bufferSize, _data);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to create buffer! Error code: %i", result);
				return nullptr;
			}

			return buffer;
		}

		IShaderModule* VulkanRenderInterface::CreateShaderModule(IDevice* _device, std::vector<uint32_t>& _shaderCode)
		{
			IShaderModule* shaderModule = new VulkanShaderModule;
			RHIResult result = shaderModule->CreateShaderModule(_device, _shaderCode);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to create shader module! Error code: %i", result);
				return nullptr;
			}

			return shaderModule;
		}

		IImage* VulkanRenderInterface::CreateImage(IDevice* _device, ICommandAllocator* _commandAllocator, void* _data, unsigned int _width, unsigned int _height, unsigned int _channelCount, const RHIImage& _imageInfo)
		{
			IImage* image = new VulkanImage;
			RHIResult result = image->CreateTextureImage(_device, _commandAllocator, _width, _height, _channelCount, _data, _imageInfo);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to create sampler! Error code: %i", result);
				return nullptr;
			}

			return image;
		}

		ISampler* VulkanRenderInterface::CreateSampler(IDevice* _device, unsigned int _mipLevels, const RHISampler& _samplerInfo)
		{
			ISampler* sampler = new VulkanSampler;
			RHIResult result = sampler->CreateSampler(_device, _mipLevels, _samplerInfo);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to create sampler! Error code: %i", result);
				return nullptr;
			}

			return sampler;
		}

		ISampler* VulkanRenderInterface::CreateSampler(IDevice* _device, IFramebuffer* _frameBuffer, const RHISampler& _samplerInfo)
		{
			ISampler* sampler = new VulkanSampler;
			RHIResult result = sampler->CreateSampler(_device, _frameBuffer, _samplerInfo);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to create sampler! Error code: %i", result);
				return nullptr;
			}

			return sampler;
		}

		VkImageUsageFlags VulkanRenderInterface::ConvertUsageFlag(RHIAttachmentType _type)
		{
			std::array<VkImageUsageFlags, 5> formats = { VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT };

			return formats[static_cast<int>(_type)];
		}

		VkFormat VulkanRenderInterface::ConvertColorFormat(RHIColorFormat _format)
		{
			// Format list based on RHIColorFormat type
			std::array<VkFormat, 14> formats = { 
				VK_FORMAT_UNDEFINED,

				VK_FORMAT_R8G8B8A8_UNORM, 
				VK_FORMAT_B8G8R8A8_UNORM,
				VK_FORMAT_R8G8B8_UNORM,
				VK_FORMAT_B8G8R8_UNORM,

				VK_FORMAT_R8G8B8A8_SRGB, 
				VK_FORMAT_B8G8R8A8_SRGB,
				VK_FORMAT_R8G8B8_SRGB,
				VK_FORMAT_B8G8R8_SRGB,

				VK_FORMAT_R16G16B16A16_SFLOAT,
				VK_FORMAT_R32G32B32A32_SFLOAT,

				VK_FORMAT_D32_SFLOAT, 
				VK_FORMAT_D32_SFLOAT_S8_UINT, 
				VK_FORMAT_D24_UNORM_S8_UINT
			};

			return formats[static_cast<int>(_format)];
		}

		VkSampleCountFlagBits VulkanRenderInterface::ConvertSampleCount(RHISampleCount _samples)
		{
			// Sample list based on RHISampleCount type
			std::array<VkSampleCountFlagBits, 7> samples = { 
				VK_SAMPLE_COUNT_1_BIT, 
				VK_SAMPLE_COUNT_2_BIT, 
				VK_SAMPLE_COUNT_4_BIT, 
				VK_SAMPLE_COUNT_8_BIT, 
				VK_SAMPLE_COUNT_16_BIT, 
				VK_SAMPLE_COUNT_32_BIT, 
				VK_SAMPLE_COUNT_64_BIT };
			
			return samples[static_cast<int>(_samples)];
		}

		VkImageLayout VulkanRenderInterface::ConvertLayout(RHILayoutType _layout)
		{
			// Layout list based on RHILayout type
			std::array<VkImageLayout, 5> layouts = {
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
			};

			return layouts[static_cast<int>(_layout)];;
		}

		VkAttachmentLoadOp VulkanRenderInterface::ConvertLoadOperation(RHILoadOperation _operation)
		{
			// Operation list based on RHIOperation type
			std::array<VkAttachmentLoadOp, 2> operations = {
				VK_ATTACHMENT_LOAD_OP_LOAD,
				VK_ATTACHMENT_LOAD_OP_CLEAR
			};

			return operations[static_cast<int>(_operation)];
		}

		VkAttachmentStoreOp VulkanRenderInterface::ConvertStoreOperation(RHIStoreOperation _operation)
		{
			// Operation list based on RHIOperation type
			std::array<VkAttachmentStoreOp, 2> operations = {
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE
			};

			return operations[static_cast<int>(_operation)];
		}

		VkFrontFace VulkanRenderInterface::ConvertFrontFace(RHIFrontFace _frontFace)
		{
			// Operation list based on RHIOperation type
			std::array<VkFrontFace, 2> frontFace = {
				VK_FRONT_FACE_COUNTER_CLOCKWISE,
				VK_FRONT_FACE_CLOCKWISE
			};

			return frontFace[static_cast<int>(_frontFace)];
		}

		VkCullModeFlagBits VulkanRenderInterface::ConvertFaceCulling(RHIFaceCulling _faceCulling)
		{
			// Operation list based on RHIOperation type
			std::array<VkCullModeFlagBits, 4> cullMode = {
				VK_CULL_MODE_FRONT_AND_BACK,
				VK_CULL_MODE_FRONT_BIT,
				VK_CULL_MODE_BACK_BIT,
				VK_CULL_MODE_NONE
			};

			return cullMode[static_cast<int>(_faceCulling)];
		}

		VkFilter VulkanRenderInterface::ConvertSamplerFilter(RHISamplerFilter _filter)
		{
			// Operation list based on RHISamplerFilter type
			std::array<VkFilter, 3> filter = {
				VK_FILTER_NEAREST,
				VK_FILTER_LINEAR,
				VK_FILTER_CUBIC_EXT
			};

			return filter[static_cast<int>(_filter)];
		}

		VkSamplerAddressMode VulkanRenderInterface::ConvertSamplerAdress(RHISamplerAdress _adress)
		{
			// Operation list based on RHISamplerAdress type
			std::array<VkSamplerAddressMode, 5> adressMode = {
				VK_SAMPLER_ADDRESS_MODE_REPEAT,
				VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
				VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
				VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
				VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE
			};

			return adressMode[static_cast<int>(_adress)];
		}

		VkBorderColor VulkanRenderInterface::ConvertSamplerBorderColor(RHISamplerBorderColor _borderColor)
		{
			// Operation list based on RHISamplerBorderColor type
			std::array<VkBorderColor, 4> borderColor = {
				VK_BORDER_COLOR_INT_OPAQUE_BLACK,
				VK_BORDER_COLOR_INT_OPAQUE_WHITE,
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
				VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE
			};

			return borderColor[static_cast<int>(_borderColor)];
		}

		VkPolygonMode VulkanRenderInterface::ConvertPolygonMode(RHIPolygonMode _polygonMode)
		{
			// Operation list based on RHIPolygonMode type
			std::array<VkPolygonMode, 3> polygonMode = {
				VK_POLYGON_MODE_FILL,
				VK_POLYGON_MODE_LINE,
				VK_POLYGON_MODE_POINT
			};

			return polygonMode[static_cast<int>(_polygonMode)];
		}

		VkCompareOp VulkanRenderInterface::ConvertCompareOperation(RHICompareOperation _compareOperation)
		{
			// Operation list based on RHICompareOperation type
			std::array<VkCompareOp, 8> compareOperation = {
				VK_COMPARE_OP_NEVER,
				VK_COMPARE_OP_LESS,
				VK_COMPARE_OP_EQUAL,
				VK_COMPARE_OP_LESS_OR_EQUAL,
				VK_COMPARE_OP_GREATER,
				VK_COMPARE_OP_NOT_EQUAL,
				VK_COMPARE_OP_GREATER_OR_EQUAL,
				VK_COMPARE_OP_ALWAYS
			};

			return compareOperation[static_cast<int>(_compareOperation)];
		}
	}
}