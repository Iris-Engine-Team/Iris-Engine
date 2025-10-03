#pragma once

#include "Core/Tools/Macros.h"

#include <string>
#include <vector>

#include "Vector/Vector4.h"
#include "Vector/Vector2.h"

namespace Iris
{
	namespace Engine
	{
		class Shader;

		enum class RHIType : int
		{
			IE_RHI_TYPE_VULKAN	= 0,
			IE_RHI_TYPE_OPENGL	= 1,
			IE_RHI_TYPE_DX12	= 2
		};

		enum class IRIS_ENGINE_API RHIResult : int
		{
			IE_RHI_FAIL_UNKNOWN = -1,
			IE_RHI_DEBUG_TOOLS_DEACTIVATED = 5,
			IE_RHI_SUCCESS = 0
		};

		enum class IRIS_ENGINE_API RHIAttachmentType : int
		{
			IE_RHI_ATTACHMENT_NONE = 0,
			IE_RHI_COLOR_ATTACHMENT = 1,
			IE_RHI_DEPTH_ATTACHMENT = 2,
			IE_RHI_COLOR_RESOLVE_ATTACHMENT = 3,
			IE_RHI_INPUT_ATTACHMENT = 4
		};

		enum class IRIS_ENGINE_API RHILoadOperation : int
		{
			IE_RHI_LOAD_OP = 0,
			IE_RHI_CLEAR_OP = 1
		};

		enum class IRIS_ENGINE_API RHIStoreOperation : int
		{
			IE_RHI_STORE_OP = 0,
			IE_RHI_CLEAR_OP = 1
		};

		enum class IRIS_ENGINE_API RHIColorFormat : int
		{
			IE_RHI_FORMAT_UNDEFINED = 0,			// Undefined format

			IE_RHI_FORMAT_R8G8B8A8_UNORM = 1,		// RGBA 8-Bits uint normal
			IE_RHI_FORMAT_B8G8R8A8_UNORM = 2,		// BGRA 8-Bits uint normal
			IE_RHI_FORMAT_R8G8B8_UNORM = 3,			// RGB 8-Bits uint normal
			IE_RHI_FORMAT_B8G8R8_UNORM = 4,			// BGR 8-Bits uint normal

			IE_RHI_FORMAT_R8G8B8A8_SRGB = 5,		// RGBA 8-Bits srgb
			IE_RHI_FORMAT_B8G8R8A8_SRGB = 6,		// BGRA 8-Bits srgb
			IE_RHI_FORMAT_R8G8B8_SRGB = 7,			// RGB 8-Bits srgb
			IE_RHI_FORMAT_B8G8R8_SRGB = 8,			// BGR 8-Bits srgb

			IE_RHI_FORMAT_R16G16B16A16_SFLOAT = 9,	// RGBA 16-Bits standart float (HDRI)
			IE_RHI_FORMAT_R32G32B32A32_SFLOAT = 10,	// RGBA 32-Bits standart float (HDRI)

			IE_RHI_FORMAT_D32_SFLOAT = 12,			// 32-Bits depth format standart float
			IE_RHI_FORMAT_D32_S8_SFLOAT = 13,		// 32-Bits depth and 8-Bits stencil format standart float
			IE_RHI_FORMAT_D24_S8_UNORM = 14			// 24-Bits depth and 8-Bits uint normal
		};

		enum class IRIS_ENGINE_API RHISampleCount : int
		{
			IE_RHI_SAMPLE_COUNT_1_BIT	= 0,			// 
			IE_RHI_SAMPLE_COUNT_2_BIT	= 1,			// 
			IE_RHI_SAMPLE_COUNT_4_BIT	= 2,			// 
			IE_RHI_SAMPLE_COUNT_8_BIT	= 3,			// 
			IE_RHI_SAMPLE_COUNT_16_BIT	= 4,			// 
			IE_RHI_SAMPLE_COUNT_32_BIT	= 5,			// 
			IE_RHI_SAMPLE_COUNT_64_BIT	= 6				// 
		};

		enum class IRIS_ENGINE_API RHILayoutType : int
		{
			IE_RHI_LAYOUT_UNDEFINED							= 0,	// Undefined layout
			IE_RHI_LAYOUT_COLOR_ATTACHMENT					= 1,	// Color attachment layout
			IE_RHI_LAYOUT_DEPTH_STENCIL_ATTACHMENT			= 2,	// Depth attachment layout
			IE_RHI_LAYOUT_SHADER_READ_ONLY					= 3,	// Shader read only layout
			IE_RHI_LAYOUT_PRESENT							= 4		// Presentation layout
		};

