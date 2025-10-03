#include "Core/Graphics/Renderer/ForwardRenderer.h"

#include "Core/IWindow.h"
#include "Core/Graphics/Context/RenderingContext.h"
#include "Core/Graphics/Context/ResourceContext.h"
#include "Resources/ResourceManager.h"
#include "Core/Tools/FilePath.h"
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

#include "Resources/Mesh.h"
#include "Resources/Material.h"
#include "SECS/LightData/LightData.h"
#include "SECS/Components/CameraComponent.h"
#include "SECS/Components/SkyboxObject.h"

namespace Iris
{
	namespace Engine
	{
		RHIResult ForwardRenderer::Initialize(RenderingContext* _renderingContext, ResourceContext* _resourceContext)
		{
			p_Type = RendererType::IE_RENDERER_FORWARD;
			p_RenderingContext = _renderingContext;
			p_ResourceContext = _resourceContext;

			p_SwapChain = p_RenderingContext->GetSwapChain();

			_resourceContext;

			p_RenderingContext = _renderingContext;

			IRenderer::CreateTemplates();

			ResourceManager* mg = ResourceManager::GetInstance();

			std::vector<RHIAttachment> mRPAtt(2);
			{
				mRPAtt[0].Type = RHIAttachmentType::IE_RHI_COLOR_ATTACHMENT;
				mRPAtt[0].ColorFormat = RHIColorFormat::IE_RHI_FORMAT_B8G8R8A8_UNORM;
				mRPAtt[0].LoadOp = RHILoadOperation::IE_RHI_CLEAR_OP;
				mRPAtt[0].StoreOp = RHIStoreOperation::IE_RHI_STORE_OP;
				mRPAtt[0].Samples = RHISampleCount::IE_RHI_SAMPLE_COUNT_1_BIT;
				mRPAtt[0].EndLayout = RHILayoutType::IE_RHI_LAYOUT_SHADER_READ_ONLY;
				mRPAtt[0].AttachmentIndex = 0;

				mRPAtt[1].Type = RHIAttachmentType::IE_RHI_DEPTH_ATTACHMENT;
				mRPAtt[1].ColorFormat = RHIColorFormat::IE_RHI_FORMAT_D32_SFLOAT;
				mRPAtt[1].LoadOp = RHILoadOperation::IE_RHI_CLEAR_OP;
				mRPAtt[1].StoreOp = RHIStoreOperation::IE_RHI_CLEAR_OP;
				mRPAtt[1].Samples = RHISampleCount::IE_RHI_SAMPLE_COUNT_1_BIT;
				mRPAtt[1].EndLayout = RHILayoutType::IE_RHI_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
				mRPAtt[1].AttachmentIndex = 1;
			}

			p_MainAttachments = mRPAtt;

			std::vector<RHISubAttachment> mRSOA(2);
			{
				mRSOA[0].AttachmentIndex = 0;
				mRSOA[1].AttachmentIndex = 1;
			}

			std::vector<RHISubpass> mSubpasses(1);
			{
				mSubpasses[0].OutputAttachments = mRSOA.data();
				mSubpasses[0].OutputAttachmentsCount = 2;
				mSubpasses[0].PreviousSubpass = ~0U;
				mSubpasses[0].NextSubpass = 0;
			}

			RHIRenderPass renderpass{};
			{
				renderpass.Attachments = mRPAtt.data();
				renderpass.AttachmentsCount = 2;
				renderpass.Subpasses = mSubpasses.data();
				renderpass.SubpassesCount = 1;

				p_OpaquePassAttachmentCount = static_cast<unsigned int>(mRSOA.size() - 1);
				p_OpaquePassIndex = 0;
				p_TransparentPassAttachmentCount = static_cast<unsigned int>(mRSOA.size() - 1);
				p_TransparentPassIndex = 0;
			}

			p_MainRenderPass = p_RenderingContext->InstantiateRenderPass(renderpass);

			// Pipeline Template
			RHIPipelineTemplate defaultLitTemplate{};
			defaultLitTemplate.Shaders.resize(2);
			defaultLitTemplate.Shaders[0].Shader = mg->Create<Shader>(FIND_PATH("VS_ForwardPBR.vert"));
			defaultLitTemplate.Shaders[1].Shader = mg->Create<Shader>(FIND_PATH("FS_ForwardPBR.frag"));

			p_RenderingContext->AddTemplate(RHIPipelinePass::IE_PIPELINE_LIT_FWD, defaultLitTemplate);

			RHIPipeline pipelineInfo{};
			{
				pipelineInfo.Template = defaultLitTemplate;
				pipelineInfo.BlendType = RHIBlendType::IE_RHI_BLEND_TYPE_NONE;
				pipelineInfo.EnableDepthTest = true;
				pipelineInfo.Subpass = 0;
				pipelineInfo.AttachmentCount = 1;
			}

			p_MainPipeline = p_RenderingContext->InstantiatePipeline(p_MainRenderPass, pipelineInfo);

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
				gridPipelineInfo.Subpass = 0;
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
				skyboxPipelineInfo.Subpass = 0;
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
				wfPipelineInfo.Subpass = 0;
				wfPipelineInfo.AttachmentCount = 1;
			}

