#include "Loader/ShaderLoader/IrisShaderLoader.h"

#include "Core/Graphics/RHI/CommonTypes.h"
#include "Resources/Shader.h"


namespace Iris
{
	namespace Engine
	{
		bool IrisShaderLoader::Load(LoadedData* _data, std::filesystem::path _path, const LoadingParameters& _parameters)
		{
			_parameters;
			ShaderLoadedData* loadedData = dynamic_cast<ShaderLoadedData*>(_data);

			if (!IsExtensionValid(_path.extension()))
			{
				DEBUG_WARN("Can't open shader file, wrong extension!");
				return false;
			}

			std::ifstream file(_path, std::ios::binary | std::ios::ate);

			if (!file.is_open())
			{
				DEBUG_WARN("Failed to open shader file: %s", _path.string().c_str());
				return false;
			}

			if (_path.extension() == ".vert")
				loadedData->ShaderType = RHIShaderType::IE_RHI_SHADER_TYPE_VERTEX;
			else if (_path.extension() == ".frag")
				loadedData->ShaderType = RHIShaderType::IE_RHI_SHADER_TYPE_FRAGMENT;
			else if (_path.extension() == ".geo")
				loadedData->ShaderType = RHIShaderType::IE_RHI_SHADER_TYPE_GEOMETRY;

			std::vector<char> sourceCode;

			size_t fileSize = (size_t)file.tellg();
			sourceCode.resize(fileSize);

			file.seekg(0, std::ios::beg);
			file.read(sourceCode.data(), fileSize);

			file.close();

			loadedData->Filename = _path.filename().string();
			loadedData->SourceCode = std::string(sourceCode.begin(), sourceCode.end());

			return true;
		}

		void IrisShaderLoader::Unload(LoadedData* _data)
		{
			_data;
		}
	}
}