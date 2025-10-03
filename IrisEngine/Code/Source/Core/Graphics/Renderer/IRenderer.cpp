#include "Core/Graphics/Renderer/IRenderer.h"

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
#include "Core/Graphics/RHI/IImage.h"
#include "Resources/Texture.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"

#include "Resources/ResourceManager.h"
#include "Core/Tools/FilePath.h"
#include "SECS/Components/CameraComponent.h"

namespace Iris
{
	namespace Engine
	{
		RHIResult IRenderer::Initialize(RenderingContext* _renderingContext, ResourceContext* _resourceContext)
		{	
			p_RenderingContext = _renderingContext;
			p_ResourceContext = _resourceContext;

			ResourceManager* mg = ResourceManager::GetInstance();

			/////

			std::vector<RHIAttachment> sRPAtt(1);
			{
				sRPAtt[0].Type = RHIAttachmentType::IE_RHI_DEPTH_ATTACHMENT;
				sRPAtt[0].ColorFormat = RHIColorFormat::IE_RHI_FORMAT_D32_SFLOAT;
				sRPAtt[0].LoadOp = RHILoadOperation::IE_RHI_CLEAR_OP;
				sRPAtt[0].StoreOp = RHIStoreOperation::IE_RHI_STORE_OP;
				sRPAtt[0].Samples = RHISampleCount::IE_RHI_SAMPLE_COUNT_1_BIT;
				sRPAtt[0].EndLayout = RHILayoutType::IE_RHI_LAYOUT_SHADER_READ_ONLY;
				sRPAtt[0].AttachmentIndex = 0;
			}

			std::vector<RHISubAttachment> sRSOA(1);
			{
				sRSOA[0].AttachmentIndex = 0;
			}

			std::vector<RHISubpass> sSubpasses(1);
			{
				sSubpasses[0].OutputAttachmentsCount = 1;
				sSubpasses[0].OutputAttachments = sRSOA.data();
				sSubpasses[0].PreviousSubpass = ~0U;
				sSubpasses[0].NextSubpass = 0;
			}

			RHIRenderPass sRenderpass{};
			{
				sRenderpass.AttachmentsCount = 1;
				sRenderpass.Attachments = sRPAtt.data();
				sRenderpass.SubpassesCount = 1;
				sRenderpass.Subpasses = sSubpasses.data();
			}

			p_ShadowRP = p_RenderingContext->InstantiateRenderPass(sRenderpass);

			RHIPipelineTemplate shadowTemplate{};
			shadowTemplate.Shaders.resize(1);
			shadowTemplate.Shaders[0].Shader = mg->Create<Shader>(FIND_PATH("VS_DirectionalShadowMap.vert"));
			shadowTemplate.BackendTemplate = true;

			RHIPipeline sPipelineInfo{};
			{
				sPipelineInfo.Template = shadowTemplate;
				sPipelineInfo.FaceCulling = RHIFaceCulling::IE_RHI_FACE_CULLING_FRONT;
				sPipelineInfo.EnableDepthTest = true;
				sPipelineInfo.EnableDepthWrite = true;
				sPipelineInfo.Subpass = 0;
				sPipelineInfo.AttachmentCount = 1;
			}

			p_DirectionalShadowPipeline = p_RenderingContext->InstantiatePipeline(p_ShadowRP, sPipelineInfo);

			std::vector<RHIAttachment> spRPAtt(1);
			{
				spRPAtt[0].Type = RHIAttachmentType::IE_RHI_DEPTH_ATTACHMENT;
				spRPAtt[0].ColorFormat = RHIColorFormat::IE_RHI_FORMAT_D32_SFLOAT;
				spRPAtt[0].LoadOp = RHILoadOperation::IE_RHI_CLEAR_OP;
				spRPAtt[0].StoreOp = RHIStoreOperation::IE_RHI_STORE_OP;
				spRPAtt[0].Samples = RHISampleCount::IE_RHI_SAMPLE_COUNT_1_BIT;
				spRPAtt[0].EndLayout = RHILayoutType::IE_RHI_LAYOUT_SHADER_READ_ONLY;
				spRPAtt[0].FramebufferType = RHITextureType::IE_RHI_TEXTURE_CUBEMAP;
				spRPAtt[0].AttachmentIndex = 0;
			}

			std::vector<RHISubAttachment> spRSOA(1);
			{
				spRSOA[0].AttachmentIndex = 0;
			}

			std::vector<RHISubpass> spSubpasses(1);
			{
				spSubpasses[0].OutputAttachmentsCount = 1;
				spSubpasses[0].OutputAttachments = spRSOA.data();
				spSubpasses[0].PreviousSubpass = ~0U;
				spSubpasses[0].NextSubpass = 0;
			}

			RHIRenderPass spRenderpass{};
			{
				spRenderpass.AttachmentsCount = 1;
				spRenderpass.Attachments = spRPAtt.data();
				spRenderpass.SubpassesCount = 1;
				spRenderpass.Subpasses = spSubpasses.data();
			}

			p_PointShadowRP = _renderingContext->InstantiateRenderPass(spRenderpass);

			RHIPipelineTemplate pointShadowTemplate{};
			pointShadowTemplate.Shaders.resize(3);
			pointShadowTemplate.Shaders[0].Shader = mg->Create<Shader>(FIND_PATH("VS_PointShadowMap.vert"));
			pointShadowTemplate.Shaders[1].Shader = mg->Create<Shader>(FIND_PATH("GS_PointShadowMap.geo"));
			pointShadowTemplate.Shaders[2].Shader = mg->Create<Shader>(FIND_PATH("FS_PointShadowMap.frag"));
			pointShadowTemplate.BackendTemplate = true;


			RHIPipeline psPipelineInfo{};
			{
				psPipelineInfo.Template = pointShadowTemplate;
				psPipelineInfo.FaceCulling = RHIFaceCulling::IE_RHI_FACE_CULLING_FRONT;
				psPipelineInfo.EnableDepthTest = true;
				psPipelineInfo.EnableDepthWrite = true;
				psPipelineInfo.Subpass = 0;
				psPipelineInfo.AttachmentCount = 1;
			}

			p_PointShadowPipeline = p_RenderingContext->InstantiatePipeline(p_PointShadowRP, psPipelineInfo);

			/////

			std::vector<RHIAttachment> panoramicCubeRPAtt(1);
			{
				panoramicCubeRPAtt[0].Type = RHIAttachmentType::IE_RHI_COLOR_ATTACHMENT;
				panoramicCubeRPAtt[0].ColorFormat = RHIColorFormat::IE_RHI_FORMAT_R32G32B32A32_SFLOAT;
				panoramicCubeRPAtt[0].LoadOp = RHILoadOperation::IE_RHI_CLEAR_OP;
				panoramicCubeRPAtt[0].StoreOp = RHIStoreOperation::IE_RHI_STORE_OP;
				panoramicCubeRPAtt[0].Samples = RHISampleCount::IE_RHI_SAMPLE_COUNT_1_BIT;
				panoramicCubeRPAtt[0].EndLayout = RHILayoutType::IE_RHI_LAYOUT_SHADER_READ_ONLY;
				panoramicCubeRPAtt[0].AttachmentIndex = 0;
			}

			std::vector<RHISubAttachment> panoramicCubeRSOA(1);
			{
				panoramicCubeRSOA[0].AttachmentIndex = 0;
			}

			std::vector<RHISubpass> panoramicCubeSubpasses(1);
			{
				panoramicCubeSubpasses[0].OutputAttachmentsCount = 1;
				panoramicCubeSubpasses[0].OutputAttachments = panoramicCubeRSOA.data();
				panoramicCubeSubpasses[0].PreviousSubpass = ~0U;
				panoramicCubeSubpasses[0].NextSubpass = 0;
			}

			RHIRenderPass panoramicCubeRenderpass{};
			{
				panoramicCubeRenderpass.AttachmentsCount = 1;
				panoramicCubeRenderpass.Attachments = panoramicCubeRPAtt.data();
				panoramicCubeRenderpass.SubpassesCount = 1;
				panoramicCubeRenderpass.Subpasses = panoramicCubeSubpasses.data();
			}

			m_PanoramicToCubeRP = p_RenderingContext->InstantiateRenderPass(panoramicCubeRenderpass);

			RHIPipelineTemplate panoramicCubeTemplate{};
			panoramicCubeTemplate.Shaders.resize(3);
			panoramicCubeTemplate.Shaders[0].Shader = mg->Create<Shader>(FIND_PATH("VS_PanoramicToCube.vert"));
			panoramicCubeTemplate.Shaders[1].Shader = mg->Create<Shader>(FIND_PATH("GS_PanoramicToCube.geo"));
			panoramicCubeTemplate.Shaders[2].Shader = mg->Create<Shader>(FIND_PATH("FS_PanoramicToCube.frag"));
			panoramicCubeTemplate.BackendTemplate = true;

			RHIPipeline panoramicCubePipelineInfo{};
			{
				panoramicCubePipelineInfo.Template = panoramicCubeTemplate;
				panoramicCubePipelineInfo.FaceCulling = RHIFaceCulling::IE_RHI_FACE_CULLING_FRONT;
				panoramicCubePipelineInfo.EnableDepthTest = false;
				panoramicCubePipelineInfo.EnableDepthWrite = false;
				panoramicCubePipelineInfo.Subpass = 0;
				panoramicCubePipelineInfo.AttachmentCount = 1;
			}

			m_PanoramicToCube = p_RenderingContext->InstantiatePipeline(m_PanoramicToCubeRP, panoramicCubePipelineInfo);

			RHIPipelineTemplate iblIrradianceTemplate{};
			iblIrradianceTemplate.Shaders.resize(3);
			iblIrradianceTemplate.Shaders[0].Shader = mg->Create<Shader>(FIND_PATH("VS_PanoramicToCube.vert"));
			iblIrradianceTemplate.Shaders[1].Shader = mg->Create<Shader>(FIND_PATH("GS_PanoramicToCube.geo"));
			iblIrradianceTemplate.Shaders[2].Shader = mg->Create<Shader>(FIND_PATH("FS_IBLIrradianceMapping.frag"));
			iblIrradianceTemplate.BackendTemplate = true;

			RHIPipeline iblIrradiancePipeline{};
			{
				iblIrradiancePipeline.Template = iblIrradianceTemplate;
				iblIrradiancePipeline.FaceCulling = RHIFaceCulling::IE_RHI_FACE_CULLING_FRONT;
				iblIrradiancePipeline.EnableDepthTest = true;
				iblIrradiancePipeline.EnableDepthWrite = false;
				iblIrradiancePipeline.Subpass = 0;
				iblIrradiancePipeline.AttachmentCount = 1;
			}

			m_IBLIrradiancePipeline = p_RenderingContext->InstantiatePipeline(m_PanoramicToCubeRP, iblIrradiancePipeline);

			RHIPipelineTemplate iblPrefilteredTemplate{};
			iblPrefilteredTemplate.Shaders.resize(3);
			iblPrefilteredTemplate.Shaders[0].Shader = mg->Create<Shader>(FIND_PATH("VS_PanoramicToCube.vert"));
			iblPrefilteredTemplate.Shaders[1].Shader = mg->Create<Shader>(FIND_PATH("GS_PanoramicToCube.geo"));
			iblPrefilteredTemplate.Shaders[2].Shader = mg->Create<Shader>(FIND_PATH("FS_IBLPrefilter.frag"));
			iblPrefilteredTemplate.BackendTemplate = true;

			RHIPipeline iblPrefilteredPipeline{};
			{
				iblPrefilteredPipeline.Template = iblPrefilteredTemplate;
				iblPrefilteredPipeline.FaceCulling = RHIFaceCulling::IE_RHI_FACE_CULLING_FRONT;
				iblPrefilteredPipeline.EnableDepthTest = true;
				iblPrefilteredPipeline.EnableDepthWrite = false;
				iblPrefilteredPipeline.Subpass = 0;
				iblPrefilteredPipeline.AttachmentCount = 1;
			}

			m_IBLPrefilteredPipeline = p_RenderingContext->InstantiatePipeline(m_PanoramicToCubeRP, iblPrefilteredPipeline);

			std::vector<RHIAttachment> lookUpTextureIBLRP(1);
			{
				lookUpTextureIBLRP[0].Type = RHIAttachmentType::IE_RHI_COLOR_ATTACHMENT;
				lookUpTextureIBLRP[0].ColorFormat = RHIColorFormat::IE_RHI_FORMAT_R32G32B32A32_SFLOAT;
				lookUpTextureIBLRP[0].LoadOp = RHILoadOperation::IE_RHI_CLEAR_OP;
				lookUpTextureIBLRP[0].StoreOp = RHIStoreOperation::IE_RHI_STORE_OP;
				lookUpTextureIBLRP[0].Samples = RHISampleCount::IE_RHI_SAMPLE_COUNT_1_BIT;
				lookUpTextureIBLRP[0].EndLayout = RHILayoutType::IE_RHI_LAYOUT_SHADER_READ_ONLY;
				lookUpTextureIBLRP[0].AttachmentIndex = 0;
			}

			std::vector<RHISubAttachment> lookUpTextureRSOA(1);
			{
				lookUpTextureRSOA[0].AttachmentIndex = 0;
			}

			std::vector<RHISubpass> lookUpTextureSubpass(1);
			{
				lookUpTextureSubpass[0].OutputAttachmentsCount = 1;
				lookUpTextureSubpass[0].OutputAttachments = lookUpTextureRSOA.data();
				lookUpTextureSubpass[0].PreviousSubpass = ~0U;
				lookUpTextureSubpass[0].NextSubpass = 0;
			}

			RHIRenderPass lookUpTextureRP{};
			{
				lookUpTextureRP.AttachmentsCount = 1;
				lookUpTextureRP.Attachments = lookUpTextureIBLRP.data();
				lookUpTextureRP.SubpassesCount = 1;
				lookUpTextureRP.Subpasses = lookUpTextureSubpass.data();
			}

			m_IBLLookUpTextureRP = p_RenderingContext->InstantiateRenderPass(lookUpTextureRP);

			RHIPipelineTemplate iblLookUpTemplate{};
			iblLookUpTemplate.Shaders.resize(2);
			iblLookUpTemplate.Shaders[0].Shader = mg->Create<Shader>(FIND_PATH("VS_IBLLookUpTexture.vert"));
			iblLookUpTemplate.Shaders[1].Shader = mg->Create<Shader>(FIND_PATH("FS_IBLLookUpTexture.frag"));
			iblLookUpTemplate.BackendTemplate = true;

			RHIPipeline iblLookUpPipeline{};
			{
				iblLookUpPipeline.Template = iblLookUpTemplate;
				iblLookUpPipeline.FaceCulling = RHIFaceCulling::IE_RHI_FACE_CULLING_FRONT;
				iblLookUpPipeline.EnableDepthTest = true;
				iblLookUpPipeline.EnableDepthWrite = false;
				iblLookUpPipeline.Subpass = 0;
				iblLookUpPipeline.AttachmentCount = 1;
				iblLookUpPipeline.ContainsInput = false;
			}

			m_IBLLookUpTexturePipeline = p_RenderingContext->InstantiatePipeline(m_IBLLookUpTextureRP, iblLookUpPipeline);

			std::vector<ShadowMap*> shadowMaps{};

			p_ShadowsDescriptors = p_ResourceContext->CreateShadowMapDescriptors(GetLightPipeline()->GetDescriptorLayout(p_ShadowLayoutIndex), shadowMaps, shadowMaps, shadowMaps);

			return RHIResult();
		}

