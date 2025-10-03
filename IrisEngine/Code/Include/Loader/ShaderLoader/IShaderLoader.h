#pragma once

#include "Loader/ILoader.h"

namespace Iris
{
	namespace Engine
	{
		struct ShaderLoadingParameters : public LoadingParameters
		{
			ShaderLoadingParameters() = default;
			~ShaderLoadingParameters() override final = default;
		};

		class IShaderLoader : public ILoader
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_path"></param>
			virtual bool Load(LoadedData* _data, std::filesystem::path _path, const LoadingParameters& _parameters) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_path"></param>
			virtual void Unload(LoadedData* _data) = 0;

			virtual ~IShaderLoader() = default;

		protected:
			virtual bool IsExtensionValid(std::filesystem::path _extension) const;
		};
	}
}