#pragma once

#include "Core/Graphics/Context/IContext.h"
#include <vector>

namespace Iris
{
	namespace Engine
	{
		class IRenderInterface;
		class IBuffer;
		class ICommandAllocator;
		class IDescriptorAllocator;
		class IDescriptorLayout;
		class IDescriptor;
		class ISampler;
		class IImage;
		class IShaderModule;
		class IFramebuffer;
		class Texture;
		class Material;
		class ShadowMap;

		class ResourceContext : public IContext
		{
		public:
			/// <summary>
			/// Default ResourceContext's destructor
			/// </summary>
			IRIS_ENGINE_API ~ResourceContext() override final = default;

			/// <summary>
			/// Command allocator getter
			/// </summary>
			/// <returns>: A pointer to the command allocator of the rendering context</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE ICommandAllocator* GetCommandAllocator() { return m_CommandAllocator; }

			/// <summary>
			/// Initializes the resource context variables
			/// </summary>
			/// <param name="_window">: Window of the application</param>
			/// <param name="_device">: Device of the application</param>
			/// <param name="_swapChain">: SwapChain of the application</param>
			/// <param name="_renderInterface">: Render interface of the application</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API RHIResult Initialize(IWindow* _window, IDevice* _device, ISwapChain* _swapChain, IRenderInterface* _renderInterface) override final;
			
			/// <summary>
			/// Terminates the resource context variables
			/// </summary>
			/// <param name="_renderInterface">: Render interface of the application</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API RHIResult Terminate(IRenderInterface* _renderInterface) override final;
			
			/// <summary>
			/// Creates a buffer
			/// </summary>
			/// <param name="_bufferType">: Type of the buffer</param>
			/// <param name="_bufferSize">: Size of the buffer</param>
			/// <param name="_data">: Pointer to the data of the buffer</param>
			/// <returns>A new buffer</returns>
			IRIS_ENGINE_API IBuffer* CreateBuffer(RHIBufferType _bufferType, size_t _bufferSize, void* _data);
			
			/// <summary>
			/// Creates a list of buffers according to the number of frames in flight
			/// </summary>
			/// <param name="_bufferType">: Type of the buffer</param>
			/// <param name="_bufferSize">: Size of the buffer</param>
			/// <param name="_data">: Pointer to the data of the buffer</param>
			/// <returns>A list of new buffers</returns>
			IRIS_ENGINE_API std::vector<IBuffer*> CreateBuffers(RHIBufferType _bufferType, size_t _bufferSize, void* _data);

			/// <summary>
			/// Creates a shader module
			/// </summary>
			/// <param name="_shaderCode">: Source code of the shader</param>
			/// <returns>A new shader module</returns>
			IRIS_ENGINE_API IShaderModule* CreateShaderModule(std::vector<uint32_t>& _shaderCode);
			
			/// <summary>
			/// Destroys a shader module
			/// </summary>
			/// <param name="_shaderModule">: Shader module to destroy</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API RHIResult DestroyShaderModule(IShaderModule* _shaderModule);

			/// <summary>
			/// Destroys a buffer
			/// </summary>
			/// <param name="_buffer">: Buffer to destroy</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API RHIResult DestroyBuffer(IBuffer* _buffer);
			
			/// <summary>
			/// Destroys a list of buffers
			/// </summary>
			/// <param name="_buffers">: List of buffers to destroy</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API RHIResult DestroyBuffers(std::vector<IBuffer*>& _buffers);

			/// <summary>
			/// Creates a IImage
			/// </summary>
			/// <param name="_width">: Width of the image</param>
			/// <param name="_height">: Height of the image</param>
			/// <param name="_channelCount">: Count of channels of the image</param>
			/// <param name="_data">: Data of the image</param>
			/// <param name="_imageInfo">: Information for the image creation</param>
			/// <returns>A new IImage</returns>
			IRIS_ENGINE_API IImage* CreateImage(unsigned int _width, unsigned int _height, unsigned int _channelCount, void* _data, const RHIImage& _imageInfo);

			/// <summary>
			/// Destroys a IImage
			/// </summary>
			/// <param name="_image">: Image to destroy</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API RHIResult DestroyImage(IImage* _image);

			/// <summary>
			/// Creates a sampler for a texture
			/// </summary>
			/// <param name="_data">: Data of the texture</param> // Should not be created here
			/// <param name="_width">: Width of the texture</param>
			/// <param name="_height">: Height of the texture</param>
			/// <param name="_channelCount">: Number of channels (RGBA)</param>
			/// <param name="_mipLevels">: Count of mip levels</param>
			/// <param name="_samplerInfo">: Sampler parameters</param>
			/// <param name="_colorFormat">: Color format of the texture</param>
			/// <returns>A new sampler</returns>
			IRIS_ENGINE_API ISampler* CreateSampler(unsigned int _mipLevels, const RHISampler& _samplerInfo);

			/// <summary>
			/// Creates a sampler for a framebuffer
			/// </summary>
			/// <param name="_frameBuffer">: Frambuffer for the sampler</param>
			/// <param name="_samplerInfo">: Sampler parameters</param>
			/// <returns>A new Sampler</returns>
			IRIS_ENGINE_API ISampler* CreateSampler(IFramebuffer* _frameBuffer, const RHISampler& _samplerInfo);
			
			/// <summary>
			/// Destroys a sampler
			/// </summary>
			/// <param name="_sampler">: Sampler to destroy</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API RHIResult DestroySampler(ISampler* _sampler);