		RHIResult IRenderer::Terminate()
		{
			p_ResourceContext->DeleteDescriptors(p_ShadowsDescriptors);
			p_RenderingContext->DestroyPipeline(p_DirectionalShadowPipeline);
			p_RenderingContext->DestroyPipeline(p_PointShadowPipeline);
			p_RenderingContext->DestroyPipeline(m_PanoramicToCube);
			p_RenderingContext->DestroyPipeline(m_IBLIrradiancePipeline);
			p_RenderingContext->DestroyPipeline(m_IBLPrefilteredPipeline);
			p_RenderingContext->DestroyPipeline(m_IBLLookUpTexturePipeline);
			p_RenderingContext->DestroyRenderPass(p_ShadowRP);
			p_RenderingContext->DestroyRenderPass(p_PointShadowRP);
			p_RenderingContext->DestroyRenderPass(m_PanoramicToCubeRP);
			p_RenderingContext->DestroyRenderPass(m_IBLLookUpTextureRP);

			return RHIResult::IE_RHI_SUCCESS;
		}

		void IRenderer::StartFrame(IWindow* _window, IRenderPass* _renderPass)
		{
			p_CurrentFrame = p_SwapChain->GetCurrentFrame();
			p_CurrentCommandBuffer = p_RenderingContext->GetCommandBuffer(p_CurrentFrame);

			p_SwapChain->StartRenderingFrame(_window, p_RenderingContext->GetDevice(), _renderPass);
			p_CurrentCommandBuffer->RecordCommandBuffer(p_CurrentFrame);
		}

