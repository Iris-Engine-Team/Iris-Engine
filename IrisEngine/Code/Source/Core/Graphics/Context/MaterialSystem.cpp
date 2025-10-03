#include "Core/Graphics/Context/MaterialSystem.h"
#include "Core/Graphics/Context/RenderingContext.h"
#include "Core/Graphics/Renderer/IRenderer.h"
#include "Core/Graphics/RHI/IRenderPass.h"
#include "Core/Graphics/RHI/IDevice.h"

namespace Iris
{
	namespace Engine
	{
		bool MaterialSystem::Initialize(const IRenderer* _renderer, const RenderingContext* _renderingContext, ResourceContext* _resourceContext)
		{
			m_Renderer = _renderer;
			m_RenderingContext = _renderingContext;
			m_ResourceContext = _resourceContext;
			return true;
		}

		bool MaterialSystem::Terminate()
		{
			return true;
		}

		IPipeline* MaterialSystem::CreateMaterialPipeline(const MaterialTemplate& _materialTemplate, const MaterialTransparency& _transparencyMode, const MaterialDraw& _drawMode) const
		{
			IRenderPass* mainRP = m_Renderer->GetMainRenderPass();
			mainRP;

			RHIPipelineTemplate pipelineTemplate{};

			switch (_materialTemplate)
			{
			case MaterialTemplate::IE_MATERIAL_LIT: default:
				switch (m_Renderer->GetType())
				{
				case RendererType::IE_RENDERER_FORWARD: default:
					pipelineTemplate = *m_RenderingContext->GetTemplate(RHIPipelinePass::IE_PIPELINE_LIT_FWD);
					break;
				case RendererType::IE_RENDERER_DEFERRED:
					if (_transparencyMode == MaterialTransparency::IE_OPAQUE)
					{
						pipelineTemplate = *m_RenderingContext->GetTemplate(RHIPipelinePass::IE_PIPELINE_LIT_DFD);
					}
					else
					{
						pipelineTemplate = *m_RenderingContext->GetTemplate(RHIPipelinePass::IE_PIPELINE_LIT_FWD);
					}
					break;
				}
				break;
			case MaterialTemplate::IE_MATERIAL_UNLIT:
				switch (m_Renderer->GetType())
				{
				case RendererType::IE_RENDERER_FORWARD: default:
					pipelineTemplate = *m_RenderingContext->GetTemplate(RHIPipelinePass::IE_PIPELINE_UNLIT_FWD);
					break;
				case RendererType::IE_RENDERER_DEFERRED:
					pipelineTemplate = *m_RenderingContext->GetTemplate(RHIPipelinePass::IE_PIPELINE_UNLIT_DFD);
					break;
				}
				break;
			}

			RHIPipeline pipelineInfo{};
			{
				pipelineInfo.Template = pipelineTemplate;
				pipelineInfo.FaceCulling = static_cast<RHIFaceCulling>(_drawMode);
				pipelineInfo.BlendType = static_cast<RHIBlendType>(_transparencyMode);

				pipelineInfo.Subpass = m_Renderer->GetOpaquePassIndex();
				pipelineInfo.AttachmentCount = m_Renderer->GetOpaquePassAttachmentCount();

				if (pipelineInfo.BlendType != RHIBlendType::IE_RHI_BLEND_TYPE_NONE)
				{
					pipelineInfo.Subpass = m_Renderer->GetTransparentPassIndex();
					pipelineInfo.AttachmentCount = m_Renderer->GetTransparentPassAttachmentCount();
				}

			}

			return m_RenderingContext->InstantiatePipeline(m_Renderer->GetMainRenderPass(), pipelineInfo);
		}

		IDescriptor* MaterialSystem::CreateMaterialDescriptor(IDescriptorLayout* _layout, Material* _material) const 
		{
			return m_ResourceContext->CreateMaterialDescriptor(_layout, _material);
		}

		void MaterialSystem::UpdateMaterials()
		{
			if (m_MaterialToUpdate.empty())
				return;

			m_ResourceContext->GetDevice()->WaitGraphicsQueueIdle();

			while (!m_MaterialToUpdate.empty())
			{
				MaterialTextureUpdate update = m_MaterialToUpdate.front();

				m_ResourceContext->UpdateTextureDescriptor(update.MaterialDescriptor, update.NewTexture, update.Binding);

				m_MaterialToUpdate.pop();
			}
		}

		void MaterialSystem::UpdateMaterialDescriptor(Material* _material, const Texture* _texture, unsigned int _binding)
		{
			MaterialTextureUpdate update{};
			update.MaterialDescriptor = _material->GetDescriptor();
			update.NewTexture = _texture;
			update.Binding = _binding;

			m_MaterialToUpdate.push(update);
		}
	}
}