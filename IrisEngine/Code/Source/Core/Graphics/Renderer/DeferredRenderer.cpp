#include "Core/Graphics/Renderer/DeferredRenderer.h"

#include "Core/IWindow.h"
#include "Core/Graphics/Context/RenderingContext.h"
#include "Core/Graphics/RHI/IDevice.h"
#include "Core/Graphics/RHI/ISwapChain.h"
#include "Core/Graphics/RHI/IPipeline.h"
#include "Core/Graphics/RHI/IRenderPass.h"
#include "Core/Graphics/RHI/ICommandAllocator.h"
#include "Core/Graphics/RHI/ICommandBuffer.h"
#include "Core/Graphics/RHI/IDescriptorAllocator.h"
#include "Core/Graphics/RHI/IDescriptor.h"
#include "Core/Graphics/RHI/IBuffer.h"
#include "Core/Graphics/RHI/IFramebuffer.h"

#include "Core/Tools/FilePath.h"
#include "Resources/ResourceManager.h"

#include "SECS/LightData/LightData.h"
#include "SECS/Components/CameraComponent.h"
#include "SECS/Components/SkyboxObject.h"

namespace Iris
{
	namespace Engine
	{
		RHIResult DeferredRenderer::Initialize(RenderingContext* _renderingContext, ResourceContext* _resourceContext)
		{
			p_Type = RendererType::IE_RENDERER_DEFERRED;
			p_RenderingContext = _renderingContext;
			p_ResourceContext = _resourceContext;

			IRenderer::CreateTemplates();

			p_SwapChain = p_RenderingContext->GetSwapChain();

			p_LightLayoutIndex = 2;
			p_ShadowLayoutIndex = 3;
			p_IBLLayoutIndex = 4;

			p_OpaquePassIndex = 0;
			p_TransparentPassIndex = 2;

			ResourceManager* mg = ResourceManager::GetInstance();

			///////////////////////////////////////////////////////////////////////
			///																	///
			/// 		Deferred Pass											///
			///																	///
			///////////////////////////////////////////////////////////////////////

			// Attachments
			std::vector<RHIAttachment> mRPAtt(6);
			{
				// Output attachment
				mRPAtt[0].Type = RHIAttachmentType::IE_RHI_COLOR_ATTACHMENT;
				mRPAtt[0].ColorFormat = RHIColorFormat::IE_RHI_FORMAT_R8G8B8A8_UNORM;
				mRPAtt[0].LoadOp = RHILoadOperation::IE_RHI_CLEAR_OP;
				mRPAtt[0].StoreOp = RHIStoreOperation::IE_RHI_STORE_OP;
				mRPAtt[0].Samples = RHISampleCount::IE_RHI_SAMPLE_COUNT_1_BIT;
				mRPAtt[0].EndLayout = RHILayoutType::IE_RHI_LAYOUT_SHADER_READ_ONLY;
				mRPAtt[0].AttachmentIndex = 0;

				// Depth attachment
				mRPAtt[1].Type = RHIAttachmentType::IE_RHI_DEPTH_ATTACHMENT;
				mRPAtt[1].ColorFormat = RHIColorFormat::IE_RHI_FORMAT_D32_SFLOAT;
				mRPAtt[1].LoadOp = RHILoadOperation::IE_RHI_CLEAR_OP;
				mRPAtt[1].StoreOp = RHIStoreOperation::IE_RHI_CLEAR_OP;
				mRPAtt[1].Samples = RHISampleCount::IE_RHI_SAMPLE_COUNT_1_BIT;
				mRPAtt[1].EndLayout = RHILayoutType::IE_RHI_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
				mRPAtt[1].AttachmentIndex = 1;

				// Position attachment (G-Buffer)
				mRPAtt[2].ColorFormat = RHIColorFormat::IE_RHI_FORMAT_R16G16B16A16_SFLOAT;
				mRPAtt[2].Type = RHIAttachmentType::IE_RHI_INPUT_ATTACHMENT;
				mRPAtt[2].LoadOp = RHILoadOperation::IE_RHI_CLEAR_OP;
				mRPAtt[2].StoreOp = RHIStoreOperation::IE_RHI_CLEAR_OP;
				mRPAtt[2].Samples = RHISampleCount::IE_RHI_SAMPLE_COUNT_1_BIT;
				mRPAtt[2].EndLayout = RHILayoutType::IE_RHI_LAYOUT_SHADER_READ_ONLY;
				mRPAtt[2].AttachmentIndex = 2;

				// Normal attachment (G-Buffer)
				mRPAtt[3].ColorFormat = RHIColorFormat::IE_RHI_FORMAT_R16G16B16A16_SFLOAT;
				mRPAtt[3].Type = RHIAttachmentType::IE_RHI_INPUT_ATTACHMENT;
				mRPAtt[3].LoadOp = RHILoadOperation::IE_RHI_CLEAR_OP;
				mRPAtt[3].StoreOp = RHIStoreOperation::IE_RHI_CLEAR_OP;
				mRPAtt[3].Samples = RHISampleCount::IE_RHI_SAMPLE_COUNT_1_BIT;
				mRPAtt[3].EndLayout = RHILayoutType::IE_RHI_LAYOUT_SHADER_READ_ONLY;
				mRPAtt[3].AttachmentIndex = 3;

				// Albedo attachment (G-Buffer)
				mRPAtt[4].ColorFormat = RHIColorFormat::IE_RHI_FORMAT_R8G8B8A8_SRGB;
				mRPAtt[4].Type = RHIAttachmentType::IE_RHI_INPUT_ATTACHMENT;
				mRPAtt[4].LoadOp = RHILoadOperation::IE_RHI_CLEAR_OP;
				mRPAtt[4].StoreOp = RHIStoreOperation::IE_RHI_CLEAR_OP;
				mRPAtt[4].Samples = RHISampleCount::IE_RHI_SAMPLE_COUNT_1_BIT;
				mRPAtt[4].EndLayout = RHILayoutType::IE_RHI_LAYOUT_SHADER_READ_ONLY;
				mRPAtt[4].AttachmentIndex = 4;

				// Material attachment (G-Buffer)
				mRPAtt[5].ColorFormat = RHIColorFormat::IE_RHI_FORMAT_R8G8B8A8_UNORM;
				mRPAtt[5].Type = RHIAttachmentType::IE_RHI_INPUT_ATTACHMENT;
				mRPAtt[5].LoadOp = RHILoadOperation::IE_RHI_CLEAR_OP;
				mRPAtt[5].StoreOp = RHIStoreOperation::IE_RHI_CLEAR_OP;
				mRPAtt[5].Samples = RHISampleCount::IE_RHI_SAMPLE_COUNT_1_BIT;
				mRPAtt[5].EndLayout = RHILayoutType::IE_RHI_LAYOUT_SHADER_READ_ONLY;
				mRPAtt[5].AttachmentIndex = 5;
			}

			p_MainAttachments = mRPAtt;

			// Output attachments of the first subpass (G-Buffer)
			std::vector<RHISubAttachment> mRSOA(5);
			{
				mRSOA[0].AttachmentIndex = 1;	// Depth
				mRSOA[1].AttachmentIndex = 2;	// Positions
				mRSOA[2].AttachmentIndex = 3;	// Normals
				mRSOA[3].AttachmentIndex = 4;	// Albedo
				mRSOA[4].AttachmentIndex = 5;	// Materials
			}

			// Output attachment of the second subpass	(Light)
			std::vector<RHISubAttachment> mRSOA2(1);
			{
				mRSOA2[0].AttachmentIndex = 0;	// Color
			}

			// Input attachments of the second subpass	(Light)
			std::vector<RHISubAttachment> mRSIA(4);
			{
				mRSIA[0].AttachmentIndex = 2;	// Positions
				mRSIA[1].AttachmentIndex = 3;	// Normals
				mRSIA[2].AttachmentIndex = 4;	// Albedo
				mRSIA[3].AttachmentIndex = 5;	// Materials
			}

			// Output attachment of the third subpass (Transparent)
			std::vector<RHISubAttachment> mRSOA3(2);
			{
				mRSOA3[0].AttachmentIndex = 0;	// Color
				mRSOA3[1].AttachmentIndex = 1;	// Depth
			}

			// Construction of the subpasses
			std::vector<RHISubpass> mSubpasses(3);
			{
				// First subpass (G-Buffer)
				mSubpasses[0].OutputAttachments = mRSOA.data();
				mSubpasses[0].OutputAttachmentsCount = 5;
				mSubpasses[0].PreviousSubpass = ~0U;
				mSubpasses[0].NextSubpass = 0;

				p_OpaquePassAttachmentCount = static_cast<unsigned int>(mRSOA.size() - 1);
				p_OpaquePassIndex = 0;

				// Second subpass (Light)
				mSubpasses[1].OutputAttachments = mRSOA2.data();
				mSubpasses[1].OutputAttachmentsCount = 1;
				mSubpasses[1].InputAttachments = mRSIA.data();
				mSubpasses[1].InputAttachmentsCount = 4;
				mSubpasses[1].PreviousSubpass = 0;
				mSubpasses[1].NextSubpass = 1;

				// Third subpass (Transparent)
				mSubpasses[2].OutputAttachments = mRSOA3.data();
				mSubpasses[2].OutputAttachmentsCount = 2;
				mSubpasses[2].PreviousSubpass = 1;
				mSubpasses[2].NextSubpass = 2;

				p_TransparentPassAttachmentCount = static_cast<unsigned int>(mRSOA3.size() - 1);
				p_TransparentPassIndex = 2;
			}

			// Complete deferred
			RHIRenderPass renderpass{};
			{
				renderpass.Attachments = mRPAtt.data();
				renderpass.AttachmentsCount = 6;
				renderpass.Subpasses = mSubpasses.data();
				renderpass.SubpassesCount = 3;
			}

			p_MainRenderPass = p_RenderingContext->InstantiateRenderPass(renderpass);

			// Pipeline Template
			RHIPipelineTemplate LitTemplateDFD{};
			LitTemplateDFD.PassType = RHIPipelinePass::IE_PIPELINE_LIT_DFD;
			LitTemplateDFD.Shaders.resize(2);
			LitTemplateDFD.Shaders[0].Shader = mg->Create<Shader>(FIND_PATH("VS_GBuffer.vert"));
			LitTemplateDFD.Shaders[1].Shader = mg->Create<Shader>(FIND_PATH("FS_GBuffer.frag"));

			p_RenderingContext->AddTemplate(RHIPipelinePass::IE_PIPELINE_LIT_DFD, LitTemplateDFD);

			// G-Buffer Pipeline
			RHIPipeline gBufferPipelineInfo{};
			{
				gBufferPipelineInfo.Template = LitTemplateDFD;
				gBufferPipelineInfo.EnableDepthTest = true;
				gBufferPipelineInfo.Subpass = 0;
				gBufferPipelineInfo.AttachmentCount = 4;
			}

			p_MainPipeline = p_RenderingContext->InstantiatePipeline(p_MainRenderPass, gBufferPipelineInfo);

			// Pipeline Template
			RHIPipelineTemplate litDFD{};
			litDFD.Shaders.resize(2);
			litDFD.Shaders[0].Shader = mg->Create<Shader>(FIND_PATH("VS_DeferredPBR.vert"));
			litDFD.Shaders[1].Shader = mg->Create<Shader>(FIND_PATH("FS_DeferredPBR.frag"));
			litDFD.BackendTemplate = true;

			// Light Pipeline
			RHIPipeline lightPipelineInfo{};
			{
				lightPipelineInfo.Template = litDFD;
				lightPipelineInfo.EnableDepthTest = true;
				lightPipelineInfo.Subpass = 1;
				lightPipelineInfo.AttachmentCount = 1;
				lightPipelineInfo.FrontFace = RHIFrontFace::IE_RHI_FACE_CLOCKWISE;
				lightPipelineInfo.ContainsInput = false;
			}

			m_LightPipeline = p_RenderingContext->InstantiatePipeline(p_MainRenderPass, lightPipelineInfo);

			// Pipeline Template
			RHIPipelineTemplate litTemplateFWD{};
			litTemplateFWD.PassType = RHIPipelinePass::IE_PIPELINE_LIT_FWD;
			litTemplateFWD.Shaders.resize(2);
			litTemplateFWD.Shaders[0].Shader = mg->Create<Shader>(FIND_PATH("VS_ForwardPBR.vert"));
			litTemplateFWD.Shaders[1].Shader = mg->Create<Shader>(FIND_PATH("FS_ForwardPBR.frag"));

			p_RenderingContext->AddTemplate(RHIPipelinePass::IE_PIPELINE_LIT_FWD, litTemplateFWD);

			// Transparent Pipeline
			RHIPipeline transparentipelineInfo{};
			{
				transparentipelineInfo.Template = litTemplateFWD;
				transparentipelineInfo.Subpass = 2;
				transparentipelineInfo.AttachmentCount = 1;
				transparentipelineInfo.FrontFace = RHIFrontFace::IE_RHI_FACE_COUNTER_CLOCKWISE;
				transparentipelineInfo.BlendType = RHIBlendType::IE_RHI_BLEND_TYPE_ALPHA_BLEND;
			}

			p_TransparentPipeline = p_RenderingContext->InstantiatePipeline(p_MainRenderPass, transparentipelineInfo);

			// Pipeline Template
			RHIPipelineTemplate gridTemplate{};
			gridTemplate.Shaders.resize(2);
			gridTemplate.Shaders[0].Shader = mg->Create<Shader>(FIND_PATH("VS_GridShader.vert"));
			gridTemplate.Shaders[1].Shader = mg->Create<Shader>(FIND_PATH("FS_GridShader.frag"));
			gridTemplate.BackendTemplate = true;

			// Grid Pipeline
			RHIPipeline gridPipelineInfo{};
			{
				gridPipelineInfo.Template = gridTemplate;
				gridPipelineInfo.FrontFace = RHIFrontFace::IE_RHI_FACE_CLOCKWISE;
				gridPipelineInfo.FaceCulling = RHIFaceCulling::IE_RHI_FACE_CULLING_FRONT;
				gridPipelineInfo.BlendType = RHIBlendType::IE_RHI_BLEND_TYPE_ALPHA_BLEND;
				gridPipelineInfo.EnableDepthTest = true;
				gridPipelineInfo.EnableDepthWrite = false;
				gridPipelineInfo.Subpass = 2;
				gridPipelineInfo.AttachmentCount = 1;
			}

			p_GridPipeline = p_RenderingContext->InstantiatePipeline(p_MainRenderPass, gridPipelineInfo);

			RHIPipelineTemplate skyboxTemplate{};
			skyboxTemplate.Shaders.resize(2);
			skyboxTemplate.Shaders[0].Shader = mg->Create<Shader>(FIND_PATH("VS_Skybox.vert"));
			skyboxTemplate.Shaders[1].Shader = mg->Create<Shader>(FIND_PATH("FS_Skybox.frag"));
			skyboxTemplate.BackendTemplate = true;

			RHIPipeline skyboxPipelineInfo{};
			{
				skyboxPipelineInfo.Template = skyboxTemplate;
				skyboxPipelineInfo.FaceCulling = RHIFaceCulling::IE_RHI_FACE_CULLING_BACK;
				skyboxPipelineInfo.EnableDepthTest = true;
				skyboxPipelineInfo.EnableDepthWrite = false;
				skyboxPipelineInfo.Subpass = 2;
				skyboxPipelineInfo.AttachmentCount = 1;
				skyboxPipelineInfo.CompareOperation = RHICompareOperation::IE_RHI_COMPARE_OP_LESS_OR_EQUAL;
			}

			p_SkyboxPipeline = p_RenderingContext->InstantiatePipeline(p_MainRenderPass, skyboxPipelineInfo);

			// Pipeline Template
			RHIPipelineTemplate wireframeTemplate{};
			wireframeTemplate.Shaders.resize(2);
			wireframeTemplate.Shaders[0].Shader = mg->Create<Shader>(FIND_PATH("VS_WireframeShader.vert"));
			wireframeTemplate.Shaders[1].Shader = mg->Create<Shader>(FIND_PATH("FS_WireframeShader.frag"));
			wireframeTemplate.BackendTemplate = true;

			RHIPipeline wfPipelineInfo{};
			{
				wfPipelineInfo.Template = wireframeTemplate;
				wfPipelineInfo.FrontFace = RHIFrontFace::IE_RHI_FACE_CLOCKWISE;
				wfPipelineInfo.FaceCulling = RHIFaceCulling::IE_RHI_FACE_CULLING_NONE;
				wfPipelineInfo.PolygonMode = RHIPolygonMode::IE_RHI_POLYGON_MODE_LINE;
				wfPipelineInfo.EnableDepthTest = true;
				wfPipelineInfo.CompareOperation = RHICompareOperation::IE_RHI_COMPARE_OP_ALWAYS;
				wfPipelineInfo.Subpass = 2;
				wfPipelineInfo.AttachmentCount = 1;
			}

			p_WireframePipeline = p_RenderingContext->InstantiatePipeline(p_MainRenderPass, wfPipelineInfo);
				
			SceneLightData data{};
			p_LightBuffers = _resourceContext->CreateBuffers(RHIBufferType::IE_RHI_UNIFORM_BUFFER, sizeof(data), &data);
			p_LightBufferDescriptors = _resourceContext->CreateBufferDescriptors(m_LightPipeline->GetDescriptorLayout(p_LightLayoutIndex), p_LightBuffers, sizeof(data), 0);

			IRenderer::Initialize(_renderingContext, _resourceContext);

			return RHIResult();
		}

