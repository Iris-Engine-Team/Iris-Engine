#include "Loader/ShaderReflection/SPIRVShaderInspector.h"

#include <spirv_reflect.h>

namespace Iris
{
	namespace Engine
	{
		bool SPIRVShaderInspector::InspectShader(std::vector<uint32_t>& _shaderCode, RHIType _shaderAPIType, RHIShaderType _shaderType, InspectedShader* _inspectedShaderResult)
		{
			SpvReflectShaderModule module;
			CreateReflectionModule(_shaderCode, &module);

			switch (_shaderAPIType)
			{
			default: case Iris::Engine::RHIType::IE_RHI_TYPE_VULKAN:
				ReflectVulkanShader(&module, _shaderType, _inspectedShaderResult);
				break;
			case Iris::Engine::RHIType::IE_RHI_TYPE_OPENGL:
				break;
			}

			DestroyReflectionModule(&module);

			return true;
		}

		bool SPIRVShaderInspector::CreateReflectionModule(std::vector<uint32_t>& _shaderCode, SpvReflectShaderModule* _reflectionModule)
		{
			SpvReflectResult reflectResult = spvReflectCreateShaderModule(_shaderCode.size() * sizeof(uint32_t), _shaderCode.data(), _reflectionModule);

			if (reflectResult != SPV_REFLECT_RESULT_SUCCESS)
			{
				DEBUG_ERROR("SPIRV-Reflect: Failed to create reflection module! Error code: %i", reflectResult);
				return false;
			}

			return true;
		}

		void SPIRVShaderInspector::DestroyReflectionModule(SpvReflectShaderModule* _reflectionModule)
		{
			spvReflectDestroyShaderModule(_reflectionModule);
		}

		bool SPIRVShaderInspector::ReflectVulkanShader(SpvReflectShaderModule* _reflectionModule, RHIShaderType _shaderType, InspectedShader* _finalShaderLayout)
		{
			std::vector<SpvReflectInterfaceVariable*> inputVariables = std::vector<SpvReflectInterfaceVariable*>();
			std::vector<SpvReflectDescriptorSet*> descriptorSets = std::vector<SpvReflectDescriptorSet*>();
			std::vector<SpvReflectBlockVariable*> blockVariable = std::vector<SpvReflectBlockVariable*>();

			ReflectInputVariables(_reflectionModule, &inputVariables);
			ReflectDescriptorSets(_reflectionModule, &descriptorSets);
			ReflectBlockVariable(_reflectionModule, &blockVariable);

			ParseDescriptorSets(descriptorSets, _shaderType, _finalShaderLayout);
			ParseBlockVariable(blockVariable, _shaderType, _finalShaderLayout);

			return false;
		}

		bool SPIRVShaderInspector::ReflectInputVariables(SpvReflectShaderModule* _reflectionModule, std::vector<SpvReflectInterfaceVariable*>* _reflectedInputVariables)
		{
			uint32_t inputVariablesCount = 0;
			SpvReflectResult reflectResult = spvReflectEnumerateInputVariables(_reflectionModule, &inputVariablesCount, nullptr);

			if (reflectResult != SPV_REFLECT_RESULT_SUCCESS)
			{
				DEBUG_ERROR("SPIRV-Reflect: Failed to enumerate input variables with reflection! Error code: %i", reflectResult);
				return false;
			}

			_reflectedInputVariables->resize(inputVariablesCount);

			reflectResult = spvReflectEnumerateInputVariables(_reflectionModule, &inputVariablesCount, _reflectedInputVariables->data());

			if (reflectResult != SPV_REFLECT_RESULT_SUCCESS)
			{
				DEBUG_ERROR("SPIRV-Reflect: Failed to enumerate input variables with reflection! Error code: %i", reflectResult);
				return false;
			}

			return true;
		}

