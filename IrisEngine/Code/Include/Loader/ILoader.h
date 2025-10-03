#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"

#include <filesystem>

namespace Iris
{
	namespace Engine
	{
		class IResource;
		struct LoadedData;

		struct LoadingParameters
		{
			virtual ~LoadingParameters() = default;
		};

		class ILoader
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_path"></param>
			IRIS_ENGINE_API	virtual bool Load(LoadedData* _data, std::filesystem::path _path, const LoadingParameters& _parameters) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_path"></param>
			IRIS_ENGINE_API virtual void Unload(LoadedData* _data) = 0;

			virtual ~ILoader() = default;
		};
	}
}