#pragma once
namespace Iris
{
	namespace Engine
	{
		class IBuffer;
		class Material;
		class IPipeline;

		struct DrawCommand
		{
			IPipeline* Pipeline = nullptr;
			IBuffer* VertexBuffer = nullptr;
			IBuffer* IndexBuffer = nullptr;
			size_t IndicesCount = 0;
			uint32_t ModelRendererIndex = 0;
			const Material* Material = nullptr;
		};
	}
}