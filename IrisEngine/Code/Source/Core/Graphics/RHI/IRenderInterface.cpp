#include "Core/Graphics/RHI/IRenderInterface.h"
#include "Core/Graphics/RHI/IDevice.h"
#include "Core/Graphics/RHI/ISwapChain.h"
#include "Core/Graphics/RHI/IRenderPass.h"
#include "Core/Graphics/RHI/IPipeline.h"
#include "Core/Graphics/RHI/ICommandAllocator.h"
#include "Core/Graphics/RHI/IDescriptorAllocator.h"
#include "Core/Graphics/RHI/IFramebuffer.h"
#include "Core/Graphics/RHI/IBuffer.h"
#include "Core/Graphics/RHI/IShaderModule.h"
#include "Core/Graphics/RHI/ISampler.h"
#include "Core/Graphics/RHI/IImage.h"

namespace Iris
{
	namespace Engine
	{
		RHIResult IRenderInterface::DestroyDevice(IDevice* _device)
		{
			RHIResult result = _device->Terminate();

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				// TODO: ERR
			}

			delete _device;
			_device = nullptr;

			return result;
		}

		RHIResult IRenderInterface::DestroySwapChain(ISwapChain* _swapChain, IDevice* _device)
		{
			RHIResult result = _swapChain->Terminate(_device);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				// TODO: ERR
			}

			delete _swapChain;
			_swapChain = nullptr;

			return result;
		}

		RHIResult IRenderInterface::DestroyRenderPass(IRenderPass* _renderPass, IDevice* _device)
		{
			RHIResult result = _renderPass->Terminate(_device);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				// TODO: ERR
			}

			delete _renderPass;
			_renderPass = nullptr;

			return result;
		}

		RHIResult IRenderInterface::DestroyPipeline(IPipeline* _pipeline, IDevice* _device)
		{
			RHIResult result = _pipeline->Terminate(_device);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				// TODO: ERR
			}

			delete _pipeline;
			_pipeline = nullptr;

			return result;
		}

		RHIResult IRenderInterface::DestroyCommandAllocator(ICommandAllocator* _commandAllocator, IDevice* _device)
		{
			RHIResult result = _commandAllocator->Terminate(_device);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				// TODO: ERR
			}

			delete _commandAllocator;
			_commandAllocator = nullptr;

			return result;
		}

		RHIResult IRenderInterface::DestroyDescriptorAllocator(IDescriptorAllocator* _descriptorAllocator, IDevice* _device)
		{
			RHIResult result = _descriptorAllocator->Terminate(_device);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				// TODO: ERR
			}

			delete _descriptorAllocator;
			_descriptorAllocator = nullptr;

			return result;
		}

		RHIResult IRenderInterface::DestroyFrameBuffer(IFramebuffer* _framebuffer, IDevice* _device)
		{
			RHIResult result = _framebuffer->DestroyFramebuffer(_device);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				// TODO: ERR
			}

			delete _framebuffer;
			_framebuffer = nullptr;

			return result;
		}

		RHIResult IRenderInterface::DestroyBuffer(IBuffer* _buffer, IDevice* _device)
		{
			RHIResult result = _buffer->DestroyBuffer(_device);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				// TODO: ERR
			}

			delete _buffer;
			_buffer = nullptr;

			return result;
		}

		RHIResult IRenderInterface::DestroyShaderModule(IShaderModule* _shaderModule, IDevice* _device)
		{
			RHIResult result = _shaderModule->DestroyShaderModule(_device);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				// TODO: ERR
			}

			delete _shaderModule;
			_shaderModule = nullptr;

			return result;
		}

		RHIResult IRenderInterface::DestroyImage(IImage* _image, IDevice* _device)
		{
			RHIResult result = _image->DestroyImage(_device);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				// TODO: ERR
			}

			delete _image;
			_image = nullptr;

			return result;
		}

		RHIResult IRenderInterface::DestroySampler(ISampler* _sampler, IDevice* _device)
		{
			RHIResult result = _sampler->DestroySampler(_device);

			if (result != RHIResult::IE_RHI_SUCCESS)
			{
				// TODO: ERR
			}

			delete _sampler;
			_sampler = nullptr;

			return result;
		}
	}
}