#pragma once

#include "Loader/ShaderCompiler/IShaderCompiler.h"

#include <shaderc/shaderc.hpp>

namespace Iris
{
	namespace Engine
	{
		struct CompilationInfo
		{
			std::string fileName = "";
			shaderc_shader_kind shaderKind = shaderc_shader_kind::shaderc_vertex_shader;
			std::string* sourceCode = nullptr;
			shaderc::CompileOptions options;
		};

		class ShadercShaderCompiler : public IShaderCompiler
		{
		public:

			~ShadercShaderCompiler() override = default;

			/// <summary>
			/// Compiles a shader code into binary
			/// </summary>
			/// <param name="_filename">: Name of the shader</param>
			/// <param name="_shaderType">: Type of the shader (VERTEX, FRAGMENT, GEOMETRY)</param>
			/// <param name="_sourceCode">: Base source code in ASCII</param>
			/// <param name="_result">: Result in binary that will be filled</param>
			/// <returns>A bool, true if the compilation succeeded, false if not</returns>
			bool CompileShaderCode(ShaderLoadedData* _data) override final;

		private:
			/// <summary>
			/// Preprocess the shader
			/// </summary>
			/// <param name="_Compiler">: Shaderc compiler instance</param>
			/// <param name="_Infos">: A structure containing all information for Shaderc</param>
			/// <returns>A bool, true if the operation succeeded, false if not</returns>
			bool PreprocessShader(shaderc::Compiler& _Compiler, CompilationInfo& _Infos);

			/// <summary>
			/// Compiles the shader from ASCII to SPIRV assembly
			/// </summary>
			/// <param name="_Compiler">: Shaderc compiler instance</param>
			/// <param name="_Infos">: A structure containing all information for Shaderc</param>
			/// <returns>A bool, true if the operation succeeded, false if not</returns>
			bool SPIRVAssemblyCompilation(shaderc::Compiler& _Compiler, CompilationInfo& _Infos);

			/// <summary>
			/// Compiles the shader from SPIRV assembly to SPIRV binary
			/// </summary>
			/// <param name="_Compiler">: Shaderc compiler instance</param>
			/// <param name="_Infos">: A structure containing all information for Shaderc</param>
			/// <param name="_result">: Result in binary that will be filled</param>
			/// <returns>A bool, true if the operation succeeded, false if not</returns>
			bool SPIRVBinaryCompilation(shaderc::Compiler& _Compiler, CompilationInfo& _Infos, std::vector<uint32_t>* _result);
		};
	}
}