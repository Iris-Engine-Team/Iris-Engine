#include "Loader/IrisShaderTools.h"
#include "Loader/ShaderLoader/IrisShaderLoader.h"
#include "Loader/ShaderCompiler/ShadercShaderCompiler.h"
#include "Loader/ShaderReflection/SPIRVShaderInspector.h"

namespace Iris
{
	namespace Engine
	{
		void Iris::Engine::IrisShaderTools::Init(ShaderLoaderType _shaderLoader, ShaderCompilerType _shaderCompiler, ShaderInspectorType _shaderInspector)
		{
			switch (_shaderLoader)
			{
			case Iris::Engine::ShaderLoaderType::IE_SHADER_LOADER_TYPE_IRIS:
				DEBUG_LOG("Shader loader of type %i created.", 0);
				m_Loader = new IrisShaderLoader();
				break;
			default:
				DEBUG_WARN("Can't create shader loader of type %i.", static_cast<int>(_shaderLoader));
				break;
			}

			switch (_shaderCompiler)
			{
			case Iris::Engine::ShaderCompilerType::IE_SHADER_COMPILER_TYPE_SHADERC:
				DEBUG_LOG("Shader compiler of type %i created.", 0);
				m_Compiler = new ShadercShaderCompiler();
				break;
			default:
				DEBUG_WARN("Can't create shader compiler of type %i.", static_cast<int>(_shaderCompiler));
				break;
			}

			switch (_shaderInspector)
			{
			case Iris::Engine::ShaderInspectorType::IE_SHADER_INSPECTOR_TYPE_SPIRV:
				DEBUG_LOG("Shader loader of type %i created.", 0);
				m_Inspector = new SPIRVShaderInspector();
				break;
			default:
				DEBUG_WARN("Can't create shader inspector of type %i.", static_cast<int>(_shaderInspector));
				break;
			}
		}

		void Iris::Engine::IrisShaderTools::Terminate()
		{
			if (m_Loader)
			{
				DEBUG_LOG("Shader loader deleted.");
				delete m_Loader;
				m_Loader = nullptr;
			}

			if (m_Compiler)
			{
				DEBUG_LOG("Shader compiler deleted.");
				delete m_Compiler;
				m_Compiler = nullptr;
			}

			if (m_Inspector)
			{
				DEBUG_LOG("Shader inspector deleted.");
				delete m_Inspector;
				m_Inspector = nullptr;
			}
		}

		bool IrisShaderTools::Load(LoadedData* _data, std::filesystem::path _path, const LoadingParameters& _parameters)
		{
			bool resultLoader = m_Loader->Load(_data, _path, _parameters);
			bool resultCompiler = m_Compiler->CompileShaderCode(dynamic_cast<ShaderLoadedData*>(_data));

			return resultLoader && resultCompiler;
		}

		void IrisShaderTools::Unload(LoadedData* _data)
		{
			_data;
		}
	}
}