#include "Resources/Material.h"
#include "Resources/Texture.h"
#include "Resources/ResourceManager.h"

#include "Core/Graphics/Context/ResourceContext.h"
#include "Core/Graphics/Context/MaterialSystem.h"

#include "Core/Graphics/RHI/IPipeline.h"

#include "Resources/MetadataManager.h"
namespace Iris
{
	namespace Engine
	{
		void Material::Serialize()
		{
			m_Data->Write("Type", MetadataResourceType::IE_MATERIAL_DATA);

			std::vector<std::string> pathMat = { "Material Data", "" };

			pathMat[1] = "GLTF";
			m_Data->Write(pathMat, m_IsGLTF);

			pathMat[1] = "Template";
			m_Data->Write(pathMat, m_Template);

			pathMat[1] = "Transparency";
			m_Data->Write(pathMat, m_TransparencyMode);

			pathMat[1] = "DrawMode";
			m_Data->Write(pathMat, m_DrawMode);

			pathMat[1] = "IsOpaque";
			m_Data->Write(pathMat, m_IsOpaque);

			pathMat[1] = "IsLit";
			m_Data->Write(pathMat, m_IsLit);

			pathMat[1] = "Inputs";
			m_Data->RemoveAt(pathMat);

			pathMat.push_back("");

			switch (m_Template)
			{
			case Iris::Engine::MaterialTemplate::IE_MATERIAL_LIT:
			{
				LitPBRInputs* litInputs = dynamic_cast<LitPBRInputs*>(m_InputVariables);

				pathMat[2] = "Albedo Vec4";
				Vector4 alb = litInputs->GetAlbedo();
				std::vector<float> vec4 = { alb[0], alb[1], alb[2], alb[3] };
				m_Data->Write(pathMat, vec4);

				int params = GetFixedData().Parameters;

				pathMat[2] = "Albedo Text";
				const Texture* albedo = litInputs->GetAlbedoTexture();

				if (params & IE_MATERIAL_ALBEDO_MAP)
					m_Data->Write(pathMat, albedo->GetName());
				else
					m_Data->Write(pathMat, "");


				pathMat[2] = "Normal";
				const Texture* normal = litInputs->GetNormalTexture();
				if (params & IE_MATERIAL_NORMAL_MAP)
					m_Data->Write(pathMat, normal->GetName());
				else
					m_Data->Write(pathMat, "");


				pathMat[2] = "Roughness";

				if (params & IE_MATERIAL_ROUGHNESS_MAP)
				{
					m_Data->Write(pathMat, litInputs->GetRoughnessTexture()->GetName());

					//type texture
					pathMat[2] = "Roughness Type";
					m_Data->Write(pathMat, 0);
				}
				else
				{
					m_Data->Write(pathMat, litInputs->GetRoughness());

					//type float
					pathMat[2] = "Roughness Type";
					m_Data->Write(pathMat, 1);
				}


				pathMat[2] = "Metallic";

				if (params & IE_MATERIAL_METALLIC_MAP)
				{
					m_Data->Write(pathMat, litInputs->GetMetallicTexture()->GetName());

					//type texture
					pathMat[2] = "Metallic Type";
					m_Data->Write(pathMat, 0);
				}
				else
				{
					m_Data->Write(pathMat, litInputs->GetMetallic());

					//type float
					pathMat[2] = "Metallic Type";
					m_Data->Write(pathMat, 1);
				}

				break;
			}
			case Iris::Engine::MaterialTemplate::IE_MATERIAL_UNLIT:
			{
				UnlitInputs* litInputs = dynamic_cast<UnlitInputs*>(m_InputVariables);

				pathMat[2] = "Albedo Vec4";
				Vector4 alb = litInputs->GetAlbedo();
				std::vector<float> vec4 = { alb[0], alb[1], alb[2], alb[3] };
				m_Data->Write(pathMat, vec4);

				pathMat[2] = "Albedo Text";
				const Texture* albedo = litInputs->GetAlbedoTexture();

				if (albedo)
					m_Data->Write(pathMat, albedo->GetName());
				else
					m_Data->Write(pathMat, "");
				break;
			}
			case Iris::Engine::MaterialTemplate::IE_MATERIAL_CUBEMAP:
			{
				CubemapInputs* litInputs = dynamic_cast<CubemapInputs*>(m_InputVariables);
				pathMat[2] = "Cubemap";
				const Texture* panoramic = litInputs->Cubemap;
				if (panoramic)
					m_Data->Write(pathMat, panoramic->GetName());
				else
					m_Data->Write(pathMat, "");
				break;
			}
			case Iris::Engine::MaterialTemplate::IE_MATERIAL_WIREFRAME:
				break;
			default:
				break;
			}

			m_Data->Update();
		}

