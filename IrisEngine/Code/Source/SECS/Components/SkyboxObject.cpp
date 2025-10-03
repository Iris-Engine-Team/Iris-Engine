#include "SECS/Components/SkyboxObject.h"
#include "Core/Graphics/RHI/IDescriptor.h"
#include "Resources/Texture.h"
#include "Core/Graphics/Context/ResourceContext.h"
#include "Core/Graphics/Renderer/IRenderer.h"

#include "Resources/ResourceManager.h"
#include "Core/Tools/FilePath.h"


//To remove
#include "Core/Graphics/VulkanRHI/VulkanImage.h"
#include "Core/Graphics/RHI/IPipeline.h"

namespace Iris
{
	namespace Engine
	{
		void SkyboxObject::Init(ContextInitializer* _contextInitializer, IRenderer* _render)
		{
			m_ContextInitializer = _contextInitializer;
			m_Renderer = _render;
		}

		void SkyboxObject::Destroy()
		{
			ResourceContext* rc = m_ContextInitializer->GetRawResourceContext();
			m_IBLIrradiance.UnloadCompiledData(rc);
			m_IBLPrefiltered.UnloadCompiledData(rc);
			m_IBLLookUp.UnloadCompiledData(rc);

			m_SkyboxTexture.UnloadCompiledData(rc);
			rc->DeleteDescriptor(m_Descriptor);
			rc->DeleteDescriptor(m_IBLDescriptor);
		}


		void SkyboxObject::SetTexture(std::string _textureName)
		{
			if (_textureName == m_SkyboxTexture.GetName())
				return;

			if (m_Descriptor)
			{
				Destroy();
			}

			TextureLoadingParameters param2{};
			param2.ColorFormat = RHIColorFormat::IE_RHI_FORMAT_R32G32B32A32_SFLOAT;
			Texture* panoramic = ResourceManager::GetInstance()->Create<Texture>(FIND_PATH(_textureName), true, param2);
			IImage* newImage = new VulkanImage();

			RHIImage imageinfo{};
			imageinfo.ColorFormat = RHIColorFormat::IE_RHI_FORMAT_R32G32B32A32_SFLOAT;
			imageinfo.Type = RHITextureType::IE_RHI_TEXTURE_CUBEMAP;
			imageinfo.Usage = RHIAttachmentType::IE_RHI_COLOR_ATTACHMENT;

			newImage->CreateFramebufferImage(m_ContextInitializer->GetRawRenderingContext()->GetDevice(), m_ContextInitializer->GetRawRenderingContext()->GetCommandAllocator(), 1024, 1024, imageinfo);

			m_Renderer->ConvertPanoramicTextureToCubemap(panoramic, newImage);
			
			std::string name = panoramic->GetName();
			m_SkyboxTexture.SetName(name);

			ResourceContext* rc = m_ContextInitializer->GetRawResourceContext();
			m_SkyboxTexture.CreateTexture(newImage, rc);
			m_Descriptor = rc->CreateTextureDescriptor(m_Renderer->GetSkybox()->GetDescriptorLayout(1), &m_SkyboxTexture);

			IImage* IBLEnvironment = new VulkanImage();

			RHIImage IBLEimageinfo{};
			IBLEimageinfo.ColorFormat = RHIColorFormat::IE_RHI_FORMAT_R32G32B32A32_SFLOAT;
			IBLEimageinfo.Type = RHITextureType::IE_RHI_TEXTURE_CUBEMAP;
			IBLEimageinfo.Usage = RHIAttachmentType::IE_RHI_COLOR_ATTACHMENT;

			IBLEnvironment->CreateFramebufferImage(m_ContextInitializer->GetRawRenderingContext()->GetDevice(), m_ContextInitializer->GetRawRenderingContext()->GetCommandAllocator(), 1024, 1024, IBLEimageinfo);

			m_Renderer->ComputeIBLEnvironmentdMap(&m_SkyboxTexture, IBLEnvironment);

			m_IBLIrradiance.CreateTexture(IBLEnvironment, m_ContextInitializer->GetRawResourceContext());

			RHIImage IBLPimageinfo{};
			IBLPimageinfo.ColorFormat = RHIColorFormat::IE_RHI_FORMAT_R32G32B32A32_SFLOAT;
			IBLPimageinfo.Type = RHITextureType::IE_RHI_TEXTURE_CUBEMAP;
			IBLPimageinfo.Usage = RHIAttachmentType::IE_RHI_COLOR_ATTACHMENT;
			IBLPimageinfo.MipCount = 5;

			IImage* IBLPRefilteredImg = new VulkanImage();

			IBLPRefilteredImg->CreateFramebufferImage(m_ContextInitializer->GetRawRenderingContext()->GetDevice(), m_ContextInitializer->GetRawRenderingContext()->GetCommandAllocator(), 128, 128, IBLPimageinfo);

			m_Renderer->ComputeIBLPrefilteredMap(&m_SkyboxTexture, IBLPRefilteredImg);

			m_IBLPrefiltered.CreateTexture(IBLPRefilteredImg, m_ContextInitializer->GetRawResourceContext());

			IImage* lookUpTexture = new VulkanImage();

			RHIImage IBLLookUpInfo{};
			IBLLookUpInfo.ColorFormat = RHIColorFormat::IE_RHI_FORMAT_R32G32B32A32_SFLOAT;
			IBLLookUpInfo.Type = RHITextureType::IE_RHI_TEXTURE_2D;
			IBLLookUpInfo.Usage = RHIAttachmentType::IE_RHI_COLOR_ATTACHMENT;

			lookUpTexture->CreateFramebufferImage(m_ContextInitializer->GetRawRenderingContext()->GetDevice(), m_ContextInitializer->GetRawRenderingContext()->GetCommandAllocator(), 512, 512, IBLLookUpInfo);

			m_Renderer->ComputeIBLLookUpTexture(lookUpTexture);

			m_IBLLookUp.CreateTexture(lookUpTexture, m_ContextInitializer->GetRawResourceContext());

			m_IBLDescriptor = m_ContextInitializer->GetRawResourceContext()->CreateIBLDescriptor(m_Renderer->GetLightPipeline()->GetDescriptorLayout(m_Renderer->GetIBLLayoutIndex()), &m_IBLIrradiance, &m_IBLPrefiltered, &m_IBLLookUp);
		}
	}
}