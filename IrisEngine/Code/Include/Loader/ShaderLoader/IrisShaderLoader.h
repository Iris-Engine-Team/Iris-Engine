#pragma once

#include "Loader/ShaderLoader/IShaderLoader.h"

#include <string>
#include <vector>

namespace Iris
{
	namespace Engine
	{
		class IrisShaderLoader : public IShaderLoader
		{
		public:

			~IrisShaderLoader() override = default;

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

			IRIS_ENGINE_API IE_FORCE_INLINE std::vector<uint32_t> GetCompiledShader() { return m_SourceCode; }

		private:
			std::vector<uint32_t> m_SourceCode;
		};
	}
}