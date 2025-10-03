#include "Loader/ShaderCompiler/ShadercIncluder.h"

#include <filesystem>
#include "Core/Tools/FilePath.h"

#include <array>

namespace Iris
{
	namespace Engine
	{
		shaderc_include_result* ShadercIncluder::GetInclude(const char* _requestedSource, shaderc_include_type _type, const char* _requestingSource, size_t _includeDepth)
		{
			_includeDepth;
			_type;

			std::string debugMsg = _requestedSource;
			debugMsg += " requested in shader ";
			debugMsg += _requestingSource;

			std::filesystem::path path = _requestedSource;
			const std::string name = path.filename().string();

			const std::string content = ReadFile(name, debugMsg);

			std::array<std::string, 2>* container = new std::array<std::string, 2>;
			(*container)[0] = name;
			(*container)[1] = content;

			shaderc_include_result* includedData = CreateData(container);

			return includedData;
		}

		void ShadercIncluder::ReleaseInclude(shaderc_include_result* _result)
		{
			delete static_cast<std::array<std::string, 2>*>(_result->user_data);
			delete _result;
		}

		shaderc_include_result* ShadercIncluder::CreateData(std::array<std::string, 2>* _container)
		{
			shaderc_include_result* includedData = new shaderc_include_result;

			includedData->user_data				= _container;

			includedData->source_name			= (*_container)[0].data();
			includedData->source_name_length	= (*_container)[0].size();

			includedData->content				= (*_container)[1].data();
			includedData->content_length		= (*_container)[1].size();

			return includedData;
		}

		std::string ShadercIncluder::ReadFile(const std::string& _name, std::string& _debugMsg)
		{
			std::filesystem::path path = FIND_PATH(_name);

			std::string sourceCode = "";
			std::ifstream file(path, std::ios::in | std::ios::binary);

			if (!file.is_open())
			{
				std::string warn = "Failed to include shader " + _debugMsg;
				warn;
				//DEBUG_WARN("%s\n", warn);
				return "";
			}

			file.seekg(0, std::ios::end);
			size_t size = file.tellg();

			if (size <= 0)
			{
				std::string warn = "Failed to read shader include " + _debugMsg;
				warn;
				//DEBUG_WARN("%s\n", warn);
				return "";
			}

			sourceCode.resize(size);
			file.seekg(0, std::ios::beg);
			file.read(&sourceCode[0], size);

			return sourceCode;
		}
	}
}