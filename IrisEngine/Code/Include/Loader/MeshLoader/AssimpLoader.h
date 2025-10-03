#pragma once

#include "Loader/MeshLoader/IMeshLoader.h"

#include <assimp/Importer.hpp>		// C++ importer interface
#include <assimp/scene.h>			// Output data structure
#include <assimp/postprocess.h>		// Post processing flags

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_GlobalScale | aiProcess_PreTransformVertices)

namespace Iris
{
	namespace Engine
	{
		struct MeshData;

		class AssimpLoader : public IMeshLoader
		{
		public:

			~AssimpLoader() override = default;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_path"></param>
			IRIS_ENGINE_API bool Load(LoadedData* _data, std::filesystem::path _path, const LoadingParameters& _parameters) override final;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_path"></param>
			IRIS_ENGINE_API void Unload(LoadedData* _data) override final;

		private:
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_Node"></param>
			/// <param name="_Scene"></param>
			void ProcessNode(const std::filesystem::path& _extension, std::vector<MeshData>& _meshData, aiNode* _node, const aiScene* _scene);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_Mesh"></param>
			/// <param name="_Scene"></param>
			void ProcessMesh(const std::filesystem::path& _extension, std::vector<MeshData>& _meshData, aiMesh* _mesh, const aiScene* _scene);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_Mesh"></param>
			/// <param name="_Scene"></param>
			void ProcessMeshZUP(std::vector<MeshData>& _meshData, aiMesh* _mesh, const aiScene* _scene);

			std::string ProcessMaterial(const aiMesh* _mesh, const aiScene* _scene);

			std::string ProcessGLTFMaterial(const aiMesh* _mesh, const aiScene* _scene);
		};
	}
}