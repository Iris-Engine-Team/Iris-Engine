#pragma once

#include "Resources/IResource.h"

#include <string>

namespace Iris
{
	namespace Engine
	{
		struct LoadingParameters;

		struct ContainerData
		{
			std::string ContainerName = "";
			virtual ~ContainerData() = default;
		};

		class IResourceContainer : public IResource
		{
		public:
			virtual ~IResourceContainer() override = default;

			virtual void LoadCompiledData(LoadedData* _data, ResourceContext* _resourceContext, const LoadingParameters& _loadingParameters) override final { _data;  _resourceContext; _loadingParameters; }
			virtual void UnloadCompiledData(ResourceContext* _resourceContext) = 0;

			virtual void CreateContainer(ContainerData* _resourceData, const ResourceContext* _resourceContext) { p_ResourceName = _resourceData->ContainerName; _resourceContext; }
		};
	}
}