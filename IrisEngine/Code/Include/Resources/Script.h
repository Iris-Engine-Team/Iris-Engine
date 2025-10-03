#pragma once

#include "Resources/IResource.h"

namespace Iris
{
	namespace Engine
	{
		class IDescriptorLayout;
		class ResourceContext;
		struct LoadingParameters;

		class Script : public IResource
		{
		public:
			~Script() override = default;

			IRIS_ENGINE_API IE_FORCE_INLINE void LoadCompiledData(LoadedData* _data, ResourceContext* _resourceContext, const LoadingParameters& _loadingParameters) override final
			{ _data; _resourceContext; _loadingParameters; }
			IRIS_ENGINE_API IE_FORCE_INLINE void UnloadCompiledData(ResourceContext* _resourceContext) override final { _resourceContext; }
		};
	}
}