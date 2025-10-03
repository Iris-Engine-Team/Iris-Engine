#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"
#include "Core/Graphics//RHI/CommonTypes.h"


#include <vector>

namespace Iris
{
	namespace Engine
	{
		struct InspectedDescriptorBinding
		{
			uint32_t Binding = 0;
			RHIDescriptorType Type = RHIDescriptorType::IE_RHI_DESCRIPTOR_TYPE_SAMPLER;
			uint32_t DescriptorCount = 0;
			RHIShaderType StageFlags = RHIShaderType::IE_RHI_SHADER_TYPE_VERTEX;
		};

		struct InspectedDescriptor
		{
			std::vector<InspectedDescriptorBinding> DescriptorBindings{};
		};

		struct InspectedBlockVariableMember
		{
			std::string Name = "";
			uint32_t Offset = 0;
			uint32_t Size = 0;
		};

		struct InspectedBlockVariable
		{
			std::string Name = "";
			uint32_t Offset = 0;
			uint32_t Size = 0;
			RHIShaderType StageFlags = RHIShaderType::IE_RHI_SHADER_TYPE_VERTEX;
			std::vector<InspectedBlockVariableMember> Members{};
		};

		struct InspectedShader
		{
			std::vector<InspectedDescriptor> Descriptors{};
			std::vector<InspectedBlockVariable> BlockVariable{};
		};

		class IShaderInspector
		{
		public:
			virtual bool InspectShader(std::vector<uint32_t>& _shaderCode, RHIType _shaderAPIType, RHIShaderType _shaderType, InspectedShader* _inspectedShaderResult) = 0;
		
			virtual ~IShaderInspector() = default;
		};
	}
}