		enum class IRIS_ENGINE_API RHITextureType : int
		{
			IE_RHI_TEXTURE_2D = 0,
			IE_RHI_TEXTURE_CUBEMAP = 1
		};

		enum class IRIS_ENGINE_API RHISamplerFilter : int
		{
			IE_RHI_NEAREST	= 0,
			IE_RHI_LINEAR	= 1,
			IE_RHI_CUBIC	= 2
		};

		enum class IRIS_ENGINE_API RHISamplerAdress : int
		{
			IE_RHI_REPEAT					= 0,
			IE_RHI_MIRRORED_REPEAT			= 1,
			IE_RHI_CLAMP_TO_EDGE			= 2,
			IE_RHI_CLAMP_TO_BORDER			= 3,
			IE_RHI_MIRRORED_CLAMP_TO_EDGE	= 4
		};

		enum class IRIS_ENGINE_API RHISamplerBorderColor : int
		{
			IE_RHI_BORDER_COLOR_BLACK_INT	= 0,
			IE_RHI_BORDER_COLOR_WHITE_INT	= 1,
			IE_RHI_BORDER_COLOR_BLACK_FLOAT = 2,
			IE_RHI_BORDER_COLOR_WHITE_FLOAT = 3
		};

		enum class IRIS_ENGINE_API RHIFrontFace : int
		{
			IE_RHI_FACE_CLOCKWISE = 0,
			IE_RHI_FACE_COUNTER_CLOCKWISE = 1
		};

		enum class IRIS_ENGINE_API RHIFaceCulling : int
		{
			IE_RHI_FACE_CULLING_FRONT_AND_BACK = 0,
			IE_RHI_FACE_CULLING_FRONT = 1,
			IE_RHI_FACE_CULLING_BACK = 2,
			IE_RHI_FACE_CULLING_NONE = 3
		};

		enum class IRIS_ENGINE_API RHIBufferType : int
		{
			IE_RHI_VERTEX_BUFFER = 0,
			IE_RHI_INDEX_BUFFER = 1,
			IE_RHI_UNIFORM_BUFFER = 2
		};

		enum class IRIS_ENGINE_API RHIShaderType : int
		{
			IE_RHI_SHADER_TYPE_VERTEX = 0,
			IE_RHI_SHADER_TYPE_TESSELLATION = 1,
			IE_RHI_SHADER_TYPE_FRAGMENT = 2,
			IE_RHI_SHADER_TYPE_GEOMETRY = 3
		};

		// MUST match the vulkan equivalent
		enum class IRIS_ENGINE_API  RHIDescriptorType : int
		{
			IE_RHI_DESCRIPTOR_TYPE_SAMPLER					= 0,
			IE_RHI_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER	= 1,
			IE_RHI_DESCRIPTOR_TYPE_UNIFORM_BUFFER			= 6,
			IE_RHI_DESCRIPTOR_TYPE_INPUT_ATTACHMENT			= 10
		};

		enum class IRIS_ENGINE_API RHIBlendType : int
		{
			IE_RHI_BLEND_TYPE_NONE = 0,
			IE_RHI_BLEND_TYPE_ALPHA_BLEND = 1,
			IE_RHI_BLEND_TYPE_ADDITIVE = 2
		};

		enum class IRIS_ENGINE_API RHIPolygonMode : int
		{
			IE_RHI_POLYGON_MODE_FILL = 0,
			IE_RHI_POLYGON_MODE_LINE = 1,
			IE_RHI_POLYGON_MODE_POINT = 2
		};

		enum class IRIS_ENGINE_API RHICompareOperation : int
		{
			IE_RHI_COMPARE_OP_NEVER = 0,
			IE_RHI_COMPARE_OP_LESS = 1,
			IE_RHI_COMPARE_OP_EQUAL = 2,
			IE_RHI_COMPARE_OP_LESS_OR_EQUAL = 3,
			IE_RHI_COMPARE_OP_GREATER = 4,
			IE_RHI_COMPARE_OP_NOT_EQUAL = 5,
			IE_RHI_COMPARE_OP_GREATER_OR_EQUAL = 6,
			IE_RHI_COMPARE_OP_ALWAYS = 7
		};

		struct RHIClearColor
		{
			Vector4 ColorClearValue = Vector4(1.f, 1.f, 1.f, 1.f);
			Vector2 ColorDepthValue = Vector2(0.f, 0.f);
		};

		struct RHIShaderInfo
		{
			Shader* Shader = nullptr;
			std::string EntryFunction = "main";
		};