		void IRenderer::EndFrame(IWindow* _window, IRenderPass* _renderPass)
		{
			p_CurrentCommandBuffer->EndCommandBuffer();
			p_SwapChain->EndRenderingFrame(_window, p_RenderingContext->GetDevice(), p_CurrentCommandBuffer, _renderPass, p_SwapChain->GetCurrentSwapChainImage());
		}

		void IRenderer::StartRenderPass(IRenderPass* _renderPass, const std::vector<IFramebuffer*>& _framebuffer, unsigned int _width, unsigned int _height)
		{
			std::vector<RHIClearColor> clear(_renderPass->GetAttachmentCount());
			if(_renderPass->GetAttachmentCount() == 1)
				clear[0].ColorDepthValue = Vector2(1.f, 1.f);
			else
				clear[1].ColorDepthValue = Vector2(1.f, 1.f);

			p_CurrentCommandBuffer->StartRenderPass(_renderPass, _framebuffer[p_CurrentFrame], _width, _height, clear);
		}

		void IRenderer::StartRenderPass(IRenderPass* _renderPass, IFramebuffer* _framebuffer, unsigned int _width, unsigned int _height)
		{
			m_CurrentRenderPass = _renderPass;

			std::vector<RHIClearColor> clear(_renderPass->GetAttachmentCount());
			clear[1].ColorDepthValue = Vector2(1.f, 1.f);

			p_CurrentCommandBuffer->StartRenderPass(_renderPass, _framebuffer, _width, _height, clear);
		}