		void Material::Deserialization(IMetadata* _metadata)
		{
			m_Data = _metadata;
			p_ResourceName = _metadata->GetData("Name");

			std::vector<std::string> pathMat = { "Material Data", "" };

			pathMat[1] = "GLTF";
			SetGLTF(_metadata->GetData(pathMat));
			
			pathMat[1] = "Template";
			SetMaterialTemplate(_metadata->GetData(pathMat));

			pathMat[1] = "Transparency";
			SetTransparencyMode(_metadata->GetData(pathMat));
		
			pathMat[1] = "DrawMode";
			SetDrawMode(_metadata->GetData(pathMat));


			pathMat[1] = "IsOpaque";
			m_IsOpaque = _metadata->GetData(pathMat);

			pathMat[1] = "IsLit";
			m_IsLit = _metadata->GetData(pathMat);

			pathMat[1] = "Inputs";
			pathMat.push_back("");

			ResourceManager* rm = ResourceManager::GetInstance();

			switch (m_Template)
			{
			case Iris::Engine::MaterialTemplate::IE_MATERIAL_LIT:
			{
				LitPBRInputs* inputs = new LitPBRInputs();
				m_InputVariables = inputs;
				m_InputVariables->SetMaterialReference(this);

				pathMat[2] = "Albedo Vec4";
				std::vector<float> vec4 = _metadata->GetData(pathMat);
				inputs->SetAlbedo(Vector4(vec4[0], vec4[1], vec4[2], vec4[3]));

				pathMat[2] = "Albedo Text";
				std::string textureName = _metadata->GetData(pathMat);
				if (textureName != "")
				{
					TextureLoadingParameters params{};
					params.ColorFormat = RHIColorFormat::IE_RHI_FORMAT_R8G8B8A8_SRGB;

					const Texture* albedoTex = rm->Create<Texture>(FIND_PATH(textureName), true, params);
					if (albedoTex)
						inputs->SetAlbedo(albedoTex);
				}


				pathMat[2] = "Normal";
				textureName = _metadata->GetData(pathMat);
				if (textureName != "")
				{
					const Texture* normalTex = rm->Create<Texture>(FIND_PATH(textureName));
					if (normalTex)
						inputs->SetNormal(normalTex);
				}


				pathMat[2] = "Roughness Type";
				if (_metadata->GetData(pathMat) == 1)
				{
					pathMat[2] = "Roughness";
					float roughness = _metadata->GetData(pathMat);
					inputs->SetRoughnessValue(roughness);
				}
				else
				{
					pathMat[2] = "Roughness";
					textureName = _metadata->GetData(pathMat);
					if (textureName != "")
					{
						const Texture* roughnessTex = rm->Create<Texture>(FIND_PATH(textureName));
						if (roughnessTex)
							inputs->SetRoughnessTexture(roughnessTex);
					}
				}


				pathMat[2] = "Metallic Type";
				if (_metadata->GetData(pathMat) == 1)
				{
					pathMat[2] = "Metallic";
					float metallic = _metadata->GetData(pathMat);
					inputs->SetMetallicValue(metallic);
				}
				else
				{
					pathMat[2] = "Metallic";
					textureName = _metadata->GetData(pathMat);
					if (textureName != "")
					{
						const Texture* metallicTex = rm->Create<Texture>(FIND_PATH(textureName));
						if (metallicTex)
							inputs->SetMetallicTexture(metallicTex);
					}
				}

				break;
			}
			case Iris::Engine::MaterialTemplate::IE_MATERIAL_UNLIT:
			{
				UnlitInputs* inputs = new UnlitInputs();
				m_InputVariables = inputs;
				m_InputVariables->SetMaterialReference(this);

				pathMat[2] = "Albedo Vec4";
				std::vector<float> vec4 = _metadata->GetData(pathMat);
				inputs->SetAlbedo(Vector4(vec4[0], vec4[1], vec4[2], vec4[3]));


				pathMat[2] = "Albedo Text";
				std::string textureName = _metadata->GetData(pathMat);
				if (textureName != "")
				{
					const Texture* albedoTex = rm->Create<Texture>(FIND_PATH(textureName));
					if (albedoTex)
						inputs->SetAlbedo(albedoTex);
				}
				break;
			}
			case Iris::Engine::MaterialTemplate::IE_MATERIAL_CUBEMAP:
			{
				CubemapInputs* inputs = new CubemapInputs();
				m_InputVariables = inputs;
				m_InputVariables->SetMaterialReference(this);

				pathMat[2] = "Cubemap";
				std::string textureName = _metadata->GetData(pathMat);
				if (textureName != "")
				{
					TextureLoadingParameters param2{};
					param2.ColorFormat = RHIColorFormat::IE_RHI_FORMAT_R32G32B32A32_SFLOAT;
					const Texture* panoramic = rm->Create<Texture>(FIND_PATH(textureName), true, param2);

					if (panoramic)
						inputs->Cubemap = panoramic;
				}
				break;
			}
			case Iris::Engine::MaterialTemplate::IE_MATERIAL_WIREFRAME:
				break;
			default:
				break;
			}

			const MaterialSystem* ms = rm->GetMaterialSystem();
			m_TemplatePipeline = ms->CreateMaterialPipeline(m_Template, m_TransparencyMode, m_DrawMode);
			m_MaterialDescriptor = ms->CreateMaterialDescriptor(m_TemplatePipeline->GetDescriptorLayout(2), this);

			if (m_Template == MaterialTemplate::IE_MATERIAL_LIT)
			{
				LitPBRInputs* inp = dynamic_cast<LitPBRInputs*>(m_InputVariables);
				inp->SetAlbedo(inp->GetAlbedo());
			}

		}

