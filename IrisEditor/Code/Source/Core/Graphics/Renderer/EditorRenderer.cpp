#include "Core/Graphics/Renderer/EditorRenderer.h"

#include "Core/Graphics/Context/RenderingContext.h"
#include "Core/Graphics/RHI/IRenderInterface.h"
#include "Core/Graphics/RHI/CommonTypes.h"

namespace Iris
{
	namespace Editor
	{
		void EditorRenderer::Initialize(Engine::RenderingContext* _renderingContext)
		{
			m_RenderingContextReference = _renderingContext;

			std::vector<Engine::RHIAttachment> fRPAtt(2);
			fRPAtt[0].Type				= Engine::RHIAttachmentType::IE_RHI_COLOR_ATTACHMENT;
			fRPAtt[0].ColorFormat		= Engine::RHIColorFormat::IE_RHI_FORMAT_B8G8R8A8_SRGB;
			fRPAtt[0].LoadOp			= Engine::RHILoadOperation::IE_RHI_CLEAR_OP;
			fRPAtt[0].StoreOp			= Engine::RHIStoreOperation::IE_RHI_STORE_OP;
			fRPAtt[0].EndLayout			= Engine::RHILayoutType::IE_RHI_LAYOUT_PRESENT;
			fRPAtt[0].Samples			= Engine::RHISampleCount::IE_RHI_SAMPLE_COUNT_1_BIT;
			fRPAtt[0].AttachmentIndex	= 0;

			fRPAtt[1].Type				= Engine::RHIAttachmentType::IE_RHI_DEPTH_ATTACHMENT;
			fRPAtt[1].ColorFormat		= Engine::RHIColorFormat::IE_RHI_FORMAT_D32_SFLOAT;
			fRPAtt[1].LoadOp			= Engine::RHILoadOperation::IE_RHI_CLEAR_OP;
			fRPAtt[1].StoreOp			= Engine::RHIStoreOperation::IE_RHI_CLEAR_OP;
			fRPAtt[1].EndLayout			= Engine::RHILayoutType::IE_RHI_LAYOUT_PRESENT;
			fRPAtt[1].Samples			= Engine::RHISampleCount::IE_RHI_SAMPLE_COUNT_1_BIT;
			fRPAtt[1].AttachmentIndex	= 1;

			std::vector<Engine::RHISubAttachment> fRSOA(2);
			{
				fRSOA[0].AttachmentIndex = 0;
				fRSOA[1].AttachmentIndex = 1;
			}

			std::vector<Engine::RHISubpass> fSubpasses(1);
			{
				fSubpasses[0].OutputAttachments = fRSOA.data();
				fSubpasses[0].OutputAttachmentsCount = 2;
				fSubpasses[0].PreviousSubpass = ~0U;
				fSubpasses[0].NextSubpass = 0;
			}

			Engine::RHIRenderPass frenderpass{};
			{
				frenderpass.Attachments = fRPAtt.data();
				frenderpass.AttachmentsCount = 2;
				frenderpass.Subpasses = fSubpasses.data();
				frenderpass.SubpassesCount = 1;
			}

			m_EditorRenderPass = m_RenderingContextReference->InstantiateRenderPass(frenderpass);

			m_EditorFramebuffers = m_RenderingContextReference->CreateFramebuffers(m_EditorRenderPass, 1920, 1080, fRPAtt);

			Engine::RHISampler samplerInfo;
			{
				samplerInfo.MagFilter = Engine::RHISamplerFilter::IE_RHI_LINEAR;
				samplerInfo.MinFilter = Engine::RHISamplerFilter::IE_RHI_LINEAR;
				samplerInfo.ModeU = Engine::RHISamplerAdress::IE_RHI_REPEAT;
				samplerInfo.ModeV = Engine::RHISamplerAdress::IE_RHI_REPEAT;
				samplerInfo.ModeW = Engine::RHISamplerAdress::IE_RHI_REPEAT;
				samplerInfo.BorderColor = Engine::RHISamplerBorderColor::IE_RHI_BORDER_COLOR_BLACK_INT;
			}

			m_FramebufferSampler = m_RenderingContextReference->GetRenderInterface()->CreateSampler(m_RenderingContextReference->GetDevice(), m_EditorFramebuffers[0], samplerInfo);
		}

		void EditorRenderer::Terminate()
		{
			m_RenderingContextReference->GetRenderInterface()->DestroySampler(m_FramebufferSampler, m_RenderingContextReference->GetDevice());

			m_RenderingContextReference->DestroyFramebuffers(m_EditorFramebuffers);

			m_RenderingContextReference->DestroyRenderPass(m_EditorRenderPass);
		}
	}
}