		RHIResult DeferredRenderer::Terminate()
		{
			IRenderer::Terminate();

			p_ResourceContext->DestroyBuffers(p_LightBuffers);
			p_ResourceContext->DeleteDescriptors(p_LightBufferDescriptors);

			p_RenderingContext->DestroyPipeline(p_SkyboxPipeline);
			p_RenderingContext->DestroyPipeline(p_WireframePipeline);

			p_RenderingContext->DestroyPipeline(p_MainPipeline);

			p_RenderingContext->DestroyPipeline(m_LightPipeline);

			p_RenderingContext->DestroyPipeline(p_GridPipeline);

			p_RenderingContext->DestroyPipeline(p_TransparentPipeline);

			p_RenderingContext->DestroyRenderPass(p_MainRenderPass);

			p_RenderingContext = nullptr;
			return RHIResult();
		}

		std::vector<IDescriptor*> DeferredRenderer::OnFramebufferResize(const std::vector<IFramebuffer*>& _framebuffers, std::vector<IDescriptor*>& _framebufferDescriptors)
		{
			std::vector<IDescriptor*> descriptors(MAX_FRAMES_IN_FLIGHT);

			if (_framebufferDescriptors.size() > 0)
				p_ResourceContext->DeleteDescriptors(_framebufferDescriptors);

			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
			{
				descriptors[i] = p_ResourceContext->CreateAttachmentsDescriptor(m_LightPipeline->GetDescriptorLayout(1), _framebuffers[i], 2, 5);
			}

			return descriptors;
		}

