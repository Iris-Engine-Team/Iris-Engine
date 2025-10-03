#pragma once

#include "Loader/TextureLoader/ITextureLoader.h"

namespace Iris
{
	namespace Engine
	{
		class StbImageLoader : public ITextureLoader
		{
		public:

			~StbImageLoader() override = default;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_path"></param>
			IRIS_ENGINE_API bool Load(LoadedData* _data, std::filesystem::path _path, const LoadingParameters& _parameters) override final;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_path"></param>
			IRIS_ENGINE_API void Unload(LoadedData* _data) override final;
		};
	}
}