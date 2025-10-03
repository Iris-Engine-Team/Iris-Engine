#pragma once

#include "Resources/IResource.h"
#include "Core/Graphics/VulkanRHI/VulkanDescriptor.h"

namespace Iris
{
	namespace Engine
	{
		class ISampler;
		class IImage;
		class IDescriptor;
		struct LoadingParameters;

		struct TextureLoadedData : public LoadedData
		{
			void* Data = nullptr;
			bool IsHDR = false;
			int Width = 0, Height = 0, ChannelCount = 0;
		};

		class Texture : public IResource
		{
		public:
			~Texture() override = default;
			IRIS_ENGINE_API void LoadCompiledData(LoadedData* _data, ResourceContext* _resourceContext, const LoadingParameters& _loadingParameters) override final;
			IRIS_ENGINE_API void CreateTexture(IImage* _image, ResourceContext* _resourceContext);
			IRIS_ENGINE_API void UnloadCompiledData(ResourceContext* _resourceContext) override final;

			IRIS_ENGINE_API IE_FORCE_INLINE void SetSampler(ISampler* _sampler) { m_TextureSampler = _sampler; }
			IRIS_ENGINE_API IE_FORCE_INLINE ISampler* GetSampler() const { return m_TextureSampler; }

			IRIS_ENGINE_API IE_FORCE_INLINE void SetImage(IImage* _image) { m_TextureImage = _image; }
			IRIS_ENGINE_API IE_FORCE_INLINE IImage* GetImage() const { return m_TextureImage; }

			IRIS_ENGINE_API IE_FORCE_INLINE bool IsHDR() const { return m_IsHDR; }
			IRIS_ENGINE_API IE_FORCE_INLINE void* GetData() const { return m_Data; }

			IRIS_ENGINE_API IE_FORCE_INLINE unsigned int GetWidth() const { return m_Width; }
			IRIS_ENGINE_API IE_FORCE_INLINE unsigned int GetHeight() const { return m_Height; }

			IRIS_ENGINE_API IE_FORCE_INLINE unsigned long long GetID() const { return m_Id; }
			IRIS_ENGINE_API IE_FORCE_INLINE void SetID(unsigned long long _id) { m_Id = _id; }

		private:
			ISampler* m_TextureSampler = nullptr;
			IImage* m_TextureImage = nullptr;

			unsigned long long m_Id = 0;

			void* m_Data = nullptr;
			bool m_IsHDR = false;
			unsigned int m_Width = 0;
			unsigned int m_Height = 0;
		};
	}
}