			p_WireframePipeline = p_RenderingContext->InstantiatePipeline(p_MainRenderPass, wfPipelineInfo);

			SceneLightData data{};
			p_LightBuffers = _resourceContext->CreateBuffers(RHIBufferType::IE_RHI_UNIFORM_BUFFER, sizeof(data), &data);
			p_LightBufferDescriptors = _resourceContext->CreateBufferDescriptors(p_MainPipeline->GetDescriptorLayout(p_LightLayoutIndex), p_LightBuffers, sizeof(data), 0);

			IRenderer::Initialize(_renderingContext, _resourceContext);

			return RHIResult();
		}

		RHIResult ForwardRenderer::Terminate()
		{
			IRenderer::Terminate();

			p_ResourceContext->DestroyBuffers(p_LightBuffers);

			p_ResourceContext->DeleteDescriptors(p_LightBufferDescriptors);

			p_RenderingContext->DestroyPipeline(p_SkyboxPipeline);

			p_RenderingContext->DestroyPipeline(p_MainPipeline);

			p_RenderingContext->DestroyPipeline(p_GridPipeline);

			p_RenderingContext->DestroyPipeline(p_WireframePipeline);

			p_RenderingContext->DestroyRenderPass(p_MainRenderPass);

			p_RenderingContext = nullptr;

			return RHIResult();
		}

		void ForwardRenderer::StartRenderingOpaque(const CameraComponent& _camera, const SkyboxObject* _skybox)
		{
			unsigned int width = _camera.GetCameraFramebuffers()[0]->GetWidth(), height = _camera.GetCameraFramebuffers()[0]->GetHeight();

			p_CurrentCommandBuffer->BeginDebugLabel(p_RenderingContext->GetDevice(), "Forward Rendering", Vector3(1.f, 0.5f, 0.5f));

			StartRenderPass(p_MainRenderPass, _camera.GetCameraFramebuffers(), width, height);

			p_CurrentCommandBuffer->BeginDebugLabel(p_RenderingContext->GetDevice(), "Opaque Objects Pass", Vector3(0.5f, 1.f, 0.5f));

			BindPipeline(p_MainPipeline, width, height, true);

			BindDescriptor(p_CurrentPipeline, _camera.GetDescriptors(), 0);

			BindDescriptor(p_CurrentPipeline, p_LightBufferDescriptors, 3);

			BindDescriptor(p_CurrentPipeline, p_ShadowsDescriptors, 4);

			BindDescriptor(p_CurrentPipeline, _skybox->GetIBLDescriptor(), 5);
		}

		void ForwardRenderer::EndRenderingOpaque(const CameraComponent& _camera, const SkyboxObject* _skybox)
		{
			_camera;
			_skybox;
			p_CurrentCommandBuffer->EndDebugLabel(p_RenderingContext->GetDevice());
		}

		void ForwardRenderer::StartRenderingTransparent(const CameraComponent& _camera, const SkyboxObject* _skybox)
		{
			_skybox;
			_camera;
			p_CurrentCommandBuffer->BeginDebugLabel(p_RenderingContext->GetDevice(), "Transparent Objects Pass", Vector3(0.5f, 0.5f, 1.f));
		}

		void ForwardRenderer::EndRenderingTransparent(const CameraComponent& _camera)
		{
			unsigned int width = _camera.GetCameraFramebuffers()[0]->GetWidth(), height = _camera.GetCameraFramebuffers()[0]->GetHeight();

			BindPipeline(p_GridPipeline, width, height, true);

			ExtendedCamDataUBO camData{};
			camData.View = _camera.GetView();
			camData.Projection = _camera.GetProjection();

			p_CurrentCommandBuffer->BindBlockVariable(p_GridPipeline, sizeof(camData), &camData, RHIShaderType::IE_RHI_SHADER_TYPE_VERTEX);

			Draw(p_CurrentCommandBuffer, 6);

			p_CurrentCommandBuffer->EndRenderPass();

			p_CurrentCommandBuffer->EndDebugLabel(p_RenderingContext->GetDevice());
			p_CurrentCommandBuffer->EndDebugLabel(p_RenderingContext->GetDevice());
		}

		void ForwardRenderer::StartDrawCommand(IPipeline* _pipeline, const CameraComponent& _camera, SkyboxObject* _skybox, bool _isOpaque)
		{
			_isOpaque;

			if (p_CurrentPipeline == _pipeline)
				return;

			BindPipeline(_pipeline, lastWidth, lastHeight, true);

			BindDescriptor(p_CurrentPipeline, _camera.GetDescriptors(), 0);

			BindDescriptor(p_CurrentPipeline, p_LightBufferDescriptors, 3);

			BindDescriptor(p_CurrentPipeline, p_ShadowsDescriptors, 4);

			BindDescriptor(p_CurrentPipeline, _skybox->GetIBLDescriptor(), 5);
		}
	}
}