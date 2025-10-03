#pragma once
#include "Loader/ILoader.h"
#include "Core/Graphics/RHI/CommonTypes.h"
	
namespace Iris
{
	namespace Engine
	{
		struct TextureLoadingParameters : public LoadingParameters
		{
			TextureLoadingParameters() = default;
			~TextureLoadingParameters() override final = default;
			bool LoadInGraphicsData = true;
			RHIColorFormat ColorFormat = RHIColorFormat::IE_RHI_FORMAT_R8G8B8A8_UNORM;
			RHITextureType TextureType = RHITextureType::IE_RHI_TEXTURE_2D;
		};

		class ITextureLoader : public ILoader
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_path"></param>
			IRIS_ENGINE_API	virtual bool Load(LoadedData* _data, std::filesystem::path _path, const LoadingParameters& _parameters) override = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_path"></param>
			IRIS_ENGINE_API	virtual void Unload(LoadedData* _data) override = 0;

			virtual ~ITextureLoader() = default;
		};
	}
}