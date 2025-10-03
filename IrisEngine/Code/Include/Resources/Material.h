#pragma once

#include "Resources/IResourceContainer.h"

#include <vector>
#include <variant>
#include <concepts>

#define IE_MATERIAL_ALBEDO_MAP      (0x1 << 0x0)
#define IE_MATERIAL_NORMAL_MAP      (0x1 << 0x1)
#define IE_MATERIAL_ROUGHNESS_MAP   (0x1 << 0x2)
#define IE_MATERIAL_METALLIC_MAP    (0x1 << 0x3)
#define IE_MATERIAL_TRANSPARENT     (0x1 << 0x4)
#define IE_MATERIAL_GLTF            (0x1 << 0x5)

namespace Iris
{
	namespace Engine
	{
		class IMetadata;

		class IDescriptor;
		class IPipeline;
		class Texture;

		enum class MaterialTemplate : uint32_t
		{
			IE_MATERIAL_LIT = 0,
			IE_MATERIAL_UNLIT = 1,
			IE_MATERIAL_CUBEMAP = 2,
			IE_MATERIAL_WIREFRAME = 3
		};

		enum class MaterialDraw : uint32_t
		{
			IE_NONE = 0,
			IE_FRONT = 1,
			IE_BACK = 2,
			IE_FRONT_AND_BACK = 3
		};

		enum class MaterialTransparency : uint32_t
		{
			IE_OPAQUE = 0,
			IE_ALPHA_BLEND = 1,
			IE_ADDITIVE = 2
		};

		class MaterialInputs;
		class LitPBRInputs;

		struct MaterialUBO
		{
			int Parameters = 0;
			float Roughness = 0;
			float Metallic = 0;

			float Padding = 0;

			Vector4 Albedo = Vector4::s_One;
		};

		class Material : public IResourceContainer
		{
		public:

			~Material() override = default;
			
			/// <summary>
			/// Writes data
			/// </summary>
			void Serialize();

			/// <summary>
			/// Init the material with the given data
			/// </summary>
			void Deserialization(IMetadata* _metadata);

			IRIS_ENGINE_API IE_FORCE_INLINE const MaterialTransparency& GetTransparencyMode() const { return m_TransparencyMode; }
			IRIS_ENGINE_API void SetTransparencyMode(MaterialTransparency _transparency);

			IRIS_ENGINE_API IE_FORCE_INLINE const MaterialDraw& GetDrawMode() const { return m_DrawMode; }
			IRIS_ENGINE_API void SetDrawMode(MaterialDraw _materialDraw);
			IRIS_ENGINE_API IE_FORCE_INLINE IPipeline* GetMaterialPipeline() const { return m_TemplatePipeline; }
				 
			IRIS_ENGINE_API IE_FORCE_INLINE IDescriptor* GetDescriptor() const { return m_MaterialDescriptor; }
			IRIS_ENGINE_API IE_FORCE_INLINE MaterialInputs* GetInputVariables() { return m_InputVariables; }

			IRIS_ENGINE_API IE_FORCE_INLINE const MaterialTemplate& GetMaterialTemplate() const { return m_Template; }
			IRIS_ENGINE_API IE_FORCE_INLINE void SetMaterialTemplate(MaterialTemplate _template) { m_Template = _template; }


			IRIS_ENGINE_API IE_FORCE_INLINE void SetGLTF(bool _isGLTF)
			{
				m_IsGLTF = _isGLTF;

				if (m_IsGLTF)
					m_MaterialData.Parameters |= IE_MATERIAL_GLTF;
				else
					m_MaterialData.Parameters &= ~IE_MATERIAL_GLTF;
			} 

			IRIS_ENGINE_API IE_FORCE_INLINE const bool IsOpaque() const { return m_IsOpaque; }
			IRIS_ENGINE_API IE_FORCE_INLINE const bool IsTransparent() const { return !m_IsOpaque; }
			IRIS_ENGINE_API IE_FORCE_INLINE const bool IsGLTF() const { return m_IsGLTF; }

