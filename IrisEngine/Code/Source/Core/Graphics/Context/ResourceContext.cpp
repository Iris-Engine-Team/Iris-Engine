#include "Core/Graphics/Context/ResourceContext.h"
#include "Core/Graphics/RHI/IRenderInterface.h"
#include "Core/Graphics/RHI/IDevice.h"
#include "Core/Graphics/RHI/ICommandAllocator.h"
#include "Core/Graphics/RHI/IDescriptorAllocator.h"
#include "Core/Graphics/RHI/IDescriptorLayout.h"
#include "Core/Graphics/RHI/IDescriptor.h"
#include "Core/Graphics/RHI/ISwapChain.h"

namespace Iris
{
	namespace Engine
	{
		RHIResult ResourceContext::Initialize(IWindow* _window, IDevice* _device, ISwapChain* _swapChain, IRenderInterface* _renderInterface)
		{
			_window;

			if (!_renderInterface)
			{
				// TODO: ERR
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			p_DeviceReference = _device;

			p_SwapChainReference = _swapChain;

			p_RenderInterfaceReference = _renderInterface;
		
			m_CommandAllocator = _renderInterface->InstantiateCommandAllocator(p_DeviceReference);

			m_DescriptorAllocator = _renderInterface->InstantiateDescriptorAllocator(p_DeviceReference);

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult ResourceContext::Terminate(IRenderInterface* _renderInterface)
		{
			if (!_renderInterface)
			{
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			p_DeviceReference->WaitIdle();

			_renderInterface->DestroyDescriptorAllocator(m_DescriptorAllocator, p_DeviceReference);

			_renderInterface->DestroyCommandAllocator(m_CommandAllocator, p_DeviceReference);

			return RHIResult::IE_RHI_SUCCESS;
		}

		IBuffer* ResourceContext::CreateBuffer(RHIBufferType _bufferType, size_t _bufferSize, void* _data)
		{
			return p_RenderInterfaceReference->CreateBuffer(p_DeviceReference, m_CommandAllocator, _bufferType, _bufferSize, _data);
		}

		std::vector<IBuffer*> ResourceContext::CreateBuffers(RHIBufferType _bufferType, size_t _bufferSize, void* _data)
		{
			std::vector<IBuffer*> buffers(MAX_FRAMES_IN_FLIGHT);

			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
			{
				buffers[i] = p_RenderInterfaceReference->CreateBuffer(p_DeviceReference, m_CommandAllocator, _bufferType, _bufferSize, _data);
			}

			return buffers;
		}

		IShaderModule* ResourceContext::CreateShaderModule(std::vector<uint32_t>& _shaderCode)
		{
			return p_RenderInterfaceReference->CreateShaderModule(p_DeviceReference, _shaderCode);
		}

		IRIS_ENGINE_API RHIResult ResourceContext::DestroyShaderModule(IShaderModule* _shaderModule)
		{
			return p_RenderInterfaceReference->DestroyShaderModule(_shaderModule, p_DeviceReference);
		}

		RHIResult ResourceContext::DestroyBuffer(IBuffer* _buffer)
		{
			return p_RenderInterfaceReference->DestroyBuffer(_buffer, p_DeviceReference);
		}

		RHIResult ResourceContext::DestroyBuffers(std::vector<IBuffer*>& _buffers)
		{
			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
			{
				p_RenderInterfaceReference->DestroyBuffer(_buffers[i], p_DeviceReference);
			}

			_buffers.clear();

			return RHIResult();
		}

		IImage* ResourceContext::CreateImage(unsigned int _width, unsigned int _height, unsigned int _channelCount, void* _data, const RHIImage& _imageInfo)
		{
			return p_RenderInterfaceReference->CreateImage(p_DeviceReference, m_CommandAllocator, _data, _width, _height, _channelCount, _imageInfo);
		}

		RHIResult ResourceContext::DestroyImage(IImage* _image)
		{
			return p_RenderInterfaceReference->DestroyImage(_image, p_DeviceReference);
		}

		ISampler* ResourceContext::CreateSampler(unsigned int _mipLevels, const RHISampler& _samplerInfo)
		{
			return p_RenderInterfaceReference->CreateSampler(p_DeviceReference, _mipLevels, _samplerInfo);
		}

		ISampler* ResourceContext::CreateSampler(IFramebuffer* _frameBuffer, const RHISampler& _samplerInfo)
		{
			return p_RenderInterfaceReference->CreateSampler(p_DeviceReference, _frameBuffer, _samplerInfo);
		}

		RHIResult ResourceContext::DestroySampler(ISampler* _sampler)
		{
			return p_RenderInterfaceReference->DestroySampler(_sampler, p_DeviceReference);
		}

		IDescriptor* ResourceContext::CreateBufferDescriptor(IDescriptorLayout* _descriptorLayout, IBuffer* _buffer, size_t _size, unsigned int _binding)
		{
			return m_DescriptorAllocator->AllocateDescriptorSet(p_DeviceReference, _descriptorLayout, _buffer, _size, _binding);
		}

		std::vector<IDescriptor*> ResourceContext::CreateBufferDescriptors(IDescriptorLayout* _descriptorLayout, std::vector<IBuffer*>& _buffers, size_t _size, unsigned int _binding) const
		{
			std::vector<IDescriptor*> descriptors(MAX_FRAMES_IN_FLIGHT);

			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
			{
				descriptors[i] = m_DescriptorAllocator->AllocateDescriptorSet(p_DeviceReference, _descriptorLayout, _buffers[i], _size, _binding);
			}

			return descriptors;
		}

		IDescriptor* ResourceContext::CreateTextureDescriptor(IDescriptorLayout* _descriptorLayout, Texture* _texture) const
		{
			return m_DescriptorAllocator->AllocateDescriptorSet(p_DeviceReference, _descriptorLayout, _texture);
		}

		void ResourceContext::UpdateTextureDescriptor(IDescriptor* _descriptor, const Texture* _texture, unsigned int _binding)
		{
			return _descriptor->UpdateTextureDescriptor(p_DeviceReference, _texture, _binding);
		}

		IDescriptor* ResourceContext::CreateMaterialDescriptor(IDescriptorLayout* _descriptorLayout, Material* _material) const
		{
			return m_DescriptorAllocator->AllocateDescriptorSet(p_DeviceReference, _descriptorLayout, _material);
		}

		IDescriptor* ResourceContext::CreateAttachmentsDescriptor(IDescriptorLayout* _descriptorLayout, IFramebuffer* _frameBuffer, unsigned int _firstAttachment, unsigned int _lastAttachment) const
		{
			return m_DescriptorAllocator->AllocateDescriptorSet(p_DeviceReference, _descriptorLayout, _frameBuffer, _firstAttachment, _lastAttachment);
		}

		IDescriptor* ResourceContext::CreateAttachmentsDescriptor(IDescriptorLayout* _descriptorLayout, ISampler* _sampler, IFramebuffer* _frameBuffer, unsigned int _Attachment) const
		{
			return m_DescriptorAllocator->AllocateDescriptorSet(p_DeviceReference, _descriptorLayout, _sampler, _frameBuffer, _Attachment);
		}

		IDescriptor* ResourceContext::CreateIBLDescriptor(IDescriptorLayout* _descriptorLayout, Texture* _irradianceTexture, Texture* _prefilteredTexture, Texture* _lookUpTexture) const
		{
			return m_DescriptorAllocator->AllocateDescriptorSet(p_DeviceReference, _descriptorLayout, _irradianceTexture, _prefilteredTexture, _lookUpTexture);
		}

		std::vector<IDescriptor*> ResourceContext::CreateShadowMapDescriptors(IDescriptorLayout* _descriptorLayout, const std::vector<ShadowMap*>& _dirLightShadowMaps, const std::vector<ShadowMap*>& _spotLightShadowMaps, const std::vector<ShadowMap*>& _pointLightShadowMaps) const
		{
			std::vector<IDescriptor*> descriptors(MAX_FRAMES_IN_FLIGHT);

			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
			{
				descriptors[i] = m_DescriptorAllocator->AllocateDescriptorSet(p_DeviceReference, _descriptorLayout, _dirLightShadowMaps, _spotLightShadowMaps, _pointLightShadowMaps, static_cast<unsigned int>(i));
			}

			return descriptors;
		}

		std::vector<IDescriptor*> ResourceContext::CreateAttachmentsDescriptors(IDescriptorLayout* _descriptorLayout, ISampler* _sampler, const std::vector<IFramebuffer*>& _frameBuffers, unsigned int _Attachment) const
		{
			std::vector<IDescriptor*> descriptors(MAX_FRAMES_IN_FLIGHT);

			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
			{
				descriptors[i] = m_DescriptorAllocator->AllocateDescriptorSet(p_DeviceReference, _descriptorLayout, _sampler, _frameBuffers[i], _Attachment);
			}

			return descriptors;
		}

		RHIResult ResourceContext::DeleteDescriptor(IDescriptor* _descriptor)
		{
			return m_DescriptorAllocator->FreeDescriptorSet(p_DeviceReference, _descriptor);
		}

		RHIResult ResourceContext::DeleteDescriptors(std::vector<IDescriptor*>& _descriptors)
		{
			if (_descriptors.size() == 0)
				return RHIResult();

			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
			{
				m_DescriptorAllocator->FreeDescriptorSet(p_DeviceReference, _descriptors[i]);
			}

			_descriptors.clear();

			return RHIResult();
		}
	}
}