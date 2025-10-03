#include "Resources/Shader.h"

#include "Core/Graphics/Context/ResourceContext.h"
#include "Loader/ShaderLoader/IrisShaderLoader.h"
#include "Core/Graphics/RHI/IDescriptor.h"

#include <spirv_reflect.h>

namespace Iris
{
	namespace Engine
	{
		void Shader::LoadCompiledData(LoadedData* _data, ResourceContext* _resourceContext, const LoadingParameters& _loadingParameters)
		{
			_loadingParameters;
			ShaderLoadedData* shaderData = dynamic_cast<ShaderLoadedData*>(_data);
			m_ShaderModule = _resourceContext->CreateShaderModule(shaderData->Data);
			m_Data = shaderData->Data;
			m_ShaderType = shaderData->ShaderType;
		}

		void Shader::UnloadCompiledData(ResourceContext* _resourceContext)
		{
			_resourceContext->DestroyShaderModule(m_ShaderModule);
		}
	}
}