		void Material::SetTransparencyMode(MaterialTransparency _transparency)
		{
			if (_transparency == m_TransparencyMode)
				return;

			m_TransparencyMode = _transparency;

			switch (m_TransparencyMode)
			{
			case Iris::Engine::MaterialTransparency::IE_OPAQUE: default:
				m_MaterialData.Parameters &= ~IE_MATERIAL_TRANSPARENT;
				m_IsOpaque = true;
				break;
			case Iris::Engine::MaterialTransparency::IE_ALPHA_BLEND: case Iris::Engine::MaterialTransparency::IE_ADDITIVE:
				m_MaterialData.Parameters |= IE_MATERIAL_TRANSPARENT;
				m_IsOpaque = false;
				break;
			}

			m_TemplatePipeline = ResourceManager::GetInstance()->GetMaterialSystem()->CreateMaterialPipeline(m_Template, m_TransparencyMode, m_DrawMode);
		}

		void Material::SetDrawMode(MaterialDraw _materialDraw)
		{
			if (_materialDraw == m_DrawMode)
				return;

			m_DrawMode = _materialDraw;

			m_TemplatePipeline = ResourceManager::GetInstance()->GetMaterialSystem()->CreateMaterialPipeline(m_Template, m_TransparencyMode, m_DrawMode);
		}

		void LitPBRInputs::SetAlbedo(const Texture* _value)
		{
			m_AlbedoText = _value;

			if (!p_MaterialReference)
				return;

			MaterialUBO data = p_MaterialReference->GetFixedData();
			data.Parameters |= IE_MATERIAL_ALBEDO_MAP;
			p_MaterialReference->SetFixedData(data);

			if (!p_MaterialReference->GetDescriptor())
				return;

			if (!m_AlbedoText)
			{
				data = p_MaterialReference->GetFixedData();
				data.Parameters &= ~IE_MATERIAL_ALBEDO_MAP;
				p_MaterialReference->SetFixedData(data);
				return;
			}

			p_MaterialReference->UpdateMaterialTexture(m_AlbedoText, 0);
		}

		void LitPBRInputs::SetNormal(const Texture* _value)
		{
			m_Normal = _value;

			if (!p_MaterialReference)
				return;

			MaterialUBO data = p_MaterialReference->GetFixedData();
			data.Parameters |= IE_MATERIAL_NORMAL_MAP;
			p_MaterialReference->SetFixedData(data);

			if (!p_MaterialReference->GetDescriptor())
				return;

			if (!m_Normal)
			{
				data = p_MaterialReference->GetFixedData();
				data.Parameters &= ~IE_MATERIAL_NORMAL_MAP;
				p_MaterialReference->SetFixedData(data);
				return;
			}

			p_MaterialReference->UpdateMaterialTexture(m_Normal, 3);
		}

