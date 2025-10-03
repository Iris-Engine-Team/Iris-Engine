#pragma once

#include "Loader/ILoader.h"

namespace Iris
{
	namespace Engine
	{
		struct MeshData;

		struct MeshLoadingParameters : public LoadingParameters
		{
			MeshLoadingParameters() = default;
			~MeshLoadingParameters() override final = default;
		};

		class IMeshLoader : public ILoader
		{
		public:
			virtual ~IMeshLoader() = default;

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

		protected:
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_meshData"></param>
			/// <returns></returns>
			virtual void ProcessTangentBitangent(MeshData& _meshData);
		};
	}
}