		void DeferredRenderer::StartRenderingOpaque(const CameraComponent& _camera, const SkyboxObject* _skybox)
		{
			_skybox;
			unsigned int width = _camera.GetCameraFramebuffers()[0]->GetWidth(), height = _camera.GetCameraFramebuffers()[0]->GetHeight();

			p_CurrentCommandBuffer->BeginDebugLabel(p_RenderingContext->GetDevice(), "Deferred Rendering", Vector3(1.f, 0.5f, 0.5f));

			StartRenderPass(p_MainRenderPass, _camera.GetCameraFramebuffers(), width, height);

			p_CurrentCommandBuffer->BeginDebugLabel(p_RenderingContext->GetDevice(), "Opaque Objects Pass", Vector3(0.5f, 1.f, 0.5f));

			BindPipeline(p_MainPipeline, width, height, true);
			
			BindDescriptor(p_CurrentPipeline, _camera.GetDescriptors(), p_CameraLayoutIndex);
		}

		void DeferredRenderer::EndRenderingOpaque(const CameraComponent& _camera, const SkyboxObject* _skybox)
		{
			unsigned int width = _camera.GetCameraFramebuffers()[0]->GetWidth(), height = _camera.GetCameraFramebuffers()[0]->GetHeight();

			p_CurrentCommandBuffer->EndDebugLabel(p_RenderingContext->GetDevice());

			ISwapChain* sc = p_RenderingContext->GetSwapChain();
			unsigned int cFrame = sc->GetCurrentFrame();
			ICommandBuffer* cb = p_RenderingContext->GetCommandBuffer(cFrame);

			p_CurrentCommandBuffer->BeginDebugLabel(p_RenderingContext->GetDevice(), "Lightning Pass", Vector3(0.984f, 0.988f, 0.49f));

			cb->NextSubPass();

			BindPipeline(m_LightPipeline, width, height, true);

			BindDescriptor(p_CurrentPipeline, _camera.GetFramebufferDescriptors(), 1);

			BindDescriptor(p_CurrentPipeline, p_LightBufferDescriptors, p_LightLayoutIndex);

			BindDescriptor(p_CurrentPipeline, _camera.GetDescriptors(), p_CameraLayoutIndex);

			BindDescriptor(p_CurrentPipeline, p_ShadowsDescriptors, p_ShadowLayoutIndex);

			BindDescriptor(p_CurrentPipeline, _skybox->GetIBLDescriptor(), 4);

			Draw(p_CurrentCommandBuffer, 6);

			p_CurrentCommandBuffer->EndDebugLabel(p_RenderingContext->GetDevice());

			p_CurrentCommandBuffer->BeginDebugLabel(p_RenderingContext->GetDevice(), "Transparent Objects Pass", Vector3(0.5f, 0.5f, 1.f));

			p_CurrentCommandBuffer->NextSubPass();
		}

