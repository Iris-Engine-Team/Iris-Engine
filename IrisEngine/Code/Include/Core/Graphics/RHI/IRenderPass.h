#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"
#include "Core/Graphics/RHI/CommonTypes.h"

#include <vector>

namespace Iris
{
	namespace Engine
	{
		class VulkanRenderPass;
		class IDevice;
		class ISwapChain;

		class IRenderPass
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			virtual ~IRenderPass() = default;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			virtual IE_FORCE_INLINE VulkanRenderPass* CastToVulkan() { return nullptr; }

			IRIS_ENGINE_API IE_FORCE_INLINE std::vector<RHIAttachment> GetAttachments() const { return p_Attachments; }

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IE_FORCE_INLINE unsigned int GetAttachmentCount() { return p_AttachmentsCount; }
			
			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IE_FORCE_INLINE unsigned int GetDepthAttachmentIndex() { return p_DepthAttachmentIndex; }

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_swapChain"></param>
			/// <param name="_renderPassDescription"></param>
			/// <returns></returns>
			virtual RHIResult Initialize(IDevice* _device, ISwapChain* _swapChain, const RHIRenderPass& _renderPassInfo) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <returns></returns>
			virtual RHIResult Terminate(IDevice* _device) = 0;

		protected:

			std::vector<RHIAttachment> p_Attachments{};

			unsigned int p_AttachmentsCount = 0;
			unsigned int p_DepthAttachmentIndex = 0;
		};
	}
}