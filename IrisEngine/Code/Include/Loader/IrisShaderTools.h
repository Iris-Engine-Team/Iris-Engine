#pragma once

#include "Loader/ILoader.h"
#include "Loader/ShaderLoader/IShaderLoader.h"
#include "Loader/ShaderCompiler/IShaderCompiler.h"
#include "Loader/ShaderReflection/IShaderInspector.h"

#include <typeinfo>
#include <typeindex>

namespace Iris
{
	namespace Engine
	{
		enum class ShaderLoaderType : int
		{
			IE_SHADER_LOADER_TYPE_IRIS = 0
		};

		enum class ShaderCompilerType : int
		{
			IE_SHADER_COMPILER_TYPE_SHADERC = 0
		};

		enum class ShaderInspectorType : int
		{
			IE_SHADER_INSPECTOR_TYPE_SPIRV = 0
		};


		class IrisShaderTools : public ILoader
		{
		public:
			void Init(ShaderLoaderType _shaderLoader, ShaderCompilerType _shaderCompiler, ShaderInspectorType _shaderInspector);
			void Terminate();

			IRIS_ENGINE_API IE_FORCE_INLINE IShaderLoader* GetLoader() { return m_Loader; }
			IRIS_ENGINE_API IE_FORCE_INLINE IShaderCompiler* GetCompiler() { return m_Compiler; }
			IRIS_ENGINE_API IE_FORCE_INLINE IShaderInspector* GetInspector() { return m_Inspector; }

			template<typename T>
			ILoader* GetSubPart()
			{
				std::type_index type = typeid(T);

				if (type == typeid(IShaderLoader)) 
					return m_Loader;
				
				return this;
			}


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


		private:
			IShaderLoader* m_Loader = nullptr;
			IShaderCompiler* m_Compiler = nullptr;
			IShaderInspector* m_Inspector = nullptr;
		};
	}
}