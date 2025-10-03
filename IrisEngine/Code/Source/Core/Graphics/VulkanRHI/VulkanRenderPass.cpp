#include "Core/Graphics/VulkanRHI/VulkanRenderPass.h"

#include "Core/Graphics/VulkanRHI/VulkanDevice.h"
#include "Core/Graphics/VulkanRHI/VulkanSwapChain.h"
#include "Core/Graphics/VulkanRHI/VulkanRenderInterface.h"

#include <array>

namespace Iris
{
	namespace Engine
	{
		RHIResult VulkanRenderPass::Initialize(IDevice* _device, ISwapChain* _swapChain, const RHIRenderPass& _renderPassInfo)
		{
			// Creates a std::vector from a pointer
			if (_renderPassInfo.Attachments && _renderPassInfo.AttachmentsCount > 0)
			{
				p_Attachments = std::vector<RHIAttachment> (_renderPassInfo.Attachments, _renderPassInfo.Attachments + _renderPassInfo.AttachmentsCount);
			}

			VulkanDevice* vkDevice = _device->CastToVulkan();
			VulkanSwapChain* vkSwapChain = _swapChain->CastToVulkan();
			vkSwapChain;

			std::vector<VkAttachmentDescription> attachmentsDescriptions{};

			// Creates all attachments
			for (size_t i = 0; i < _renderPassInfo.AttachmentsCount; ++i)
			{
				const RHIAttachment& attachment = _renderPassInfo.Attachments[i];

				VkAttachmentDescription attachmentDescription{};
				attachmentDescription.format = VulkanRenderInterface::ConvertColorFormat(attachment.ColorFormat);
				attachmentDescription.samples = VulkanRenderInterface::ConvertSampleCount(attachment.Samples);

				attachmentDescription.loadOp = VulkanRenderInterface::ConvertLoadOperation(attachment.LoadOp);
				attachmentDescription.storeOp = VulkanRenderInterface::ConvertStoreOperation(attachment.StoreOp);;

				attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

				attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				attachmentDescription.finalLayout = VulkanRenderInterface::ConvertLayout(attachment.EndLayout);

				attachmentsDescriptions.push_back(attachmentDescription);
				++p_AttachmentsCount;

				if (attachment.Type == RHIAttachmentType::IE_RHI_DEPTH_ATTACHMENT)
					p_DepthAttachmentIndex = p_DepthAttachmentIndex;
			}

			std::vector<VkSubpassDescription> subpasses{};
			std::vector<VkSubpassDependency> subpassDependencies{};

			std::vector<std::vector<VkAttachmentReference>> outputAttachments{};
			std::vector<std::vector<VkAttachmentReference>> inputAttachments{};
			std::vector<std::vector<VkAttachmentReference>> resolveAttachments{};

			std::vector<std::vector<VkAttachmentReference>> depthAttachment{};

			int subpassIndex = 0;

			// Creates all subpasses and reference each attachments as outputs or inputs
			for (size_t i = 0; i < _renderPassInfo.SubpassesCount; ++i)
			{
				const RHISubpass& rhisubpass = _renderPassInfo.Subpasses[i];

				std::vector<VkAttachmentReference> currentOuputs{};
				std::vector<VkAttachmentReference> currentInputs{};
				std::vector<VkAttachmentReference> currentResolves{};
				std::vector<VkAttachmentReference> currentDepth{};

				for (size_t j = 0; j < rhisubpass.OutputAttachmentsCount; ++j)
				{
					const RHISubAttachment& subAttachment = rhisubpass.OutputAttachments[j];

					VkAttachmentReference outputAttachment{};
					switch (_renderPassInfo.Attachments[subAttachment.AttachmentIndex].Type)
					{
					case RHIAttachmentType::IE_RHI_COLOR_ATTACHMENT: default:
						outputAttachment.attachment = subAttachment.AttachmentIndex;
						outputAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
						// Can be converted but is supposed to only have this layout as an output
						currentOuputs.push_back(outputAttachment);
						break;
					case RHIAttachmentType::IE_RHI_DEPTH_ATTACHMENT:
						outputAttachment.attachment = subAttachment.AttachmentIndex;
						outputAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
						currentDepth.push_back(outputAttachment);
						break;
					case RHIAttachmentType::IE_RHI_COLOR_RESOLVE_ATTACHMENT:
						outputAttachment.attachment = subAttachment.AttachmentIndex;
						outputAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
						currentResolves.push_back(outputAttachment);
						break;
					}
				}

				for (size_t j = 0; j < rhisubpass.InputAttachmentsCount; ++j)
				{
					const RHISubAttachment& subAttachment = rhisubpass.InputAttachments[j];

					VkAttachmentReference inputAttachment{};
					inputAttachment.attachment = subAttachment.AttachmentIndex;
					inputAttachment.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					// Can be converted but is supposed to only have this layout as an input

					currentInputs.push_back(inputAttachment);
				}

				outputAttachments.push_back(currentOuputs);
				inputAttachments.push_back(currentInputs);
				resolveAttachments.push_back(currentResolves);
				depthAttachment.push_back(currentDepth);

				VkSubpassDescription subpass{};
				subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
				subpass.colorAttachmentCount = static_cast<uint32_t>(outputAttachments[subpassIndex].size());
				subpass.pColorAttachments = outputAttachments[subpassIndex].data();
				subpass.inputAttachmentCount = static_cast<uint32_t>(inputAttachments[subpassIndex].size());
				subpass.pInputAttachments = inputAttachments[subpassIndex].data();
				subpass.pDepthStencilAttachment = depthAttachment[subpassIndex].data();
				subpass.pResolveAttachments = resolveAttachments[subpassIndex].data();

				VkSubpassDependency dependency{};
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependency.srcSubpass = rhisubpass.PreviousSubpass;
				dependency.dstSubpass = rhisubpass.NextSubpass;

				dependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

				dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
				dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

				subpasses.push_back(subpass);
				subpassDependencies.push_back(dependency);

				++subpassIndex;
			}

			VkRenderPassCreateInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentsDescriptions.size());
			renderPassInfo.pAttachments = attachmentsDescriptions.data();
			renderPassInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
			renderPassInfo.pSubpasses = subpasses.data();
			renderPassInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
			renderPassInfo.pDependencies = subpassDependencies.data();

			VkResult result = vkCreateRenderPass(vkDevice->GetLogicalDevice(), &renderPassInfo, nullptr, &m_RenderPass);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: failed to create render pass. Error code: %i", result);
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanRenderPass::Terminate(IDevice* _device)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			vkDestroyRenderPass(vkDevice->GetLogicalDevice(), m_RenderPass, nullptr);

			return RHIResult::IE_RHI_SUCCESS;
		}
	}
}