		void Material::CreateContainer(ContainerData* _resourceData, const ResourceContext* _resourceContext)
		{
			IResourceContainer::CreateContainer(_resourceData, _resourceContext);

			MaterialData* data = reinterpret_cast<MaterialData*>(_resourceData);

			if (!data)
				return;

			ResourceManager* rm = ResourceManager::GetInstance();

			m_IsGLTF = data->IsGLTF;

			if (m_IsGLTF)
				m_MaterialData.Parameters |= IE_MATERIAL_GLTF;

			m_TransparencyMode = data->TransparencyMode;
			m_DrawMode = data->DrawMode;

			const MaterialSystem* ms = rm->GetMaterialSystem();


			m_Template = data->Template;

			m_TemplatePipeline = ms->CreateMaterialPipeline(m_Template, m_TransparencyMode, m_DrawMode);

			CreateInputVariables(m_Template, data->Inputs);

			m_MaterialDescriptor = ms->CreateMaterialDescriptor(m_TemplatePipeline->GetDescriptorLayout(2), this);


			MetaDataManager* mm = rm->GetMetadataManager();
			IMetadata* allMatList = mm->GetMetadata("AllMaterials");

			MetaDataType allMatData = allMatList->GetData("Data");
			auto matData = allMatData.end();

			std::filesystem::path matDataPath = FIND_PATH(p_ResourceName + ".meta");


			allMatData.find(p_ResourceName);

			if (matData != allMatData.end())
			{
				return;
			}

			allMatList->AddValue("Data", p_ResourceName);

			if (matDataPath == "")
			{
				matDataPath = FIND_PATH("ResourceData");
				matDataPath += "\\" + p_ResourceName + ".meta";
			}

			m_Data = mm->CreateMetadata(matDataPath, p_ResourceName);
			Serialize();
		}

		void Material::UnloadCompiledData(ResourceContext* _resourceContext)
		{
			_resourceContext->DeleteDescriptor(m_MaterialDescriptor);

			if (m_InputVariables)
			{
				delete m_InputVariables;
				m_InputVariables = nullptr;
			}
		}

		void Material::UpdateMaterialTexture(const Texture* _texture, unsigned int _binding)
		{
			ResourceManager::GetInstance()->GetMaterialSystem()->UpdateMaterialDescriptor(this, _texture, _binding);
		}

		void Material::CreateInputVariables(const MaterialTemplate& _template, MaterialInputs* _inputs)
		{
			switch (_template)
			{
			case MaterialTemplate::IE_MATERIAL_LIT: default:

				m_InputVariables = new LitPBRInputs();
				m_InputVariables->SetMaterialReference(this);
				CopyLitInputVariables(dynamic_cast<LitPBRInputs*>(_inputs));
				break;
			case MaterialTemplate::IE_MATERIAL_UNLIT:
				m_InputVariables = new UnlitInputs();
				*m_InputVariables = *dynamic_cast<UnlitInputs*>(_inputs);
				break;
			case MaterialTemplate::IE_MATERIAL_CUBEMAP:
				m_InputVariables = new CubemapInputs();
				*m_InputVariables = *dynamic_cast<CubemapInputs*>(_inputs);
				break;
			case MaterialTemplate::IE_MATERIAL_WIREFRAME:
				break;
			}
		}

		void Material::CopyLitInputVariables(LitPBRInputs* _inputs)
		{
			LitPBRInputs* localInputs = dynamic_cast<LitPBRInputs*>(m_InputVariables);

			localInputs->SetAlbedo(_inputs->GetAlbedoTexture());
			localInputs->SetAlbedo(_inputs->GetAlbedo());

			const Texture* metallic = _inputs->GetMetallicTexture();
			const Texture* roughness = _inputs->GetRoughnessTexture();

			if (roughness)
				localInputs->SetRoughnessTexture(roughness);
			else
				localInputs->SetRoughnessValue(_inputs->GetRoughness());

			if (metallic)
				localInputs->SetMetallicTexture(metallic);
			else
				localInputs->SetMetallicValue(_inputs->GetMetallic());

			localInputs->SetNormal(_inputs->GetNormalTexture());
		}
	}
}