		void IRenderer::BindDescriptor(IPipeline* _pipeline, const std::vector<IDescriptor*>& _descriptors, unsigned int _set)
		{
			p_CurrentCommandBuffer->BindDescriptor(_descriptors[p_CurrentFrame], _pipeline, _set);
		}

		void IRenderer::BindDescriptor(IPipeline* _pipeline, IDescriptor* _descriptors, unsigned int _set)
		{
			p_CurrentCommandBuffer->BindDescriptor(_descriptors, _pipeline, _set);
		}

		void IRenderer::BindBlockVariable(IPipeline* _pipeline, size_t _size, void* _data, RHIShaderType _shaderBind)
		{
			p_CurrentCommandBuffer->BindBlockVariable(_pipeline, _size, _data, _shaderBind);
		}

		void IRenderer::DrawCollider(const Matrix4 _transform, const Vector4 _color, Mesh* _mesh)
		{
			WireframeUBO data{};
			data.Transform = _transform;
			data.Color = _color;

			p_CurrentCommandBuffer->BindBlockVariable(p_WireframePipeline, sizeof(data), &data, RHIShaderType::IE_RHI_SHADER_TYPE_VERTEX);

			DrawMesh(p_CurrentCommandBuffer, _mesh);
		}

		void IRenderer::DrawModel(IBuffer* _vertexBuffer, IBuffer* _indexBuffer, size_t _indicesCount, const Material* _material)
		{
			MaterialUBO data = _material->GetFixedData();
			p_CurrentCommandBuffer->BindDescriptor(_material->GetDescriptor(), p_CurrentPipeline, 2);
			p_CurrentCommandBuffer->BindBlockVariable(p_CurrentPipeline, sizeof(MaterialUBO), &data, RHIShaderType::IE_RHI_SHADER_TYPE_FRAGMENT);
			p_CurrentCommandBuffer->BindVertexBuffer(_vertexBuffer);
			p_CurrentCommandBuffer->BindIndexBuffer(_indexBuffer);
			p_CurrentCommandBuffer->DrawIndexed(_indicesCount);
		}

