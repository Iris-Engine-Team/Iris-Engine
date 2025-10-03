#pragma once

#include "Resources/Material.h"
#include <queue>

namespace Iris
{
	namespace Engine
	{
		class IPipeline;
		class RenderingContext;
		class IRenderer;

		struct MaterialTextureUpdate
		{
			IDescriptor* MaterialDescriptor = nullptr;
			const Texture* NewTexture = nullptr;
			unsigned int Binding = 0;
		};

		class MaterialSystem
		{
		public:
			bool Initialize(const IRenderer* _renderer, const RenderingContext* _renderingContext, ResourceContext* _resourceContext);
			bool Terminate();

			void UpdateMaterials();

			IPipeline* CreateMaterialPipeline(const MaterialTemplate& _materialTemplate, const MaterialTransparency& _transparencyMode, const MaterialDraw& _drawMode) const;
			IDescriptor* CreateMaterialDescriptor(IDescriptorLayout* _layout, Material* _material) const;

			void UpdateMaterialDescriptor(Material* _material, const Texture* _texture, unsigned int _binding);
		private:
			const RenderingContext* m_RenderingContext = nullptr;
			ResourceContext* m_ResourceContext = nullptr;
			const IRenderer* m_Renderer = nullptr;

			std::queue<MaterialTextureUpdate> m_MaterialToUpdate{};
		};
	}
}