		struct IRIS_ENGINE_API RHIAttachment
		{
			RHIColorFormat		ColorFormat		= RHIColorFormat::IE_RHI_FORMAT_R8G8B8A8_UNORM;
			RHIAttachmentType	Type			= RHIAttachmentType::IE_RHI_ATTACHMENT_NONE;
			RHILoadOperation	LoadOp			= RHILoadOperation::IE_RHI_CLEAR_OP;
			RHIStoreOperation	StoreOp			= RHIStoreOperation::IE_RHI_CLEAR_OP;
			RHISampleCount		Samples			= RHISampleCount::IE_RHI_SAMPLE_COUNT_1_BIT;
			RHILayoutType		EndLayout		= RHILayoutType::IE_RHI_LAYOUT_UNDEFINED;
			RHITextureType		FramebufferType	= RHITextureType::IE_RHI_TEXTURE_2D;
			unsigned int		AttachmentIndex	= 0;
		};

		struct IRIS_ENGINE_API RHISubAttachment
		{
			unsigned int AttachmentIndex	= 0;
			RHILayoutType EndLayout			= RHILayoutType::IE_RHI_LAYOUT_UNDEFINED;
		};

		struct IRIS_ENGINE_API RHISubpass
		{
			RHISubAttachment* InputAttachments = nullptr;
			unsigned int InputAttachmentsCount = 0;
			RHISubAttachment* OutputAttachments = nullptr;
			unsigned int OutputAttachmentsCount = 0;
			unsigned int PreviousSubpass = 0;
			unsigned int NextSubpass = 1;
		};

		struct IRIS_ENGINE_API RHIRenderPass
		{
			RHIAttachment* Attachments = nullptr;
			unsigned int AttachmentsCount = 0;
			RHISubpass* Subpasses = nullptr;
			unsigned int SubpassesCount = 0;
		};

		enum class RHIPipelinePass : uint32_t
		{
			IE_PIPELINE_LIT_FWD = 0,
			IE_PIPELINE_UNLIT_FWD = 1,
			IE_PIPELINE_LIT_DFD = 2,
			IE_PIPELINE_UNLIT_DFD = 3
		};

		struct RHIPipelineTemplate
		{
			RHIPipelinePass PassType = RHIPipelinePass::IE_PIPELINE_LIT_FWD;
			bool BackendTemplate = false;
			std::vector<RHIShaderInfo> Shaders;
		};

		struct IRIS_ENGINE_API RHIPipeline
		{
			RHIPipelineTemplate Template{};
			RHIFrontFace FrontFace = RHIFrontFace::IE_RHI_FACE_COUNTER_CLOCKWISE;
			RHIFaceCulling FaceCulling = RHIFaceCulling::IE_RHI_FACE_CULLING_FRONT;
			RHISampleCount SampleCount = RHISampleCount::IE_RHI_SAMPLE_COUNT_1_BIT;
			RHIBlendType BlendType = RHIBlendType::IE_RHI_BLEND_TYPE_NONE;
			RHIPolygonMode PolygonMode = RHIPolygonMode::IE_RHI_POLYGON_MODE_FILL;
			RHICompareOperation CompareOperation = RHICompareOperation::IE_RHI_COMPARE_OP_LESS;
			bool EnableDepthTest = true;
			bool EnableDepthWrite = true;
			unsigned int Subpass = 0;
			unsigned int AttachmentCount = 1;
			bool ContainsInput = true;
		};

		struct IRIS_ENGINE_API RHIImage
		{
			RHIAttachmentType Usage = RHIAttachmentType::IE_RHI_COLOR_ATTACHMENT;
			RHITextureType Type = RHITextureType::IE_RHI_TEXTURE_2D;
			RHIColorFormat ColorFormat = RHIColorFormat::IE_RHI_FORMAT_R8G8B8_UNORM;
			RHISampleCount SampleCount = RHISampleCount::IE_RHI_SAMPLE_COUNT_1_BIT;
			uint32_t MipCount = 1;
		};

		struct RHISampler
		{
			RHISamplerFilter MagFilter = RHISamplerFilter::IE_RHI_LINEAR;
			RHISamplerFilter MinFilter = RHISamplerFilter::IE_RHI_LINEAR;
			RHISamplerAdress ModeU = RHISamplerAdress::IE_RHI_REPEAT;
			RHISamplerAdress ModeV = RHISamplerAdress::IE_RHI_REPEAT;
			RHISamplerAdress ModeW = RHISamplerAdress::IE_RHI_REPEAT;
			RHISamplerBorderColor BorderColor = RHISamplerBorderColor::IE_RHI_BORDER_COLOR_BLACK_INT;
		};
	}
}