		void IRenderer::DrawMesh(ICommandBuffer* _cmdBuffer, Mesh* _mesh)
		{
			std::vector<ChildMesh> meshData = *_mesh->GetMeshData();

			for (size_t i = 0; i < meshData.size(); ++i)
			{
				_cmdBuffer->BindVertexBuffer(meshData[i].VertexBuffer);
				_cmdBuffer->BindIndexBuffer(meshData[i].IndexBuffer);
				_cmdBuffer->DrawIndexed(meshData[i].IndicesCount);
			}
		}

		void IRenderer::DrawModel(Mesh* _mesh, const std::vector<Material*> _materials)
		{
			std::vector<ChildMesh> meshData = *_mesh->GetMeshData();

			for (size_t i = 0; i < meshData.size(); ++i)
			{
				p_CurrentCommandBuffer->BindDescriptor(_materials[meshData[i].MaterialIndex]->GetDescriptor(), p_CurrentPipeline, 2);
				p_CurrentCommandBuffer->BindVertexBuffer(meshData[i].VertexBuffer);
				p_CurrentCommandBuffer->BindIndexBuffer(meshData[i].IndexBuffer);
				p_CurrentCommandBuffer->DrawIndexed(meshData[i].IndicesCount);
			}
		}

		void IRenderer::UpdateBuffer(const std::vector<IBuffer*>& _buffers, void* _newData, size_t _size)
		{
			_buffers[p_CurrentFrame]->UpdateUniformBuffer(p_RenderingContext->GetDevice(), _newData, _size);
		}

		void IRenderer::UpdateLightBuffer(void* _newData, size_t _size)
		{
			UpdateBuffer(p_LightBuffers, _newData, _size);
		}

		void IRenderer::EndRenderPass()
		{
			p_CurrentCommandBuffer->EndRenderPass();
		}

		void IRenderer::BindPipeline(IPipeline* _pipeline, unsigned int _width, unsigned int _height, bool _invert)
		{
			p_CurrentPipeline = _pipeline;

			lastWidth = _width;
			lastHeight = _height;

			p_CurrentCommandBuffer->BindPipeline(_pipeline);
			p_CurrentCommandBuffer->SetViewport(_width, _height, _invert);
			p_CurrentCommandBuffer->SetScissor(_width, _height);
		}

		void IRenderer::CreateTemplates()
		{

		}

		void IRenderer::StartRenderingShadows()
		{
			p_CurrentCommandBuffer->BeginDebugLabel(p_RenderingContext->GetDevice(), "Shadow Rendering", Vector3(0.25f));
		}

		void IRenderer::StartRenderingDirectionalShadow(LightComponent& _light)
		{
			ShadowMap sm = *_light.GetShadowMap();
			_light.UpdateShadowMapData(*this);

			StartRenderPass(p_ShadowRP, sm.GetFramebuffers(), sm.GetFramebuffers()[0]->GetWidth(), sm.GetFramebuffers()[0]->GetHeight());

			BindPipeline(p_DirectionalShadowPipeline, sm.GetFramebuffers()[0]->GetWidth(), sm.GetFramebuffers()[0]->GetHeight(), false);

			BindDescriptor(p_DirectionalShadowPipeline, sm.GetShadowDataDescriptors(), 0);
		}

		void IRenderer::StartRenderingPointShadow(LightComponent& _light)
		{
			ShadowMap sm = *_light.GetShadowMap();
			_light.UpdateShadowMapData(*this);

			StartRenderPass(p_PointShadowRP, sm.GetFramebuffers(), sm.GetFramebuffers()[0]->GetWidth(), sm.GetFramebuffers()[0]->GetHeight());

			BindPipeline(p_PointShadowPipeline, sm.GetFramebuffers()[0]->GetWidth(), sm.GetFramebuffers()[0]->GetHeight(), false);

			BindDescriptor(p_PointShadowPipeline, sm.GetShadowDataDescriptors(), 0);
		}

		void IRenderer::EndRenderingShadow()
		{
			EndRenderPass();
		}