			/// <summary>
			/// Creates a descriptor for a buffer
			/// </summary>
			/// <param name="_descriptorLayout">: Descriptor layout of the pipeline</param>
			/// <param name="_buffer">: Buffer for the descriptor</param>
			/// <param name="_size">: Size of the buffer</param>
			/// <param name="_binding">: Binding position in the shader</param>
			/// <returns>A new descriptor</returns>
			IRIS_ENGINE_API IDescriptor* CreateBufferDescriptor(IDescriptorLayout* _descriptorLayout, IBuffer* _buffer, size_t _size, unsigned int _binding);
			
			/// <summary>
			/// Creates a list of descriptors for a buffer according to the number of frames in flight
			/// </summary>
			/// <param name="_descriptorLayout">: Descriptor layout of the pipeline</param>
			/// <param name="_buffer">: Buffer for the descriptor</param>
			/// <param name="_size">: Size of the buffer</param>
			/// <param name="_binding">: Binding position in the shader</param>
			/// <returns>A list of new descriptor</returns> 
			IRIS_ENGINE_API std::vector<IDescriptor*> CreateBufferDescriptors(IDescriptorLayout* _descriptorLayout, std::vector<IBuffer*>& _buffers, size_t _size, unsigned int _binding) const;

			/// <summary>
			/// Creates a descriptor for a texture
			/// </summary>
			/// <param name="_descriptorLayout">: Descriptor layout of the pipeline</param>
			/// <param name="_texture">: Texture for the descriptor</param>
			/// <returns>A new descriptor</returns>
			IRIS_ENGINE_API IDescriptor* CreateTextureDescriptor(IDescriptorLayout* _descriptorLayout, Texture* _texture) const;
			
			/// <summary>
			/// Creates a descriptor for a material
			/// </summary>
			/// <param name="_descriptorLayout">: Descriptor layout of the pipeline</param>
			/// <param name="_material">: Material for the descriptor</param>
			/// <returns>A new descriptor</returns>
			IRIS_ENGINE_API IDescriptor* CreateMaterialDescriptor(IDescriptorLayout* _descriptorLayout, Material* _material) const;
			
			IRIS_ENGINE_API void UpdateTextureDescriptor(IDescriptor* _descriptor, const Texture* _texture, unsigned int _binding);

			/// <summary>
			/// Creates a descriptor for different attachments
			/// </summary>
			/// <param name="_descriptorLayout">: Descriptor layout of the pipeline</param>
			/// <param name="_frameBuffer">: Framebuffer containing the attachments</param>
			/// <param name="_firstAttachment">: First attachment of the list</param>
			/// <param name="_lastAttachment">: Last attachment of the list</param>
			/// <returns>A new descriptor</returns>
			IRIS_ENGINE_API IDescriptor* CreateAttachmentsDescriptor(IDescriptorLayout* _descriptorLayout, IFramebuffer* _frameBuffer, unsigned int _firstAttachment, unsigned int _lastAttachment) const;
			
			/// <summary>
			/// Creates a descriptor for a single attachment
			/// </summary>
			/// <param name="_descriptorLayout">: Descriptor layout of the pipeline</param>
			/// <param name="_sampler">: </param>
			/// <param name="_frameBuffer">: Framebuffer containing the attachments</param>
			/// <param name="_firstAttachment">: First attachment of the list</param>
			/// <param name="_lastAttachment">: Last attachment of the list</param>
			/// <returns>A new descriptor</returns>
			IRIS_ENGINE_API IDescriptor* CreateAttachmentsDescriptor(IDescriptorLayout* _descriptorLayout, ISampler* _sampler, IFramebuffer* _frameBuffer, unsigned int _Attachment) const;
			
			IRIS_ENGINE_API IDescriptor* CreateIBLDescriptor(IDescriptorLayout* _descriptorLayout, Texture* _irradianceTexture, Texture* _prefilteredTexture, Texture* _lookUpTexture) const;

			IRIS_ENGINE_API std::vector<IDescriptor*> CreateShadowMapDescriptors(IDescriptorLayout* _descriptorLayout, const std::vector<ShadowMap*>& _dirLightShadowMaps, const std::vector<ShadowMap*>& _spotLightShadowMaps, const std::vector<ShadowMap*>& _pointLightShadowMaps) const;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_descriptorLayout"></param>
			/// <param name="_sampler"></param>
			/// <param name="_frameBuffers"></param>
			/// <param name="_Attachment"></param>
			/// <returns></returns>
			IRIS_ENGINE_API std::vector<IDescriptor*> CreateAttachmentsDescriptors(IDescriptorLayout* _descriptorLayout, ISampler* _sampler, const std::vector<IFramebuffer*>& _frameBuffers, unsigned int _Attachment) const;

			/// <summary>
			/// Deletes a descriptor
			/// </summary>
			/// <param name="_descriptor">: Descriptor to delete</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API RHIResult DeleteDescriptor(IDescriptor* _descriptor);
			
			/// <summary>
			/// Deletes a list of descriptors
			/// </summary>
			/// <param name="_descriptors">: List of descriptors to delete</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API RHIResult DeleteDescriptors(std::vector<IDescriptor*>& _descriptors);
			 
		private:
			ICommandAllocator* m_CommandAllocator = nullptr;

			IDescriptorAllocator* m_DescriptorAllocator = nullptr;
		};
	}
}