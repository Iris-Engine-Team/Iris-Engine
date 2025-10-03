#pragma once

#include "Core/Tools/Macros.h"

#include "Core/Graphics/RHI/IDescriptor.h"

#include <vulkan/vulkan.h>

namespace Iris
{
	namespace Engine
	{
		class VulkanDescriptor : public IDescriptor
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			~VulkanDescriptor() override = default;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IE_FORCE_INLINE VulkanDescriptor* CastToVulkan() override { return this; }

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IE_FORCE_INLINE VkDescriptorSet GetDescriptor() const { return m_Descriptor; }


			IE_FORCE_INLINE void SetDescriptor(VkDescriptorSet _descriptor) { m_Descriptor = _descriptor; }

			IRIS_ENGINE_API void UpdateTextureDescriptor(IDevice* _device, const Texture* _texture, unsigned int _binding) override;

			void UpdateShadowDescriptors(IDevice* _device, const std::vector<ShadowMap*>& _dirShadowMaps, const std::vector<ShadowMap*>& _spotShadowMaps, const std::vector<ShadowMap*>& _pointShadowMaps, unsigned int _currentDescriptor) override;

		private:
			VkDescriptorSet m_Descriptor = VK_NULL_HANDLE;
		};
	}
}