		void IRenderer::EndRenderingShadows()
		{
			p_CurrentCommandBuffer->EndDebugLabel(p_RenderingContext->GetDevice());
		}

		void IRenderer::StartRenderingColliders(const CameraComponent& _camera)
		{
			unsigned int width = _camera.GetCameraFramebuffers()[0]->GetWidth(), height = _camera.GetCameraFramebuffers()[0]->GetHeight();

			p_CurrentCommandBuffer->BeginDebugLabel(p_RenderingContext->GetDevice(), "Collider Pass", Vector3(1.f, 0.5f, 0.f));
			BindPipeline(p_WireframePipeline, width, height, true);
			BindDescriptor(p_WireframePipeline, _camera.GetDescriptors(), p_CameraLayoutIndex);
		}

		void IRenderer::EndRenderingColliders()
		{
			p_CurrentCommandBuffer->EndDebugLabel(p_RenderingContext->GetDevice());
		}

		void IRenderer::UpdateShadowMaps(const std::vector<ShadowMap*>& _dirShadowMaps, const std::vector<ShadowMap*>& _spotShadowMaps, const std::vector<ShadowMap*>& _pointShadowMaps)
		{
			p_ShadowsDescriptors[p_CurrentFrame]->UpdateShadowDescriptors(p_RenderingContext->GetDevice(), _dirShadowMaps, _spotShadowMaps, _pointShadowMaps, p_CurrentFrame);
		}

		void IRenderer::StartRenderingSkybox(IDescriptor* _skybox, const CameraComponent& _camera)
		{
			unsigned int width = _camera.GetCameraFramebuffers()[0]->GetWidth(), height = _camera.GetCameraFramebuffers()[0]->GetHeight();

			BindPipeline(p_SkyboxPipeline, width, height, true);
			BindDescriptor(p_SkyboxPipeline, _camera.GetDescriptors(), p_CameraLayoutIndex);
			BindDescriptor(p_SkyboxPipeline, _skybox, 1);
			Mesh* cube = ResourceManager::GetInstance()->Get<Mesh>("DefaultCube.obj");

			DrawMesh(p_CurrentCommandBuffer, cube);
		}

		void IRenderer::EndRenderingSkybox(const CameraComponent& _camera)
		{
			_camera;
		}

		void IRenderer::RecreateSwapChain()
		{
			ISwapChain* sc = p_RenderingContext->GetSwapChain();

			sc->SetFrameResized(true);
		}

		void IRenderer::Draw(ICommandBuffer* _cmdBuffer, unsigned int _verticesCount)
		{
			_cmdBuffer->Draw(_verticesCount);
		}

		void IRenderer::ConvertPanoramicTextureToCubemap(Texture* _baseTexture, IImage* _newTexture)
		{
			IDescriptor* basetext = p_ResourceContext->CreateTextureDescriptor(m_PanoramicToCube->GetDescriptorLayout(1), _baseTexture);

			CubemapUBO cubemapData{};
			cubemapData.Projection = Matrix4::ProjectionPerspectiveMatrix(0.1f, 100.f, 1024.f/1024.f, 90.f * Deg2Rad);
			cubemapData.Views[0] = Matrix4::LookAtMatrix(Vector3::s_Zero, Vector3::s_Right, Vector3::s_Down);
			cubemapData.Views[1] = Matrix4::LookAtMatrix(Vector3::s_Zero, Vector3::s_Left, Vector3::s_Down);
			cubemapData.Views[2] = Matrix4::LookAtMatrix(Vector3::s_Zero, Vector3::s_Up, Vector3::s_Forward);
			cubemapData.Views[3] = Matrix4::LookAtMatrix(Vector3::s_Zero, Vector3::s_Down, Vector3::s_Back);
			cubemapData.Views[4] = Matrix4::LookAtMatrix(Vector3::s_Zero, Vector3::s_Forward, Vector3::s_Down);
			cubemapData.Views[5] = Matrix4::LookAtMatrix(Vector3::s_Zero, Vector3::s_Back, Vector3::s_Down);

			IBuffer* matricesBuffer = p_ResourceContext->CreateBuffer(RHIBufferType::IE_RHI_UNIFORM_BUFFER, sizeof(CubemapUBO), &cubemapData);
			IDescriptor* matricesDescriptor = p_ResourceContext->CreateBufferDescriptor(m_PanoramicToCube->GetDescriptorLayout(0), matricesBuffer, sizeof(CubemapUBO), 0);
			matricesBuffer->UpdateUniformBuffer(p_ResourceContext->GetDevice(), &cubemapData, sizeof(CubemapUBO));

			std::vector<IImage*> attachment(1);
			attachment[0] = _newTexture;

			IFramebuffer* fb = p_RenderingContext->CreateFramebuffer(m_PanoramicToCubeRP, 1024, 1024, attachment);

			ResourceManager* rm = ResourceManager::GetInstance();
			Mesh* cube = rm->Get<Mesh>("DefaultCube.obj");

			std::vector<RHIClearColor> clearColor(1);

			ICommandBuffer* cmd = p_RenderingContext->GetCommandAllocator()->BeginSingleTimeCommands(p_ResourceContext->GetDevice());

			cmd->StartRenderPass(m_PanoramicToCubeRP, fb, 1024, 1024, clearColor);
			cmd->BindPipeline(m_PanoramicToCube);
			cmd->SetViewport(1024, 1024, false);
			cmd->SetScissor(1024, 1024);
			cmd->BindDescriptor(matricesDescriptor, m_PanoramicToCube, 0);
			cmd->BindDescriptor(basetext, m_PanoramicToCube, 1);
			DrawMesh(cmd, cube);
			cmd->EndRenderPass();

			p_RenderingContext->GetCommandAllocator()->EndSingleTimeCommands(p_ResourceContext->GetDevice(), cmd);

			p_RenderingContext->DestroyFramebuffer(fb);
			p_ResourceContext->DeleteDescriptor(matricesDescriptor);
			p_ResourceContext->DeleteDescriptor(basetext);
			p_ResourceContext->DestroyBuffer(matricesBuffer);
		}

