#include "Core/Graphics/Renderer/Shadows/ShadowMap.h"

#include "Core/Graphics/Context/RenderingContext.h"
#include "Core/Graphics/Context/ResourceContext.h"
#include "Core/Graphics/Renderer/IRenderer.h"

#include "Core/Graphics/RHI/ISampler.h"
#include "Core/Graphics/RHI/IDescriptor.h"
#include "Core/Graphics/RHI/IFramebuffer.h"
#include "Core/Graphics/RHI/IRenderPass.h"
#include "Core/Graphics/RHI/IPipeline.h"

#include "Resources/ResourceManager.h"

#include "Core/Tools/FilePath.h"

namespace Iris
{
	namespace Engine
	{
		void ShadowMap::CreateShadowMap(IPipeline* _lightPipeline, IRenderPass* _renderpass, RenderingContext* _renderingContext, ResourceContext* _resourceContext, unsigned int _shadowLayout)
		{
			_renderpass; _renderingContext;

			RHISampler samplerInfo;
			{
				samplerInfo.MagFilter = RHISamplerFilter::IE_RHI_NEAREST;
				samplerInfo.MinFilter = RHISamplerFilter::IE_RHI_NEAREST;
				samplerInfo.ModeU = RHISamplerAdress::IE_RHI_CLAMP_TO_EDGE;
				samplerInfo.ModeV = RHISamplerAdress::IE_RHI_CLAMP_TO_EDGE;
				samplerInfo.ModeW = RHISamplerAdress::IE_RHI_CLAMP_TO_EDGE;
			}

			m_ShadowMapSampler = _resourceContext->CreateSampler(m_ShadowFramebuffers[0], samplerInfo);

			m_ShadowMapDescriptors = _resourceContext->CreateAttachmentsDescriptors(_lightPipeline->GetDescriptorLayout(_shadowLayout), m_ShadowMapSampler, m_ShadowFramebuffers, 0);
		}

		void ShadowMap::DestroyShadowMap(RenderingContext* _renderingContext, ResourceContext* _resourceContext)
		{
			_resourceContext->DestroyBuffers(m_ShadowMapDataBuffer);
			_resourceContext->DeleteDescriptors(m_ShadowMapDataDescriptors);
			_renderingContext->DestroyFramebuffers(m_ShadowFramebuffers);
			_resourceContext->DeleteDescriptors(m_ShadowMapDescriptors);
			_resourceContext->DestroySampler(m_ShadowMapSampler);
		}

		void ShadowMap::CreateDirectionalShadowMap(IPipeline* _lightPipeline, IPipeline* _shadowPipeline, IRenderPass* _renderpass, RenderingContext* _renderingContext, ResourceContext* _resourceContext, unsigned int _shadowLayout)
		{
			DirectionalLightUBO data{};
			m_ShadowMapDataBuffer = _resourceContext->CreateBuffers(RHIBufferType::IE_RHI_UNIFORM_BUFFER, sizeof(DirectionalLightUBO), &data);
			m_ShadowMapDataDescriptors = _resourceContext->CreateBufferDescriptors(_shadowPipeline->GetDescriptorLayout(0), m_ShadowMapDataBuffer, sizeof(DirectionalLightUBO), 0);

			m_ShadowFramebuffers = _renderingContext->CreateFramebuffers(_renderpass, IE_POINT_SHADOW_WIDTH, IE_POINT_SHADOW_HEIGHT, _renderpass->GetAttachments());

			CreateShadowMap(_lightPipeline, _renderpass, _renderingContext, _resourceContext, _shadowLayout);
		}

		void ShadowMap::CreateSpotShadowMap(IPipeline* _lightPipeline, IPipeline* _shadowPipeline, IRenderPass* _renderpass, RenderingContext* _renderingContext, ResourceContext* _resourceContext, unsigned int _shadowLayout)
		{
			DirectionalLightUBO data{};
			m_ShadowMapDataBuffer = _resourceContext->CreateBuffers(RHIBufferType::IE_RHI_UNIFORM_BUFFER, sizeof(DirectionalLightUBO), &data);
			m_ShadowMapDataDescriptors = _resourceContext->CreateBufferDescriptors(_shadowPipeline->GetDescriptorLayout(0), m_ShadowMapDataBuffer, sizeof(DirectionalLightUBO), 0);

			m_ShadowFramebuffers = _renderingContext->CreateFramebuffers(_renderpass, IE_POINT_SHADOW_WIDTH, IE_POINT_SHADOW_HEIGHT, _renderpass->GetAttachments());

			CreateShadowMap(_lightPipeline, _renderpass, _renderingContext, _resourceContext, _shadowLayout);
		}

