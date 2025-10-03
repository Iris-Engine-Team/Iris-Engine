#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"
#include "Core/Graphics/Context/ResourceContext.h"

#include <filesystem>
#include <string>

namespace Iris
{
	namespace Engine
	{
		struct LoadedData
		{
			virtual ~LoadedData() = default;
		};

		struct LoadingParameters;

		class IResource
		{
		public:
			virtual ~IResource() = default;

			IRIS_ENGINE_API virtual void LoadCompiledData(LoadedData* _data, ResourceContext* _resourceContext, const LoadingParameters& _loadingParameters) = 0;
			IRIS_ENGINE_API virtual void UnloadCompiledData(ResourceContext* _resourceContext) = 0;

			IRIS_ENGINE_API IE_FORCE_INLINE void SetName(std::string& _name) { p_ResourceName = _name; }
			IRIS_ENGINE_API IE_FORCE_INLINE const std::string& GetName() const { return p_ResourceName; }

		protected:
			std::string p_ResourceName = "";
		};
	}
}