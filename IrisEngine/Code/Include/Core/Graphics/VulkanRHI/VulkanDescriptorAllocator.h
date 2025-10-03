#pragma once

#include "Core/Graphics/RHI/IDescriptorAllocator.h"

#include <vulkan/vulkan.h>

namespace Iris
{
	namespace Engine
	{
		class VulkanDescriptorAllocator : public IDescriptorAllocator
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			IRIS_ENGINE_API ~VulkanDescriptorAllocator() override = default;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VulkanDescriptorAllocator* CastToVulkan() override { return this; }

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VkDescriptorPool GetDescriptorAllocator() { return m_DescriptorAllocator; }

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <returns></returns>
			IRIS_ENGINE_API RHIResult Initialize(IDevice* _device) override;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <returns></returns>
			IRIS_ENGINE_API RHIResult Terminate(IDevice* _device) override;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_descriptorLayout"></param>
			/// <param name="_buffer"></param>
			/// <param name="_size"></param>
			/// <param name="_binding"></param>
			/// <returns></returns>
			IRIS_ENGINE_API IDescriptor* AllocateDescriptorSet(IDevice* _device, IDescriptorLayout* _descriptorLayout, IBuffer* _buffer, size_t _size, unsigned int _binding) override;
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_texture"></param>
			/// <returns></returns>
			IRIS_ENGINE_API IDescriptor* AllocateDescriptorSet(IDevice* _device, IDescriptorLayout* _descriptorLayout, Texture* _texture) override;

			IRIS_ENGINE_API IDescriptor* AllocateDescriptorSet(IDevice* _device, IDescriptorLayout* _descriptorLayout, Material* _material) override final;

			IRIS_ENGINE_API IDescriptor* AllocateDescriptorSet(IDevice* _device, IDescriptorLayout* _descriptorLayout, IFramebuffer* _frameBuffer, unsigned int _firstAttachment, unsigned int _lastAttachment) override final;
			IRIS_ENGINE_API IDescriptor* AllocateDescriptorSet(IDevice* _device, IDescriptorLayout* _descriptorLayout, ISampler* _sampler, IFramebuffer* _frameBuffer, unsigned int _attachment) override final;

			IRIS_ENGINE_API IDescriptor* AllocateDescriptorSet(IDevice* _Device, IDescriptorLayout* _descriptorLayout, Texture* _irradianceTexture, Texture* _prefilteredTexture, Texture* _lookUpTexture) override final;

			IDescriptor* AllocateDescriptorSet(IDevice* _Device, IDescriptorLayout* _descriptorLayout, const std::vector<ShadowMap*>& _dirShadowMaps, const std::vector<ShadowMap*>& _spotShadowMaps, const std::vector<ShadowMap*>& _pointShadowMaps, unsigned int _currentDescriptor) override final;

			IRIS_ENGINE_API RHIResult FreeDescriptorSet(IDevice* _device, IDescriptor* _descriptor) override final;

		private:
			VkDescriptorPool m_DescriptorAllocator = VK_NULL_HANDLE;
		};
	}
}