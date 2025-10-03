#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"
#include "Core/Graphics/RHI/CommonTypes.h"

#include <vector>

namespace Iris
{
	namespace Engine
	{
		class VulkanPipeline;
		class IDevice;
		class ISwapChain;
		class IRenderPass;
		class IDescriptorLayout;
		class IShader;

		class IPipeline
		{
		public:
			/// <summary>
			/// Default IPipeline's destructor
			/// </summary>
			virtual ~IPipeline() = default;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			virtual IRIS_ENGINE_API IE_FORCE_INLINE VulkanPipeline* CastToVulkan() { return nullptr; }

			/// <summary>
			/// 
			/// </summary>
			IRIS_ENGINE_API IE_FORCE_INLINE IDescriptorLayout* GetDescriptorLayout(unsigned int _index) { return p_DescriptorLayouts[_index]; }

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_renderPass"></param>
			/// <param name="_pipelineInfo"></param>
			/// <returns></returns>
			virtual IRIS_ENGINE_API IE_FORCE_INLINE RHIResult Initialize(IDevice* _device, IRenderPass* _renderPass, const RHIPipeline& _pipelineInfo)
			{
				_device; _renderPass;

				m_FaceCulling = _pipelineInfo.FaceCulling;
				m_BlendMode = _pipelineInfo.BlendType;

				return RHIResult::IE_RHI_SUCCESS;
			}

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_Device"></param>
			/// <returns></returns>
			virtual IRIS_ENGINE_API RHIResult Terminate(IDevice* _Device) = 0;

		protected:

			std::vector<IDescriptorLayout*> p_DescriptorLayouts;

		private:

			RHIFaceCulling m_FaceCulling = RHIFaceCulling::IE_RHI_FACE_CULLING_FRONT;
			RHIBlendType m_BlendMode = RHIBlendType::IE_RHI_BLEND_TYPE_NONE;
		};
	}
}