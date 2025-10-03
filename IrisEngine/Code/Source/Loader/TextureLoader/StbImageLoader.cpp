#include "Loader/TextureLoader/StbImageLoader.h"

#include "Resources/Texture.h"
#include "Core/Tools/Log.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace Iris
{
	namespace Engine
	{
		bool StbImageLoader::Load(LoadedData* _data, std::filesystem::path _path, const LoadingParameters& _parameters)
		{
			_parameters;

			TextureLoadedData* loadedData = dynamic_cast<TextureLoadedData*>(_data);

			stbi_set_flip_vertically_on_load(true);

			if (_path.extension() == ".hdr")
			{
				loadedData->Data = stbi_loadf(_path.string().c_str(), &(loadedData->Width), &(loadedData->Height), &(loadedData->ChannelCount), STBI_rgb_alpha);
				loadedData->IsHDR = true;
			}
			else
			{
				loadedData->Data = stbi_load(_path.string().c_str(), &(loadedData->Width), &(loadedData->Height), &(loadedData->ChannelCount), STBI_rgb_alpha);
			}
			

			if (!loadedData->Data)
			{
				DEBUG_WARN("Can't load %s data", _path.filename().c_str());
				return false;
			}

			return true;
		}

		void StbImageLoader::Unload(LoadedData* _data)
		{
			TextureLoadedData* loadedData = dynamic_cast<TextureLoadedData*>(_data);

			stbi_image_free(loadedData->Data);
		}
	}
}