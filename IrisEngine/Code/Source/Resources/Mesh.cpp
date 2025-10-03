#include "Resources/Mesh.h"
#include "Resources/ResourceManager.h"

namespace Iris
{
	namespace Engine
	{
		void Mesh::LoadCompiledData(LoadedData* _loadedData, ResourceContext* _resourceContext, const LoadingParameters& _loadingParameters)
		{
			_loadingParameters;
			MeshLoadedData* loadedData = dynamic_cast<MeshLoadedData*>(_loadedData);
			size_t size = loadedData->Data.size();

			ResourceManager* mgr = ResourceManager::GetInstance();
			mgr;

			for (size_t i = 0; i < size; ++i)
			{
				ChildMesh newMesh{};
				newMesh.VertexBuffer = _resourceContext->CreateBuffer(RHIBufferType::IE_RHI_VERTEX_BUFFER, sizeof(loadedData->Data[i].Vertices[0]) * loadedData->Data[i].Vertices.size(), loadedData->Data[i].Vertices.data());
				newMesh.IndexBuffer = _resourceContext->CreateBuffer(RHIBufferType::IE_RHI_INDEX_BUFFER, sizeof(loadedData->Data[i].Indices[0]) * loadedData->Data[i].Indices.size(), loadedData->Data[i].Indices.data());
				newMesh.IndicesCount = loadedData->Data[i].Indices.size();
				newMesh.MaterialID = loadedData->Data[i].MaterialID;
				newMesh.MaterialIndex = loadedData->Data[i].MaterialIndex;

				m_ChildMeshes.push_back(newMesh);
			}
		}

		void Mesh::UnloadCompiledData(ResourceContext* _resourceContext)
		{
			size_t meshDataCount = m_ChildMeshes.size();

			for (size_t i = 0; i < meshDataCount; ++i)
			{
				_resourceContext->DestroyBuffer(m_ChildMeshes[i].VertexBuffer);
				_resourceContext->DestroyBuffer(m_ChildMeshes[i].IndexBuffer);
			}
		}
	}
}