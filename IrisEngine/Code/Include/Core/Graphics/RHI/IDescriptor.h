#pragma once

#include "Core/Tools/Log.h"
#include "Core/Tools/Macros.h"
#include "Core/Graphics/RHI/CommonTypes.h"

namespace Iris
{
	namespace Engine
	{
		class VulkanDescriptor;
		class IDevice;
		class Texture;
		class ShadowMap;

		class IDescriptor
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			virtual ~IDescriptor() = default;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			virtual IE_FORCE_INLINE VulkanDescriptor* CastToVulkan() { return nullptr; }

			virtual void UpdateTextureDescriptor(IDevice* _device, const Texture* _texture, unsigned int _binding) = 0;
			virtual void UpdateShadowDescriptors(IDevice* _device, const std::vector<ShadowMap*>& _dirShadowMaps, const std::vector<ShadowMap*>& _spotShadowMaps, const std::vector<ShadowMap*>& _pointShadowMaps, unsigned int _currentDescriptor) = 0;
		};
	}
}