			IRIS_ENGINE_API IE_FORCE_INLINE const bool IsLit() const { return m_IsLit; }
			IRIS_ENGINE_API IE_FORCE_INLINE const bool IsUnlit() const { return !m_IsLit; }

			IRIS_ENGINE_API IE_FORCE_INLINE const MaterialUBO& GetFixedData() const { return m_MaterialData; }
			IRIS_ENGINE_API IE_FORCE_INLINE void SetFixedData(const MaterialUBO& _fixedData) { m_MaterialData = _fixedData; }

			IRIS_ENGINE_API void CreateContainer(ContainerData* _resourceData, const ResourceContext* _resourceContext) override final;
			IRIS_ENGINE_API void UnloadCompiledData(ResourceContext* _resourceContext) override final;

			IRIS_ENGINE_API void UpdateMaterialTexture(const Texture* _texture, unsigned int _binding);

		private:
			void CreateInputVariables(const MaterialTemplate& _template, MaterialInputs* _inputs);
			void CopyLitInputVariables(LitPBRInputs* _inputs);

		private:
			IDescriptor* m_MaterialDescriptor = nullptr;
			IPipeline* m_TemplatePipeline = nullptr;

			bool m_IsGLTF = false;

			bool m_IsOpaque = true;

			bool m_IsLit = true;

			MaterialInputs* m_InputVariables = nullptr;

			MaterialTemplate m_Template = MaterialTemplate::IE_MATERIAL_LIT;
			MaterialTransparency m_TransparencyMode = MaterialTransparency::IE_OPAQUE;
			MaterialDraw m_DrawMode = MaterialDraw::IE_FRONT;
			MaterialUBO m_MaterialData{};

			IMetadata* m_Data = nullptr;
		};

		class MaterialInputs
		{
		public:
			virtual ~MaterialInputs() = default;

			IE_FORCE_INLINE void SetMaterialReference(Material* _material) { p_MaterialReference = _material; }

		protected:
			Material* p_MaterialReference = nullptr;
		};

		template<typename T>
		concept TextFloatInput = std::is_same_v<T, Texture*> || std::is_same_v<T, const Texture*> || std::is_same_v<T, float>;

		class LitPBRInputs : public MaterialInputs
		{
		public:
			~LitPBRInputs() override = default;

			IRIS_ENGINE_API void SetAlbedo(const Texture* _value);
			IRIS_ENGINE_API IE_FORCE_INLINE void SetAlbedo(const Vector4& _value)
			{
				m_AlbedoVec = _value;

				if (p_MaterialReference && p_MaterialReference->GetDescriptor())
				{
					MaterialUBO data = p_MaterialReference->GetFixedData();
					data.Albedo = m_AlbedoVec;
					p_MaterialReference->SetFixedData(data);
				}
			}

			IRIS_ENGINE_API IE_FORCE_INLINE const Texture* GetAlbedoTexture() const { return m_AlbedoText; }
			IRIS_ENGINE_API IE_FORCE_INLINE const Vector4 GetAlbedo() const { return m_AlbedoVec; }

			IRIS_ENGINE_API IE_FORCE_INLINE void SetRoughnessTexture(const Texture* _value)
			{
				m_RoughnessTexture = _value;

				if (!p_MaterialReference)
					return;

				MaterialUBO data = p_MaterialReference->GetFixedData();

				data.Parameters |= IE_MATERIAL_ROUGHNESS_MAP;

				if (p_MaterialReference->GetDescriptor())
				{
					p_MaterialReference->UpdateMaterialTexture(m_RoughnessTexture, 1);
				}

				p_MaterialReference->SetFixedData(data);
			}

			IRIS_ENGINE_API IE_FORCE_INLINE void SetRoughnessValue(float _value)
			{
				m_RoughnessValue = _value;

				if (!p_MaterialReference)
					return;

				MaterialUBO data = p_MaterialReference->GetFixedData();

				data.Roughness = m_RoughnessValue;
				data.Parameters &= ~IE_MATERIAL_ROUGHNESS_MAP;

				p_MaterialReference->SetFixedData(data);
			}

