#pragma once

#include "Core/Graphics/RHI/IFramebuffer.h"
#include "Core/Graphics/VulkanRHI/VulkanImage.h"

#include <vulkan/vulkan.h>

namespace Iris
{
	namespace Engine
	{
		class VulkanDevice;
		class VulkanImage;
		class VulkanRenderPass;

		class VulkanFramebuffer : public IFramebuffer
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			IRIS_ENGINE_API ~VulkanFramebuffer() override final = default;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VulkanFramebuffer* CastToVulkan() { return this; }

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VkFramebuffer GetFramebuffer() { return m_Framebuffer; }

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API IE_FORCE_INLINE std::vector<VulkanImage> GetAttachments() { return m_Attachments; }

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_renderPass"></param>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			/// <param name="_attachments"></param>
			/// <returns></returns>
			IRIS_ENGINE_API RHIResult CreateFramebuffer(IDevice* _device, IRenderPass* _renderPass, unsigned int _width, unsigned int _height, const std::vector<RHIAttachment>& _attachments) override final;

			IRIS_ENGINE_API RHIResult CreateFramebuffer(IDevice* _device, IRenderPass* _renderPass, unsigned int _width, unsigned int _height, const std::vector<IImage*>& _attachments) override final;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_renderPass"></param>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			/// <param name="_attachments"></param>
			/// <returns></returns>
			IRIS_ENGINE_API RHIResult CreateFramebuffer(VulkanDevice* _device, VulkanRenderPass* _renderPass, unsigned int _width, unsigned int _height, unsigned int _layer, std::vector<VkImageView>& _attachments);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <returns></returns>
			IRIS_ENGINE_API RHIResult DestroyFramebuffer(IDevice* _device) override final;

		private:
			VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;

			std::vector<VulkanImage> m_Attachments;
		};
	}
}