		bool SPIRVShaderInspector::ReflectDescriptorSets(SpvReflectShaderModule* _reflectionModule, std::vector<SpvReflectDescriptorSet*>* _reflectedDescriptors)
		{
			uint32_t descriptorCount = 0;
			SpvReflectResult reflectResult = spvReflectEnumerateDescriptorSets(_reflectionModule, &descriptorCount, nullptr);

			if (reflectResult != SPV_REFLECT_RESULT_SUCCESS)
			{
				DEBUG_ERROR("SPIRV-Reflect: Failed to enumerate descriptors with reflection! Error code: %i", reflectResult);
				return false;
			}

			_reflectedDescriptors->resize(descriptorCount);

			reflectResult = spvReflectEnumerateDescriptorSets(_reflectionModule, &descriptorCount, _reflectedDescriptors->data());

			if (reflectResult != SPV_REFLECT_RESULT_SUCCESS)
			{
				DEBUG_ERROR("SPIRV-Reflect: Failed to enumerate descriptors with reflection! Error code: %i", reflectResult);
				return false;
			}

			return true;
		}

		bool SPIRVShaderInspector::ReflectBlockVariable(SpvReflectShaderModule* _reflectionModule, std::vector<SpvReflectBlockVariable*>* _reflectedBlockVariable)
		{
			uint32_t blockVariableCount = 0;
			SpvReflectResult reflectResult = spvReflectEnumeratePushConstantBlocks(_reflectionModule, &blockVariableCount, nullptr);

			if (reflectResult != SPV_REFLECT_RESULT_SUCCESS)
			{
				DEBUG_ERROR("SPIRV-Reflect: Failed to enumerate block variable with reflection! Error code: %i", reflectResult);
				return false;
			}

			_reflectedBlockVariable->resize(blockVariableCount);

			reflectResult = spvReflectEnumeratePushConstantBlocks(_reflectionModule, &blockVariableCount, _reflectedBlockVariable->data());

			if (reflectResult != SPV_REFLECT_RESULT_SUCCESS)
			{
				DEBUG_ERROR("SPIRV-Reflect: Failed to enumerate block variable with reflection! Error code: %i", reflectResult);
				return false;
			}

			return true;
		}

		void SPIRVShaderInspector::ParseBlockVariable(const std::vector<SpvReflectBlockVariable*>& _reflectedBlockVariable, RHIShaderType _shaderType, InspectedShader* _finalShaderLayout)
		{
			for (const SpvReflectBlockVariable* blockVariable : _reflectedBlockVariable)
			{
				size_t memberCount = blockVariable->member_count;
				InspectedBlockVariable newBlockVariable{};

				newBlockVariable.Name = blockVariable->name;
				newBlockVariable.Offset = blockVariable->offset;
				newBlockVariable.Size = blockVariable->size;
				newBlockVariable.StageFlags = _shaderType;

				for (size_t i = 0; i < memberCount; ++i)
				{
					const SpvReflectBlockVariable blockVariableMember = blockVariable->members[i];
					
					InspectedBlockVariableMember newMember{};
					newMember.Name = blockVariableMember.name;
					newMember.Offset = blockVariableMember.offset;
					newMember.Size = blockVariableMember.size;

					newBlockVariable.Members.push_back(newMember);
				}

				_finalShaderLayout->BlockVariable.push_back(newBlockVariable);
			}
		}

		void SPIRVShaderInspector::ParseInputVariables(const std::vector<SpvReflectInterfaceVariable*>& _reflectedDescriptors, RHIShaderType _shaderType, InspectedShader* _finalShaderLayout)
		{
			_reflectedDescriptors; _shaderType; _finalShaderLayout;
		}

		void SPIRVShaderInspector::ParseDescriptorSets(const std::vector<SpvReflectDescriptorSet*>& _reflectedDescriptors, RHIShaderType _shaderType, InspectedShader* _finalShaderLayout)
		{
			for (const SpvReflectDescriptorSet* descriptor : _reflectedDescriptors)
			{
				size_t bindingCount = descriptor->binding_count;
				InspectedDescriptor newDescriptor{};

				for (size_t i = 0; i < bindingCount; ++i)
				{
					const SpvReflectDescriptorBinding* binding = descriptor->bindings[i];

					InspectedDescriptorBinding newBinding = {};
					newBinding.Binding = binding->binding;
					newBinding.Type = static_cast<RHIDescriptorType>(binding->descriptor_type);
					newBinding.DescriptorCount = binding->count;
					newBinding.StageFlags = _shaderType;

					newDescriptor.DescriptorBindings.push_back(newBinding);
				}

				_finalShaderLayout->Descriptors.push_back(newDescriptor);
			}
		}
	}
}