			IRIS_ENGINE_API IE_FORCE_INLINE const Texture* GetRoughnessTexture() const { return m_RoughnessTexture; }

			IRIS_ENGINE_API IE_FORCE_INLINE float GetRoughness() const { return m_RoughnessValue; }

			IRIS_ENGINE_API IE_FORCE_INLINE void SetMetallicTexture(const Texture* _value)
			{
				m_MetallicTexture = _value;

				if (!p_MaterialReference)
					return;

				MaterialUBO data = p_MaterialReference->GetFixedData();

				data.Parameters |= IE_MATERIAL_METALLIC_MAP;

				if (p_MaterialReference->IsGLTF())
				{
					if (p_MaterialReference->GetDescriptor())
					{
						p_MaterialReference->UpdateMaterialTexture(m_RoughnessTexture, 2);
					}
				}
				else
				{
					if (p_MaterialReference->GetDescriptor())
					{
						p_MaterialReference->UpdateMaterialTexture(m_MetallicTexture, 3);
					}
				}

				p_MaterialReference->SetFixedData(data);
			}

			IRIS_ENGINE_API IE_FORCE_INLINE void SetMetallicValue(float _value)
			{
				m_MetallicValue = _value;

				if (!p_MaterialReference)
					return;

				MaterialUBO data = p_MaterialReference->GetFixedData();

				data.Metallic = m_MetallicValue;
				data.Parameters &= ~IE_MATERIAL_METALLIC_MAP;

				p_MaterialReference->SetFixedData(data);
			}

			IRIS_ENGINE_API IE_FORCE_INLINE const Texture* GetMetallicTexture() const { return m_MetallicTexture; }

			IRIS_ENGINE_API IE_FORCE_INLINE float GetMetallic() const { return m_MetallicValue; }

			IRIS_ENGINE_API void SetNormal(const Texture* _value);

			IRIS_ENGINE_API IE_FORCE_INLINE const Texture* GetNormalTexture() const { return m_Normal; }

		private:
			const Texture* m_AlbedoText{};
			Vector4 m_AlbedoVec = Vector4::s_One;

			const Texture* m_RoughnessTexture = nullptr;
			float m_RoughnessValue = 0.f;

			const Texture* m_MetallicTexture = nullptr;
			float m_MetallicValue = 0.f;

			const Texture* m_Normal = nullptr;
		};

		class UnlitInputs : public MaterialInputs
		{
		public:
			~UnlitInputs() override = default;

			IRIS_ENGINE_API IE_FORCE_INLINE void SetAlbedo(const Texture* _value) { m_AlbedoText = _value; }
			IRIS_ENGINE_API IE_FORCE_INLINE void SetAlbedo(const Vector4& _value) { m_AlbedoVec = _value; }

			IRIS_ENGINE_API IE_FORCE_INLINE const Texture* GetAlbedoTexture() const { return m_AlbedoText; }
			IRIS_ENGINE_API IE_FORCE_INLINE const Vector4 GetAlbedo() const { return m_AlbedoVec; }

		private:
			const Texture* m_AlbedoText = nullptr;
			Vector4 m_AlbedoVec = Vector4::s_Zero;
		};


		class CubemapInputs : public MaterialInputs
		{
		public:
			~CubemapInputs() override = default;

			const Texture* Cubemap = nullptr;
		};

		struct MaterialData : public ContainerData
		{
			~MaterialData() override = default;
			MaterialTemplate Template = MaterialTemplate::IE_MATERIAL_LIT;
			MaterialTransparency TransparencyMode = MaterialTransparency::IE_OPAQUE;
			MaterialDraw DrawMode = MaterialDraw::IE_FRONT;
			MaterialInputs* Inputs = nullptr;
			bool IsGLTF = false;
		};
	}
}