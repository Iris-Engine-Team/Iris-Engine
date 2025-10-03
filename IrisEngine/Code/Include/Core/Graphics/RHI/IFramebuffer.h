#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"
#include "Core/Graphics/RHI/CommonTypes.h"

#include <vector>

namespace Iris
{
	namespace Engine
	{
		class IDevice;
		class IImage;
		class VulkanFramebuffer;
		class IRenderPass;
		struct KR_Attachment;

		class IFramebuffer
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			IRIS_ENGINE_API virtual ~IFramebuffer() = default;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API virtual IE_FORCE_INLINE VulkanFramebuffer* CastToVulkan() { return nullptr; }

			IRIS_ENGINE_API IE_FORCE_INLINE unsigned int GetWidth() { return p_Width; }
			IRIS_ENGINE_API IE_FORCE_INLINE unsigned int GetHeight() { return p_Height; }

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_renderPass"></param>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			/// <param name="_attachments"></param>
			/// <returns></returns>
			IRIS_ENGINE_API virtual RHIResult CreateFramebuffer(IDevice* _device, IRenderPass* _renderPass, unsigned int _width, unsigned int _height, const std::vector<RHIAttachment>& _attachments) = 0;

			IRIS_ENGINE_API virtual RHIResult CreateFramebuffer(IDevice* _device, IRenderPass* _renderPass, unsigned int _width, unsigned int _height, const std::vector<IImage*>& _attachments) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <returns></returns>
			IRIS_ENGINE_API virtual RHIResult DestroyFramebuffer(IDevice* _device) = 0;
		
		protected:
			unsigned int p_Width = 1920;
			unsigned int p_Height = 1080;
		
		};
	}
}