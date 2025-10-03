#pragma once

#include "Resources/IResource.h"
#include "Core/Graphics/RHI/Vertex.h"
#include "Core/Graphics/RHI/IBuffer.h"

#include <vector>

namespace Iris
{
	namespace Engine
	{
		class Material;
		struct LoadingParameters;

		struct ChildMesh
		{
			IBuffer* VertexBuffer = nullptr;
			IBuffer* IndexBuffer = nullptr;
			size_t IndicesCount = 0;
			uint32_t MaterialIndex = 0;
			std::string MaterialID = "";
		};

		struct MeshData
		{
			std::vector<Vertex> Vertices{};
			std::vector<uint32_t> Indices{};
			std::string MaterialID = "";
			uint32_t MaterialIndex = 0;
		};

		struct MeshLoadedData : LoadedData
		{
			std::vector<MeshData> Data{};
		};

		class Mesh : public IResource
		{
		public:
			~Mesh() override = default;

			IE_FORCE_INLINE std::vector<ChildMesh>* GetMeshData() { return &m_ChildMeshes; }

			IRIS_ENGINE_API void LoadCompiledData(LoadedData* _data, ResourceContext* _resourceContext, const LoadingParameters& _loadingParameters) override final;
			IRIS_ENGINE_API void UnloadCompiledData(ResourceContext* _resourceContext) override final;

		private:
			std::vector<ChildMesh> m_ChildMeshes;
		};
	}
}