		void IRenderer::ComputeIBLEnvironmentdMap(Texture* _baseTexture, IImage* _newTexture)
		{
			IDescriptor* basetext = p_ResourceContext->CreateTextureDescriptor(m_PanoramicToCube->GetDescriptorLayout(1), _baseTexture);

			CubemapUBO cubemapData{};
			cubemapData.Projection = Matrix4::ProjectionPerspectiveMatrix(0.1f, 100.f, 1024.f / 1024.f, 90.f * Deg2Rad);
			cubemapData.Views[0] = Matrix4::LookAtMatrix(Vector3::s_Zero, Vector3::s_Right, Vector3::s_Down);
			cubemapData.Views[1] = Matrix4::LookAtMatrix(Vector3::s_Zero, Vector3::s_Left, Vector3::s_Down);
			cubemapData.Views[2] = Matrix4::LookAtMatrix(Vector3::s_Zero, Vector3::s_Up, Vector3::s_Forward);
			cubemapData.Views[3] = Matrix4::LookAtMatrix(Vector3::s_Zero, Vector3::s_Down, Vector3::s_Back);
			cubemapData.Views[4] = Matrix4::LookAtMatrix(Vector3::s_Zero, Vector3::s_Forward, Vector3::s_Down);
			cubemapData.Views[5] = Matrix4::LookAtMatrix(Vector3::s_Zero, Vector3::s_Back, Vector3::s_Down);

			IBuffer* matricesBuffer = p_ResourceContext->CreateBuffer(RHIBufferType::IE_RHI_UNIFORM_BUFFER, sizeof(CubemapUBO), &cubemapData);
			IDescriptor* matricesDescriptor = p_ResourceContext->CreateBufferDescriptor(m_PanoramicToCube->GetDescriptorLayout(0), matricesBuffer, sizeof(CubemapUBO), 0);
			matricesBuffer->UpdateUniformBuffer(p_ResourceContext->GetDevice(), &cubemapData, sizeof(CubemapUBO));

			std::vector<IImage*> attachment(1);
			attachment[0] = _newTexture;

			IFramebuffer* fb = p_RenderingContext->CreateFramebuffer(m_PanoramicToCubeRP, 1024, 1024, attachment);

			ResourceManager* rm = ResourceManager::GetInstance();
			Mesh* cube = rm->Get<Mesh>("DefaultCube.obj");

			std::vector<RHIClearColor> clearColor(1);

			ICommandBuffer* cmd = p_RenderingContext->GetCommandAllocator()->BeginSingleTimeCommands(p_ResourceContext->GetDevice());

			cmd->StartRenderPass(m_PanoramicToCubeRP, fb, 1024, 1024, clearColor);
			cmd->BindPipeline(m_IBLIrradiancePipeline);
			cmd->SetViewport(1024, 1024, false);
			cmd->SetScissor(1024, 1024);
			cmd->BindDescriptor(matricesDescriptor, m_IBLIrradiancePipeline, 0);
			cmd->BindDescriptor(basetext, m_IBLIrradiancePipeline, 1);
			DrawMesh(cmd, cube);
			cmd->EndRenderPass();

			p_RenderingContext->GetCommandAllocator()->EndSingleTimeCommands(p_ResourceContext->GetDevice(), cmd);

			p_RenderingContext->DestroyFramebuffer(fb);
			p_ResourceContext->DeleteDescriptor(matricesDescriptor);
			p_ResourceContext->DeleteDescriptor(basetext);
			p_ResourceContext->DestroyBuffer(matricesBuffer);
		}

