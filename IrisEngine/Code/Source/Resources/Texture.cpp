#include "Resources/Texture.h"
#include "Loader/TextureLoader/ITextureLoader.h"

#include "Core/Graphics/RHI/ISampler.h"
#include "Core/Graphics/RHI/IDescriptor.h"
#include "Core/Graphics/VulkanRHI/VulkanSampler.h"
#include "Core/Graphics/VulkanRHI/VulkanDevice.h"
#include "Core/Graphics/RHI/IDevice.h"
#include <stb/stb_image.h>

namespace Iris
{
	namespace Engine
	{
		void Texture::LoadCompiledData(LoadedData* _loadedData, ResourceContext* _resourceContext, const LoadingParameters& _loadingParameters)
		{
			TextureLoadedData* loadedData = dynamic_cast<TextureLoadedData*>(_loadedData);

			const TextureLoadingParameters* textureParams = dynamic_cast<const TextureLoadingParameters*>(&_loadingParameters);
			TextureLoadingParameters params = TextureLoadingParameters();

			if (textureParams)
			{
				params = *textureParams;
			}

			if (textureParams && !textureParams->LoadInGraphicsData)
			{
				m_Data = loadedData->Data;
			}
			else
			{

				unsigned int mips = static_cast<uint32_t>(std::floor(std::log2((loadedData->Width > loadedData->Height) ? loadedData->Width : loadedData->Height))) + 1;

				RHIImage info{};
				{
					info.ColorFormat = params.ColorFormat;
					info.Type = params.TextureType;
					info.MipCount = static_cast<uint32_t>(mips);
				}

				m_TextureImage = _resourceContext->CreateImage(loadedData->Width, loadedData->Height, loadedData->ChannelCount, loadedData->Data, info);

				RHISampler samplerInfo{};
				{
					samplerInfo.MagFilter = RHISamplerFilter::IE_RHI_LINEAR;
					samplerInfo.MinFilter = RHISamplerFilter::IE_RHI_LINEAR;
					samplerInfo.ModeU = RHISamplerAdress::IE_RHI_REPEAT;
					samplerInfo.ModeV = RHISamplerAdress::IE_RHI_REPEAT;
					samplerInfo.ModeW = RHISamplerAdress::IE_RHI_REPEAT;
					samplerInfo.BorderColor = RHISamplerBorderColor::IE_RHI_BORDER_COLOR_BLACK_INT;
				}
				
				m_TextureSampler = _resourceContext->CreateSampler(mips, samplerInfo);
			}

			m_Width = loadedData->Width;
			m_Height = loadedData->Height;
			m_IsHDR = loadedData->IsHDR;
		}

		void Texture::CreateTexture(IImage* _image, ResourceContext* _resourceContext)
		{
			m_TextureImage = _image;

			RHISampler samplerInfo{};
			{
				samplerInfo.MagFilter = RHISamplerFilter::IE_RHI_LINEAR;
				samplerInfo.MinFilter = RHISamplerFilter::IE_RHI_LINEAR;
				samplerInfo.ModeU = RHISamplerAdress::IE_RHI_CLAMP_TO_EDGE;
				samplerInfo.ModeV = RHISamplerAdress::IE_RHI_CLAMP_TO_EDGE;
				samplerInfo.ModeW = RHISamplerAdress::IE_RHI_CLAMP_TO_EDGE;
				samplerInfo.BorderColor = RHISamplerBorderColor::IE_RHI_BORDER_COLOR_BLACK_INT;
			}

			m_TextureSampler = _resourceContext->CreateSampler(m_TextureImage->GetMipCount(), samplerInfo);
		}

		void Texture::UnloadCompiledData(ResourceContext* _resourceContext)
		{
			_resourceContext->DestroyImage(m_TextureImage);
			_resourceContext->DestroySampler(m_TextureSampler);
		}
	}
}