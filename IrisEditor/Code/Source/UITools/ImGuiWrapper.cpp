#include "UITools/ImGuiWrapper.h"

#include "Core/Graphics/Renderer/EditorRenderer.h"

#include "Resources/Texture.h"
#include "Core/Graphics/VulkanRHI/VulkanSampler.h"
#include "Core/Graphics/VulkanRHI/VulkanImage.h"
#include "Core/Graphics/VulkanRHI/VulkanSwapChain.h"
#include "Core/Graphics/VulkanRHI/VulkanFramebuffer.h"

#include "Core/Graphics/RHI/CommonTypes.h"

#include <imgui_impl_vulkan.h>

namespace Iris
{
	namespace Editor
	{
		Engine::RHIType ImGuiWrapper::m_RHIType = Engine::RHIType::IE_RHI_TYPE_VULKAN;

		ImTextureID ImGuiWrapper::CreateTextureID(const Engine::Texture* _texture)
		{
			if (!_texture)
				return 0;
			Engine::ISampler* sp = _texture->GetSampler();
			Engine::IImage* im = _texture->GetImage();
			ImTextureID textureID = 0;
			switch (m_RHIType)
			{
			case Engine::RHIType::IE_RHI_TYPE_VULKAN: default:
				textureID = (ImTextureID)(ImGui_ImplVulkan_AddTexture(sp->CastToVulkan()->GetSampler(), im->CastToVulkan()->GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
				break;
			case Engine::RHIType::IE_RHI_TYPE_OPENGL:
				break;
			case Engine::RHIType::IE_RHI_TYPE_DX12:
				break;
			}

			return textureID;
		}

		ImTextureID ImGuiWrapper::CreateTextureID(Engine::Texture* _texture)
		{
			if (!_texture)
				return 0;
			Engine::ISampler* sp = _texture->GetSampler();
			Engine::IImage* im = _texture->GetImage();
			ImTextureID textureID = 0;
			switch (m_RHIType)
			{
			case Engine::RHIType::IE_RHI_TYPE_VULKAN: default:
				textureID = (ImTextureID)(ImGui_ImplVulkan_AddTexture(sp->CastToVulkan()->GetSampler(), im->CastToVulkan()->GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
				break;
			case Engine::RHIType::IE_RHI_TYPE_OPENGL:
				break;
			case Engine::RHIType::IE_RHI_TYPE_DX12:
				break;
			}

			return textureID;
		}

		std::vector<ImTextureID> ImGuiWrapper::CreateFramebufferIDs(EditorRenderer* _editorRenderer, std::vector<Engine::IFramebuffer*> _framebuffers)
		{
			std::vector<ImTextureID> IDs(MAX_FRAMES_IN_FLIGHT);

			Engine::ISampler* fbSampler = _editorRenderer->GetFramebufferSampler();

			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
			{
				switch (m_RHIType)
				{
				case Engine::RHIType::IE_RHI_TYPE_VULKAN: default:
					IDs[i] = (ImTextureID)(ImGui_ImplVulkan_AddTexture(fbSampler->CastToVulkan()->GetSampler(), _framebuffers[i]->CastToVulkan()->GetAttachments()[0].GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
					break;
				}
			}

			return IDs;
		}

		void ImGuiWrapper::DeleteFramebufferIDs(std::vector<ImTextureID>& _framebufferIDs)
		{
			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
			{
				switch (m_RHIType)
				{
				case Engine::RHIType::IE_RHI_TYPE_VULKAN: default:
					ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet)_framebufferIDs[i]);
					break;
				}
			}
		}

		void ImGuiWrapper::SetRHIType(Engine::RHIType _RHIType)
		{
			m_RHIType = _RHIType;
		}
	}
}