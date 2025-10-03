#pragma once

#include "SECS/Components/IComponent.h"


#include "Core/Graphics/Renderer/DrawCommand.h"

#include "Resources/Mesh.h"
#include "Resources/Texture.h"
#include "Resources/Material.h"

#include "Matrix/Matrix4.h"
#include "Matrix/Matrix3.h"

namespace Iris
{
	namespace Engine
	{
		class RenderingContext;
		class IBuffer;
		class IDescriptor;
		class ResourceContext;
		class IPipeline;
		class IRenderer;
		class ResourceContext;
		class SkyboxObject;

		struct MeshDataUBO
		{
			Matrix4 Model = Matrix4::Identity;
			Matrix4 NormalMatrix = Matrix3::Identity();
		};

		class ModelRenderer : public IComponent
		{
		public:
			DEFINE_COMPONENT(ModelRenderer);
			static IE_FORCE_INLINE std::string GetName() { return "Model Renderer"; }
			static IE_FORCE_INLINE ComponentID GetID() { return ComponentID::IE_MODEL_RENDERER; }

			IRIS_ENGINE_API ModelRenderer() = default;
			IRIS_ENGINE_API ~ModelRenderer() override;

			/// <summary>
			/// Writes data in the given metadata
			/// </summary>
			/// <param name="_metadata"></param>
			IRIS_ENGINE_API void Serialize(IMetadata* _metadata) final override;

			/// <summary>
			/// Init the components with the given data
			/// </summary>
			IRIS_ENGINE_API void Deserialization(IMetadata* _metadata, InitEditorContext& _initializer) final override;

			/// <summary>
			/// Init fonction
			/// </summary>
			/// <param name="_resourceContext"></param>
			/// <param name="_mesh"></param>
			/// <param name="_material"></param>
			/// <param name="_pipeline"></param>
			IRIS_ENGINE_API void Init(ResourceContext* _resourceContext, Mesh* _mesh, IPipeline* _pipeline);

			IRIS_ENGINE_API void Init(InitEditorContext& _initializer) override;

			IRIS_ENGINE_API void Destroy() final override;
			
			IRIS_ENGINE_API void Copy(IComponent* _componentToCopy) final override;

			IRIS_ENGINE_API void Duplicate(IComponent* _componentToDuplicate, InitEditorContext* _initializer) final override;

			/// <summary>
			/// Render the mesh in the world
			/// </summary>
			/// <param name="_renderer"></param>
			void RenderDrawCommand(IRenderer* _renderer, const DrawCommand& _command, const CameraComponent& _camera, SkyboxObject* _skybox);
			void RenderMesh(IRenderer* _renderer);
			void RenderModel(IRenderer* _renderer);

			IE_FORCE_INLINE bool IsCastingShadow() const { return m_CastShadow; }
			IE_FORCE_INLINE void SetCastingShadow(bool _isCastingShadow) { m_CastShadow = _isCastingShadow; }

			IRIS_ENGINE_API void SetMesh(Mesh* _mesh);
			IRIS_ENGINE_API void SetMaterial(Material* _material, size_t _index);

			IRIS_ENGINE_API IE_FORCE_INLINE Mesh* GetMesh() { return m_ModelMesh; }
			IRIS_ENGINE_API IE_FORCE_INLINE const std::vector<Material*>& GetMaterials() { return m_ModelMaterials; }
			IE_FORCE_INLINE void SetMatrix(Matrix4 _model) { m_Model = _model; }
			IE_FORCE_INLINE Matrix4 GetMatrix() { return m_Model; }
			IRIS_ENGINE_API void GetDefaultMaterials();

		private:
			bool m_CastShadow = true;

			Matrix4 m_Model;

			Mesh* m_ModelMesh = nullptr;
			std::vector<Material*> m_ModelMaterials{};
			IPipeline* m_Pipeline = nullptr;

			std::vector<IBuffer*> m_TransformBuffers{};
			std::vector<IDescriptor*> m_TransformDescriptors{};

			ResourceContext* m_ResourceContext = nullptr;
		};
	}
}