#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"
#include "Core/Graphics/RHI/CommonTypes.h"

namespace Iris
{
	namespace Engine
	{
		class IDevice;
		class ICommandAllocator;
		class VulkanImage;

		class IImage
		{
		public:
			virtual ~IImage() = default;

			virtual VulkanImage* CastToVulkan() = 0;

			IRIS_ENGINE_API IE_FORCE_INLINE const RHIAttachmentType& GetAttachmentType() const { return p_AttachmentType; }
			IRIS_ENGINE_API IE_FORCE_INLINE const RHITextureType& GetTextureType() const { return p_TextureType; }
			IRIS_ENGINE_API IE_FORCE_INLINE const RHIColorFormat& GetColorFormat() const { return p_ColorFormat; }
			IRIS_ENGINE_API IE_FORCE_INLINE const RHISampleCount& GetSampleCount() const { return p_SampleCount; }
			IRIS_ENGINE_API IE_FORCE_INLINE const uint32_t& GetMipCount() const { return p_MipCount; }

			virtual RHIResult CreateTextureImage(IDevice* _device, ICommandAllocator* _cmdAllocator, unsigned int _width, unsigned int _height, unsigned int _mipsLevels, void* _data, const RHIImage& _imageInfo) = 0;
			virtual RHIResult CreateFramebufferImage(IDevice* _device, ICommandAllocator* _cmdAllocator, unsigned int _width, unsigned int _height, const RHIImage& _imageInfo) = 0;
			virtual std::vector<IImage*> GenerateFramebufferMipMaps(IDevice* _device, unsigned int _count) = 0;
			virtual RHIResult DestroyFramebufferMipMaps(IDevice* _device, std::vector<IImage*> _mipMaps) = 0;
			virtual RHIResult DestroyImage(IDevice* _device) = 0;

		protected:
			RHIAttachmentType p_AttachmentType = RHIAttachmentType::IE_RHI_COLOR_ATTACHMENT;
			RHITextureType p_TextureType = RHITextureType::IE_RHI_TEXTURE_2D;
			RHIColorFormat p_ColorFormat = RHIColorFormat::IE_RHI_FORMAT_R8G8B8A8_SRGB;
			RHISampleCount p_SampleCount = RHISampleCount::IE_RHI_SAMPLE_COUNT_1_BIT;
			uint32_t p_MipCount = 1;
		};
	}
}