#include "Loader/ShaderCompiler/ShadercShaderCompiler.h"
#include "Loader/ShaderCompiler/ShadercIncluder.h"

namespace Iris
{
	namespace Engine
	{
		bool ShadercShaderCompiler::CompileShaderCode(ShaderLoadedData* _data)
		{
			shaderc::Compiler compiler;

			CompilationInfo info{};
			info.fileName = _data->Filename;
			info.sourceCode = &(_data->SourceCode);

			switch (_data->ShaderType)
			{
			case RHIShaderType::IE_RHI_SHADER_TYPE_VERTEX: default:
				info.shaderKind = shaderc_vertex_shader;
				break;
			case RHIShaderType::IE_RHI_SHADER_TYPE_FRAGMENT:
				info.shaderKind = shaderc_fragment_shader;
				break;
			case RHIShaderType::IE_RHI_SHADER_TYPE_GEOMETRY:
				info.shaderKind = shaderc_geometry_shader;
				break;
			}

			info.options.SetIncluder(std::make_unique<ShadercIncluder>());
			info.options.SetSourceLanguage(shaderc_source_language_glsl);
			info.options.SetOptimizationLevel(shaderc_optimization_level_performance);
			info.options.SetGenerateDebugInfo();

			if (!PreprocessShader(compiler, info))
				return false;

			if (!SPIRVAssemblyCompilation(compiler, info))
				return false;

			if(!SPIRVBinaryCompilation(compiler, info, &(_data->Data)))
				return false;

			if (_data->Data.empty())
				return false;

			return true;
		}

		bool ShadercShaderCompiler::PreprocessShader(shaderc::Compiler& _Compiler, CompilationInfo& _Infos)
		{
			// First step - Preprocessing GLSL
			shaderc::PreprocessedSourceCompilationResult result = _Compiler.PreprocessGlsl(_Infos.sourceCode->data(), _Infos.shaderKind, _Infos.fileName.c_str(), _Infos.options);

			if (result.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				DEBUG_ERROR("Failed to preprocessed shader, %s", result.GetErrorMessage().c_str());
				return false;
			}

			// Copy the precompiled code in the buffer
			const char* src = result.cbegin();
			size_t newSize = result.cend() - src;
			_Infos.sourceCode->resize(newSize);
			memcpy(_Infos.sourceCode->data(), src, newSize);

			return true;
		}

		bool ShadercShaderCompiler::SPIRVAssemblyCompilation(shaderc::Compiler& _Compiler, CompilationInfo& _Infos)
		{
			// Second step - SPIR-V Assembly compilation
			shaderc::AssemblyCompilationResult sResult = _Compiler.CompileGlslToSpvAssembly(_Infos.sourceCode->data(), _Infos.shaderKind, _Infos.fileName.c_str(), _Infos.options);

			if (sResult.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				DEBUG_ERROR("Failed to compiled in assembly shader, %s", sResult.GetErrorMessage().c_str());
				return false;
			}

			// Copy the SPRIV Assembly code in the buffer
			const char* src = sResult.cbegin();
			size_t newSize = sResult.cend() - src;
			_Infos.sourceCode->resize(newSize);
			memcpy(_Infos.sourceCode->data(), src, newSize);

			return true;
		}

		bool ShadercShaderCompiler::SPIRVBinaryCompilation(shaderc::Compiler& _Compiler, CompilationInfo& _Infos, std::vector<uint32_t>* _result)
		{
			// Third step - SPIR-V Binary compilation
			shaderc::SpvCompilationResult tResult = _Compiler.AssembleToSpv(_Infos.sourceCode->data(), _Infos.sourceCode->size(), _Infos.options);

			if (tResult.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				DEBUG_ERROR("Failed to compiled in binary shader, %s", tResult.GetErrorMessage().c_str());
				return false;
			}

			// Copy the SPIRV Binary code in the output buffer
			const uint32_t* srcBinary = tResult.cbegin();
			size_t wordCount = tResult.cend() - srcBinary;
			_result->resize(wordCount);
			memcpy(_result->data(), srcBinary, wordCount * sizeof(uint32_t));

			return true;
		}
	}
}