#include "Loader/MeshLoader/IMeshLoader.h"
#include "Resources/Mesh.h"

namespace Iris
{
	namespace Engine
	{
		void IMeshLoader::ProcessTangentBitangent(MeshData& _meshData)
		{
			size_t indicesCount = _meshData.Indices.size();

			for (size_t i = 0; i < indicesCount; i += 3)
			{
				uint32_t i0 = _meshData.Indices[i];
				uint32_t i1 = _meshData.Indices[i + 1];
				uint32_t i2 = _meshData.Indices[i + 2];

				Vector3 edge1 = _meshData.Vertices[i1].Position - _meshData.Vertices[i0].Position;
				Vector3 edge2 = _meshData.Vertices[i2].Position - _meshData.Vertices[i0].Position;

				Vector2 deltaUV1 = _meshData.Vertices[i1].TextCoord - _meshData.Vertices[i0].TextCoord;
				Vector2 deltaUV2 = _meshData.Vertices[i2].TextCoord - _meshData.Vertices[i0].TextCoord;

				float f = 1.f / (deltaUV1[0] * deltaUV2[1] - deltaUV2[0] * deltaUV1[1]);

				Vector3 tangent = Vector3::s_Zero;
				tangent.SetX(f * (deltaUV2[1] * edge1[0] - deltaUV1[1] * edge2[0]));
				tangent.SetY(f * (deltaUV2[1] * edge1[1] - deltaUV1[1] * edge2[1]));
				tangent.SetZ(f * (deltaUV2[1] * edge1[2] - deltaUV1[1] * edge2[2]));
				tangent = tangent.Normalize();

				Vector3 bitangent = Vector3::s_Zero;
				bitangent.SetX(f * (-deltaUV2[1] * edge1[0] + deltaUV1[1] * edge2[0]));
				bitangent.SetY(f * (-deltaUV2[1] * edge1[1] + deltaUV1[1] * edge2[1]));
				bitangent.SetZ(f * (-deltaUV2[1] * edge1[2] + deltaUV1[1] * edge2[2]));
				bitangent = bitangent.Normalize();

				_meshData.Vertices[i0].Tangent = tangent;
				_meshData.Vertices[i1].Tangent = tangent;
				_meshData.Vertices[i2].Tangent = tangent;

				_meshData.Vertices[i0].BiTangent = bitangent;
				_meshData.Vertices[i1].BiTangent = bitangent;
				_meshData.Vertices[i2].BiTangent = bitangent;
			}
		}
	}
}