		void ShadowMap::CreatePointShadowMap(IPipeline* _lightPipeline, IPipeline* _shadowPipeline, IRenderPass* _renderpass, RenderingContext* _renderingContext, ResourceContext* _resourceContext, unsigned int _shadowLayout)
		{
			std::vector<RHIAttachment> attachment(1);
			{
				attachment[0].Type = RHIAttachmentType::IE_RHI_DEPTH_ATTACHMENT;
				attachment[0].ColorFormat = RHIColorFormat::IE_RHI_FORMAT_D32_SFLOAT;
				attachment[0].LoadOp = RHILoadOperation::IE_RHI_CLEAR_OP;
				attachment[0].StoreOp = RHIStoreOperation::IE_RHI_STORE_OP;
				attachment[0].Samples = RHISampleCount::IE_RHI_SAMPLE_COUNT_1_BIT;
				attachment[0].EndLayout = RHILayoutType::IE_RHI_LAYOUT_SHADER_READ_ONLY;
				attachment[0].FramebufferType = RHITextureType::IE_RHI_TEXTURE_CUBEMAP;
				attachment[0].AttachmentIndex = 0;
			}

			m_ShadowFramebuffers = _renderingContext->CreateFramebuffers(_renderpass, 2048, 2048, attachment);

			PointLightUBO data{};
			m_ShadowMapDataBuffer = _resourceContext->CreateBuffers(RHIBufferType::IE_RHI_UNIFORM_BUFFER, sizeof(PointLightUBO), &data);
			m_ShadowMapDataDescriptors = _resourceContext->CreateBufferDescriptors(_shadowPipeline->GetDescriptorLayout(0), m_ShadowMapDataBuffer, sizeof(PointLightUBO), 0);
			
			CreateShadowMap(_lightPipeline, _renderpass, _renderingContext, _resourceContext, _shadowLayout);
		}

		void ShadowMap::UpdateDirectionalData(IRenderer* _renderer, Vector3 _lightPosition, Vector3 _lightDirection, float _near, float _far, float _top, float _bot, float _right, float _left)
		{
			Vector3 target = _lightPosition - (_lightDirection * 50.f);
			DirectionalLightUBO data{};
			data.ProjectionView = Matrix4::ProjectionOrthographicMatrix(_near, _far, _top, _bot, _right, _left) * Matrix4::LookAtMatrix(target, _lightPosition, Vector3::s_Up);

			_renderer->UpdateBuffer(m_ShadowMapDataBuffer, &data, sizeof(data));
		}

		void ShadowMap::UpdateSpotData(IRenderer* _renderer, Vector3 _lightPosition, Vector3 _lightDirection, float _near, float _far)
		{
			DirectionalLightUBO data{};
			data.ProjectionView = Matrix4::ProjectionPerspectiveMatrix(_near, _far, 1.f, 90.f * Deg2Rad) * Matrix4::LookAtMatrix(_lightPosition, _lightPosition + _lightDirection, Vector3::s_Up);
			
			_renderer->UpdateBuffer(m_ShadowMapDataBuffer, &data, sizeof(data));
		}

		void ShadowMap::UpdatePointData(IRenderer* _renderer, Vector3 _lightPosition, float _near, float _far)
		{
			Matrix4 projection = Matrix4::ProjectionPerspectiveMatrix(_near, _far, IE_POINT_SHADOW_WIDTH / IE_POINT_SHADOW_HEIGHT, 90.f * Deg2Rad);

			PointLightUBO data{};
			data.ProjectionViews[0] = projection * Matrix4::LookAtMatrix(_lightPosition, _lightPosition + Vector3::s_Right, Vector3::s_Down);
			data.ProjectionViews[1] = projection * Matrix4::LookAtMatrix(_lightPosition, _lightPosition + Vector3::s_Left, Vector3::s_Down);
			data.ProjectionViews[2] = projection * Matrix4::LookAtMatrix(_lightPosition, _lightPosition + Vector3::s_Up, Vector3::s_Forward);
			data.ProjectionViews[3] = projection * Matrix4::LookAtMatrix(_lightPosition, _lightPosition + Vector3::s_Down, Vector3::s_Back);
			data.ProjectionViews[4] = projection * Matrix4::LookAtMatrix(_lightPosition, _lightPosition + Vector3::s_Forward, Vector3::s_Down);
			data.ProjectionViews[5] = projection * Matrix4::LookAtMatrix(_lightPosition, _lightPosition + Vector3::s_Back, Vector3::s_Down);
			data.LightPosition = Vector4(_lightPosition[0], _lightPosition[1], _lightPosition[2], 1.f);
			data.Far = _far;
			_renderer->UpdateBuffer(m_ShadowMapDataBuffer, &data, sizeof(data));
		}
	}
}