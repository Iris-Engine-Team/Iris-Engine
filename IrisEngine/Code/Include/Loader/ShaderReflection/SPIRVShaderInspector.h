#pragma once

#include "Loader/ShaderReflection/IShaderInspector.h"
#include <spirv_reflect.h>

namespace Iris
{
	namespace Engine
	{
		class SPIRVShaderInspector : public IShaderInspector
		{
		public:
			~SPIRVShaderInspector() override = default;

			bool InspectShader(std::vector<uint32_t>& _shaderCode, RHIType _shaderAPIType, RHIShaderType _shaderType, InspectedShader* _inspectedShaderResult) override final;

		private:
			bool CreateReflectionModule(std::vector<uint32_t>& _shaderCode, SpvReflectShaderModule* _reflectionModule);
			void DestroyReflectionModule(SpvReflectShaderModule* _reflectionModule);

			bool ReflectVulkanShader(SpvReflectShaderModule* _reflectionModule, RHIShaderType _shaderType, InspectedShader* _finalShaderLayout);

			bool ReflectInputVariables(SpvReflectShaderModule* _reflectionModule, std::vector<SpvReflectInterfaceVariable*>* _reflectedInputVariables);
			bool ReflectDescriptorSets(SpvReflectShaderModule* _reflectionModule, std::vector<SpvReflectDescriptorSet*>* _reflectedDescriptors);
			bool ReflectBlockVariable(SpvReflectShaderModule* _reflectionModule, std::vector<SpvReflectBlockVariable*>* _reflectedBlockVariable);
			
			void ParseInputVariables(const std::vector<SpvReflectInterfaceVariable*>& _reflectedDescriptors, RHIShaderType _shaderType, InspectedShader* _finalShaderLayout);
			void ParseDescriptorSets(const std::vector<SpvReflectDescriptorSet*>& _reflectedDescriptors, RHIShaderType _shaderType, InspectedShader* _finalShaderLayout);
			void ParseBlockVariable(const std::vector<SpvReflectBlockVariable*>& _reflectedBlockVariable, RHIShaderType _shaderType, InspectedShader* _finalShaderLayout);
		};
	}
}