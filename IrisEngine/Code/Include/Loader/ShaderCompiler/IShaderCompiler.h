#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"

#include <string>
#include "Core/Graphics/RHI/CommonTypes.h"
#include "Resources/Shader.h"

namespace Iris
{
	namespace Engine
	{
		class IShaderCompiler
		{
		public:
			/// <summary>
			/// Compiles a shader code into binary
			/// </summary>
			/// <param name="_filename">: Name of the shader</param>
			/// <param name="_shaderType">: Type of the shader (VERTEX, FRAGMENT, GEOMETRY)</param>
			/// <param name="_sourceCode">: Base source code in ASCII</param>
			/// <param name="_result">: Result in binary that will be filled</param>
			/// <returns>A bool, true if the compilation succeeded, false if not</returns>
			virtual bool CompileShaderCode(ShaderLoadedData* _data) = 0;

			virtual ~IShaderCompiler() = default;
		};
	}
}