		void IRenderer::ComputeIBLPrefilteredMap(Texture* _baseTexture, IImage* _newTexture)
		{
			IDescriptor* basetext = p_ResourceContext->CreateTextureDescriptor(m_PanoramicToCube->GetDescriptorLayout(1), _baseTexture);

			CubemapUBO cubemapData{};
			cubemapData.Projection = Matrix4::ProjectionPerspectiveMatrix(0.1f, 100.f, 1024.f / 1024.f, 90.f * Deg2Rad);
			cubemapData.Views[0] = Matrix4::LookAtMatrix(Vector3::s_Zero, Vector3::s_Right, Vector3::s_Down);
			cubemapData.Views[1] = Matrix4::LookAtMatrix(Vector3::s_Zero, Vector3::s_Left, Vector3::s_Down);
			cubemapData.Views[2] = Matrix4::LookAtMatrix(Vector3::s_Zero, Vector3::s_Up, Vector3::s_Forward);
			cubemapData.Views[3] = Matrix4::LookAtMatrix(Vector3::s_Zero, Vector3::s_Down, Vector3::s_Back);
			cubemapData.Views[4] = Matrix4::LookAtMatrix(Vector3::s_Zero, Vector3::s_Forward, Vector3::s_Down);
			cubemapData.Views[5] = Matrix4::LookAtMatrix(Vector3::s_Zero, Vector3::s_Back, Vector3::s_Down);

			IBuffer* matricesBuffer = p_ResourceContext->CreateBuffer(RHIBufferType::IE_RHI_UNIFORM_BUFFER, sizeof(CubemapUBO), &cubemapData);
			IDescriptor* matricesDescriptor = p_ResourceContext->CreateBufferDescriptor(m_PanoramicToCube->GetDescriptorLayout(0), matricesBuffer, sizeof(CubemapUBO), 0);
			matricesBuffer->UpdateUniformBuffer(p_ResourceContext->GetDevice(), &cubemapData, sizeof(CubemapUBO));

			unsigned int maxMipCount = _newTexture->GetMipCount();

			// We dont want to generate 5 mips but 4 because the first one is already up
			std::vector<IImage*> mips = _newTexture->GenerateFramebufferMipMaps(p_RenderingContext->GetDevice(), maxMipCount);

			ResourceManager* rm = ResourceManager::GetInstance();
			Mesh* cube = rm->Get<Mesh>("DefaultCube.obj");

			std::vector<RHIClearColor> clearColor(1);

			for (size_t i = 0; i < maxMipCount; ++i)
			{
				ICommandBuffer* cmd = p_RenderingContext->GetCommandAllocator()->BeginSingleTimeCommands(p_ResourceContext->GetDevice());
				unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, i));
				unsigned int mipHeight = mipWidth;

				std::vector<IImage*> attachment(1);
				attachment[0] = mips[i];

				IFramebuffer* fb = p_RenderingContext->CreateFramebuffer(m_PanoramicToCubeRP, mipWidth, mipHeight, attachment);

				float roughness = (float)i / (float)(maxMipCount - 1);

				cmd->StartRenderPass(m_PanoramicToCubeRP, fb, mipWidth, mipHeight, clearColor);
				cmd->BindPipeline(m_IBLPrefilteredPipeline);
				cmd->SetViewport(mipWidth, mipHeight, false);
				cmd->SetScissor(mipWidth, mipHeight);
				cmd->BindDescriptor(matricesDescriptor, m_IBLPrefilteredPipeline, 0);
				cmd->BindDescriptor(basetext, m_IBLPrefilteredPipeline, 1);
				cmd->BindBlockVariable(m_IBLPrefilteredPipeline, sizeof(float), &roughness, RHIShaderType::IE_RHI_SHADER_TYPE_FRAGMENT);
				DrawMesh(cmd, cube);
				cmd->EndRenderPass();

				p_RenderingContext->GetCommandAllocator()->EndSingleTimeCommands(p_ResourceContext->GetDevice(), cmd);
				p_RenderingContext->DestroyFramebuffer(fb);
			}

			_newTexture->DestroyFramebufferMipMaps(p_RenderingContext->GetDevice(), mips);

			p_ResourceContext->DeleteDescriptor(matricesDescriptor);
			p_ResourceContext->DeleteDescriptor(basetext);
			p_ResourceContext->DestroyBuffer(matricesBuffer);
		}

		void IRenderer::ComputeIBLLookUpTexture(IImage* _newTexture)
		{
			ICommandBuffer* cmd = p_RenderingContext->GetCommandAllocator()->BeginSingleTimeCommands(p_ResourceContext->GetDevice());

			std::vector<IImage*> attachment(1);
			attachment[0] = _newTexture;

			IFramebuffer* fb = p_RenderingContext->CreateFramebuffer(m_IBLLookUpTextureRP, 512, 512, attachment);

			std::vector<RHIClearColor> clearColor(1);

			cmd->StartRenderPass(m_IBLLookUpTextureRP, fb, 512, 512, clearColor);
			cmd->BindPipeline(m_IBLLookUpTexturePipeline);
			cmd->SetViewport(512, 512, false);
			cmd->SetScissor(512, 512);
			Draw(cmd, 6);
			cmd->EndRenderPass();

			p_RenderingContext->GetCommandAllocator()->EndSingleTimeCommands(p_ResourceContext->GetDevice(), cmd);
			p_RenderingContext->DestroyFramebuffer(fb);
		}
	}
}