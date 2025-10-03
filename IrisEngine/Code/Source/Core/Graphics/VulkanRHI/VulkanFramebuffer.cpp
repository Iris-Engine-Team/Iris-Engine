#include "Core/Graphics/VulkanRHI/VulkanFramebuffer.h"

#include "Core/Graphics/VulkanRHI/VulkanDevice.h"
#include "Core/Graphics/VulkanRHI/VulkanRenderPass.h"
#include "Core/Graphics/VulkanRHI/VulkanImage.h"
#include "Core/Graphics/VulkanRHI/VulkanRenderInterface.h"
#include "Core/Graphics/VulkanRHI/VulkanImage.h"

namespace Iris
{
	namespace Engine
	{
		RHIResult VulkanFramebuffer::CreateFramebuffer(IDevice* _device, IRenderPass* _renderPass, unsigned int _width, unsigned int _height, const std::vector<RHIAttachment>& _attachments)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();
			VulkanRenderPass* vkRenderPass = _renderPass->CastToVulkan();

			std::vector<VkImageView> attachmentsView;

			VkFormat depthF = VulkanImage::FindDepthFormat(vkDevice);
			RHIColorFormat f = RHIColorFormat::IE_RHI_FORMAT_D32_SFLOAT;

			p_Width = _width;
			p_Height = _height;

			// TODO: REMOVE THIS
			switch (depthF)
			{
			case VK_FORMAT_D32_SFLOAT:
				f = RHIColorFormat::IE_RHI_FORMAT_D32_SFLOAT;
				break;
			case VK_FORMAT_D32_SFLOAT_S8_UINT:
				f = RHIColorFormat::IE_RHI_FORMAT_D32_S8_SFLOAT;
				break;
			case VK_FORMAT_D24_UNORM_S8_UINT:
				f = RHIColorFormat::IE_RHI_FORMAT_D24_S8_UNORM;
				break;
			}

			for (const RHIAttachment& attachment : _attachments)
			{
				VulkanImage imageAttachment;

				switch (attachment.Type)
				{
				default: case RHIAttachmentType::IE_RHI_ATTACHMENT_NONE:
					break;
				case RHIAttachmentType::IE_RHI_COLOR_ATTACHMENT: case RHIAttachmentType::IE_RHI_COLOR_RESOLVE_ATTACHMENT: case RHIAttachmentType::IE_RHI_INPUT_ATTACHMENT:
					imageAttachment.CreateImage(vkDevice, _width, _height, 1, attachment.ColorFormat, attachment.FramebufferType, VulkanRenderInterface::ConvertSampleCount(attachment.Samples), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VulkanRenderInterface::ConvertUsageFlag(attachment.Type) | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
					break;
				case RHIAttachmentType::IE_RHI_DEPTH_ATTACHMENT:
					imageAttachment.CreateImage(vkDevice, _width, _height, 1, f, attachment.FramebufferType, VulkanRenderInterface::ConvertSampleCount(attachment.Samples), VulkanRenderInterface::ConvertUsageFlag(attachment.Type) | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
					break;
				}

				m_Attachments.emplace_back(imageAttachment);
				attachmentsView.emplace_back(imageAttachment.GetImageView());
			}

			switch (_attachments[0].FramebufferType)
			{
			case RHITextureType::IE_RHI_TEXTURE_2D: default:
				CreateFramebuffer(vkDevice, vkRenderPass, _width, _height, 1, attachmentsView);
				break;
			case RHITextureType::IE_RHI_TEXTURE_CUBEMAP:
				CreateFramebuffer(vkDevice, vkRenderPass, _width, _height, 6, attachmentsView);
				break;
			}

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanFramebuffer::CreateFramebuffer(IDevice* _device, IRenderPass* _renderPass, unsigned int _width, unsigned int _height, const std::vector<IImage*>& _attachments)
		{
			std::vector<VkImageView> attachmentsView{};

			size_t attachmentCount = _attachments.size();
			for (size_t i = 0; i < attachmentCount; ++i)
			{
				attachmentsView.push_back(_attachments[i]->CastToVulkan()->GetImageView());
			}

			unsigned int layerCount = 1;

			switch (_attachments[0]->GetTextureType())
			{
			default: case RHITextureType::IE_RHI_TEXTURE_2D:
				layerCount = 1;
				break;
			case RHITextureType::IE_RHI_TEXTURE_CUBEMAP:
				layerCount = 6;
				break;
			}

			return CreateFramebuffer(_device->CastToVulkan(), _renderPass->CastToVulkan(), _width, _height, layerCount, attachmentsView);
		}

		RHIResult VulkanFramebuffer::CreateFramebuffer(VulkanDevice* _device, VulkanRenderPass* _renderPass, unsigned int _width, unsigned int _height, unsigned int _layer, std::vector<VkImageView>& _attachments)
		{
			p_Width = _width;
			p_Height = _height;

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = _renderPass->GetRenderPass();
			framebufferInfo.attachmentCount = static_cast<uint32_t>(_attachments.size());
			framebufferInfo.pAttachments = _attachments.data();
			framebufferInfo.width = static_cast<uint32_t>(_width);
			framebufferInfo.height = static_cast<uint32_t>(_height);
			framebufferInfo.layers = static_cast<uint32_t>(_layer);

			VkResult result = vkCreateFramebuffer(_device->GetLogicalDevice(), &framebufferInfo, nullptr, &m_Framebuffer);

			if (result != VK_SUCCESS)
			{
				DEBUG_ERROR("Vulkan: Failed to create frame buffer. Error code: %i", result);
				return RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			return RHIResult::IE_RHI_SUCCESS;
		}

		RHIResult VulkanFramebuffer::DestroyFramebuffer(IDevice* _device)
		{
			VulkanDevice* vkDevice = _device->CastToVulkan();

			for (VulkanImage& attachment : m_Attachments)
			{
				attachment.DestroyImage(vkDevice);
			}

			m_Attachments.clear();

			vkDestroyFramebuffer(vkDevice->GetLogicalDevice(), m_Framebuffer, nullptr);

			return RHIResult::IE_RHI_SUCCESS;
		}
	}
}