#pragma once

#include "Resources/IResource.h"

#include <vector>

namespace Iris
{
	namespace Engine
	{
		class IDescriptorLayout;
		class IShaderModule;
		class ResourceContext;
		struct LoadingParameters;

		struct ShaderLoadedData : public LoadedData
		{
			std::string Filename{};
			RHIShaderType ShaderType{};
			std::string SourceCode{};
			std::vector<uint32_t> Data{};
		};

		class Shader : public IResource
		{
		public:
			~Shader() override = default;

			IRIS_ENGINE_API IE_FORCE_INLINE IShaderModule* GetShaderModule() const { return m_ShaderModule; }
			IRIS_ENGINE_API IE_FORCE_INLINE std::vector<uint32_t> GetShaderCode() const { return m_Data; }
			IRIS_ENGINE_API IE_FORCE_INLINE RHIShaderType GetType() const { return m_ShaderType; }

			IRIS_ENGINE_API void LoadCompiledData(LoadedData* _data, ResourceContext* _resourceContext, const LoadingParameters& _loadingParameters) override final;
			IRIS_ENGINE_API void ReflectShader(ResourceContext* _resourceContext);
			IRIS_ENGINE_API void UnloadCompiledData(ResourceContext* _resourceContext) override final;

		private:
			IShaderModule* m_ShaderModule = nullptr;
			std::vector<uint32_t> m_Data;
			RHIShaderType m_ShaderType = RHIShaderType::IE_RHI_SHADER_TYPE_VERTEX;
		};
	}
}