		void DeferredRenderer::StartRenderingTransparent(const CameraComponent& _camera, const SkyboxObject* _skybox)
		{
			unsigned int width = _camera.GetCameraFramebuffers()[0]->GetWidth(), height = _camera.GetCameraFramebuffers()[0]->GetHeight();

			if (_camera.GetDrawGrid())
			{
				BindPipeline(p_GridPipeline, width, height, true);

				ExtendedCamDataUBO camData{};
				camData.View = _camera.GetView();
				camData.Projection = _camera.GetProjection();

				p_CurrentCommandBuffer->BindBlockVariable(p_GridPipeline, sizeof(camData), &camData, RHIShaderType::IE_RHI_SHADER_TYPE_VERTEX);

				Draw(p_CurrentCommandBuffer, 6);
			}

			BindPipeline(p_TransparentPipeline, width, height, true);

			BindDescriptor(p_CurrentPipeline, _camera.GetDescriptors(), 0);

			BindDescriptor(p_CurrentPipeline, p_LightBufferDescriptors, 3);

			BindDescriptor(p_CurrentPipeline, p_ShadowsDescriptors, 4);

			BindDescriptor(p_CurrentPipeline, _skybox->GetIBLDescriptor(), 5);
		}

		void DeferredRenderer::EndRenderingTransparent(const CameraComponent& _camera)
		{
			_camera;

			p_CurrentCommandBuffer->EndDebugLabel(p_RenderingContext->GetDevice());
			EndRenderPass();
			p_CurrentCommandBuffer->EndDebugLabel(p_RenderingContext->GetDevice());
		}

		void DeferredRenderer::StartDrawCommand(IPipeline* _pipeline, const CameraComponent& _camera, SkyboxObject* _skybox, bool _isOpaque)
		{
			if (p_CurrentPipeline == _pipeline)
				return;

			BindPipeline(_pipeline, lastWidth, lastHeight, true);

			if (_isOpaque)
			{
				BindDescriptor(p_CurrentPipeline, _camera.GetDescriptors(), p_CameraLayoutIndex);
			}
			else
			{
				BindDescriptor(p_CurrentPipeline, _camera.GetDescriptors(), 0);

				BindDescriptor(p_CurrentPipeline, p_LightBufferDescriptors, 3);

				BindDescriptor(p_CurrentPipeline, p_ShadowsDescriptors, 4);

				BindDescriptor(p_CurrentPipeline, _skybox->GetIBLDescriptor(), 5);
			}
		}
	}
}