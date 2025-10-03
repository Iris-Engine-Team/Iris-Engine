#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Graphics/RHI/CommonTypes.h"

#include "Core/Graphics/RHI/IImage.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace Iris
{
	namespace Engine
	{
		class VulkanDevice;
		class VulkanCommandAllocator;
		
		class VulkanImage : public IImage
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			IRIS_ENGINE_API ~VulkanImage() override = default;

			IE_FORCE_INLINE VulkanImage* CastToVulkan() { return this; }
			
			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VkImage GetImage() { return m_Image; }
			
			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VkImageView GetImageView() { return m_ImageView; }

			IRIS_ENGINE_API IE_FORCE_INLINE VkDeviceMemory GetImageMemory() { return m_ImageMemory; }

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_Image"></param>
			/// <returns></returns>
			IRIS_ENGINE_API IE_FORCE_INLINE void SetImage(VkImage _Image) { m_Image = _Image; }
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_ImageView"></param>
			/// <returns></returns>
			IRIS_ENGINE_API IE_FORCE_INLINE void SetImageView(VkImageView _ImageView) { m_ImageView = _ImageView; }

			IRIS_ENGINE_API RHIResult CreateTextureImage(IDevice* _device, ICommandAllocator* _cmdAllocator, unsigned int _width, unsigned int _height, unsigned int _channelCount, void* _data, const RHIImage& _imageInfo) override final;
			IRIS_ENGINE_API RHIResult CreateFramebufferImage(IDevice* _device, ICommandAllocator* _cmdAllocator, unsigned int _width, unsigned int _height, const RHIImage& _imageInfo) override final;
			IRIS_ENGINE_API std::vector<IImage*> GenerateFramebufferMipMaps(IDevice* _device, unsigned int _count) override;
			IRIS_ENGINE_API RHIResult DestroyFramebufferMipMaps(IDevice* _device, std::vector<IImage*> _mipMaps) override;
			IRIS_ENGINE_API RHIResult DestroyImage(IDevice* _device) override final;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			/// <param name="_samplesCount"></param>
			/// <param name="_format"></param>
			/// <param name="_usage"></param>
			/// <param name="_aspectFlags"></param>
			/// <param name="_properties"></param>
			/// <returns></returns>
			IRIS_ENGINE_API RHIResult CreateImage(VulkanDevice* _device, unsigned int _width, unsigned int _height, unsigned int _mipLevels, RHIColorFormat _format, RHITextureType _type, VkSampleCountFlagBits _samplesCount, VkImageUsageFlags _usage, VkImageAspectFlags _aspectFlags, VkMemoryPropertyFlags _properties);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <returns></returns>
			IRIS_ENGINE_API RHIResult DestroyImageOnly(VulkanDevice* _device) const;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <returns></returns>
			IRIS_ENGINE_API RHIResult DestroyImageView(VulkanDevice* _device) const;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <returns></returns>
			IRIS_ENGINE_API RHIResult FreeImageMemory(VulkanDevice* _device) const;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_format"></param>
			/// <param name="_aspectFlags"></param>
			/// <returns></returns>
			IRIS_ENGINE_API RHIResult CreateVulkanImageView(VulkanDevice* _device, VkFormat _format, VkImageAspectFlags _aspectFlags, VkImageViewType _viewType, unsigned int _mipCount, unsigned int _mipLevel);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_image"></param>
			/// <param name="_imageFormat"></param>
			/// <param name="_format"></param>
			/// <param name="_oldLayout"></param>
			/// <param name="_newLayout"></param>
			/// <returns></returns>
			IRIS_ENGINE_API static RHIResult TransitionImageLayout(VulkanDevice* _device, VulkanCommandAllocator* _commandAllocator, VkImage _image, RHITextureType _imageFormat, const RHIColorFormat& _format, VkImageLayout _oldLayout, VkImageLayout _newLayout, unsigned int _mipLevels);
			
			IRIS_ENGINE_API static RHIResult GenerateMipmaps(VulkanDevice* _device, VulkanCommandAllocator* _commandAllocator, VkImage _image, unsigned int _width, unsigned int _height, unsigned int _mipLevels, unsigned int _layerCount);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_candidates"></param>
			/// <param name="_tiling"></param>
			/// <param name="_features"></param>
			/// <returns></returns>
			IRIS_ENGINE_API static VkFormat FindSupportedFormat(VulkanDevice* _device, const std::vector<VkFormat>& _candidates, VkImageTiling _tiling, VkFormatFeatureFlags _features);
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <returns></returns>
			IRIS_ENGINE_API static VkFormat FindDepthFormat(VulkanDevice* _device);
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_format"></param>
			/// <returns></returns>
			IRIS_ENGINE_API static bool HasStencilComponent(VkFormat _format);

		private:
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			/// <param name="_samplesCount"></param>
			/// <param name="_format"></param>
			/// <param name="_usage"></param>
			/// <returns></returns>
			RHIResult CreateVulkanImage(VulkanDevice* _device, unsigned int _width, unsigned int _height, unsigned int _mipLevels, VkSampleCountFlagBits _samplesCount, RHITextureType _type, RHIColorFormat _format, VkImageUsageFlags _usage);
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_properties"></param>
			/// <returns></returns>
			RHIResult AllocateVulkanImage(VulkanDevice* _device, VkMemoryPropertyFlags _properties);

		private:
			uint32_t m_MipLevels = 0;
			VkImage m_Image = VK_NULL_HANDLE;
			VkImageView m_ImageView = VK_NULL_HANDLE;
			VkDeviceMemory m_ImageMemory = VK_NULL_HANDLE;
		};
	}
}