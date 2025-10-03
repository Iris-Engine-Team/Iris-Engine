#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"
#include "Core/Graphics/RHI/CommonTypes.h"

namespace Iris
{
	namespace Engine
	{
		class VulkanDescriptorAllocator;
		class IDevice;
		class IDescriptor;
		class IBuffer;
		class ISampler;
		class IDescriptorLayout;
		class Texture;
		class Material;
		class IFramebuffer;
		class ShadowMap;

		class IDescriptorAllocator
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			virtual IRIS_ENGINE_API ~IDescriptorAllocator() = default;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			virtual IE_FORCE_INLINE VulkanDescriptorAllocator* CastToVulkan() { return nullptr; }

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_Device"></param>
			/// <returns></returns>
			virtual RHIResult Initialize(IDevice* _device) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_Device"></param>
			/// <returns></returns>
			virtual RHIResult Terminate(IDevice* _device) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_Device"></param>
			/// <param name="_Buffer"></param>
			/// <param name="_Size"></param>
			/// <param name="_Binding"></param>
			/// <returns></returns>
			virtual IDescriptor* AllocateDescriptorSet(IDevice* _device, IDescriptorLayout* _descriptorLayout, IBuffer* _buffer, size_t _size, unsigned int _binding) = 0;

			 /// <summary>
			 /// 
			 /// </summary>
			 /// <param name="_Device"></param>
			 /// <param name="_Texture"></param>
			 /// <returns></returns>
			virtual IDescriptor* AllocateDescriptorSet(IDevice* _device, IDescriptorLayout* _descriptorLayout, Texture* _texture) = 0;

			virtual IDescriptor* AllocateDescriptorSet(IDevice* _device, IDescriptorLayout* _descriptorLayout, Material* _material) = 0;

			virtual IDescriptor* AllocateDescriptorSet(IDevice* _device, IDescriptorLayout* _descriptorLayout, IFramebuffer* _frameBuffer, unsigned int _firstAttachment, unsigned int _lastAttachment) = 0;

			virtual IDescriptor* AllocateDescriptorSet(IDevice* _device, IDescriptorLayout* _descriptorLayout, ISampler* _sampler, IFramebuffer* _frameBuffer, unsigned int _attachment) = 0;

			virtual IDescriptor* AllocateDescriptorSet(IDevice* _Device, IDescriptorLayout* _descriptorLayout, Texture* _irradianceTexture, Texture* _prefilteredTexture, Texture* _lookUpTexture) = 0;
			
			virtual IDescriptor* AllocateDescriptorSet(IDevice* _Device, IDescriptorLayout* _descriptorLayout, const std::vector<ShadowMap*>& _dirShadowMaps, const std::vector<ShadowMap*>& _spotShadowMaps, const std::vector<ShadowMap*>& _pointShadowMaps, unsigned int _currentDescriptor) = 0;

			virtual RHIResult FreeDescriptorSet(IDevice* _device, IDescriptor* _descriptor) = 0;
		};
	}
}