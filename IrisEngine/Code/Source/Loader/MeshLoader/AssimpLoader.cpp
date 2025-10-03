#include "Loader/MeshLoader/AssimpLoader.h"
#include "Core/Graphics/RHI/Vertex.h"
#include "Resources/Mesh.h"
#include "Core/Graphics/Context/ResourceContext.h"
#include "Core/Graphics/RHI/IRenderInterface.h"
#include "Resources/ResourceManager.h"
#include "Resources/Material.h"

#include "Core/Tools/FilePath.h"

#include <unordered_map>

namespace Iris
{
	namespace Engine
	{
		bool AssimpLoader::Load(LoadedData* _data, std::filesystem::path _path, const LoadingParameters& _parameters)
		{
			_parameters;

			Assimp::Importer importer;

			importer.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 1.f);

			const aiScene* meshData = importer.ReadFile(_path.string().c_str(), ASSIMP_LOAD_FLAGS);

			meshData->mFlags;

			if (!meshData)
			{
				std::cout << importer.GetErrorString() << std::endl;
				DEBUG_ERROR("Failed to load model. Unknown error");
				return false;
			}
			else if (meshData->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !meshData->mRootNode)
			{
				DEBUG_ERROR("Failed to load model. Error code: %i", meshData->mFlags);
				return false;
			}

			std::vector<MeshData> data;
			ProcessNode(_path.extension(), data, meshData->mRootNode, meshData);

			dynamic_cast<MeshLoadedData*>(_data)->Data = data;

			return true;
		}

		void AssimpLoader::Unload(LoadedData* _data)
		{
			_data;
		}

		void AssimpLoader::ProcessNode(const std::filesystem::path& _extension, std::vector<MeshData>& _meshData, aiNode* _node, const aiScene* _scene)
		{
			for (size_t i = 0; i < _node->mNumMeshes; ++i)
			{
				aiMatrix4x4 mat = _scene->mRootNode->mTransformation;

				aiVector3D up(mat.b1, mat.b2, mat.b3);

				if (up == aiVector3D(0.f, 1.f, 0.f))
				{
					aiMesh* mesh = _scene->mMeshes[_node->mMeshes[i]];
					ProcessMesh(_extension, _meshData, mesh, _scene);
				}
				else
				{
					aiMesh* mesh = _scene->mMeshes[_node->mMeshes[i]];
					ProcessMesh(_extension, _meshData, mesh, _scene);
				}
			}

			for (size_t i = 0; i < _node->mNumChildren; ++i)
			{
				ProcessNode(_extension, _meshData, _node->mChildren[i], _scene);
			}
		}

		void AssimpLoader::ProcessMesh(const std::filesystem::path& _extension, std::vector<MeshData>& _meshData, aiMesh* _mesh, const aiScene* _scene)
		{
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;

			// Vertex duplication still present

			for (size_t i = 0; i < _mesh->mNumVertices; ++i)
			{
				const aiVector3D& position  = _mesh->mVertices[i];
				const aiVector3D& normal	= _mesh->HasNormals()					? _mesh->mNormals[i]			: aiVector3D(1.f);
				const aiVector3D& textCoord = _mesh->HasTextureCoords(0)			? _mesh->mTextureCoords[0][i]	: aiVector3D(1.f);
				const aiVector3D& tangent	= _mesh->HasTangentsAndBitangents()		? _mesh->mTangents[i]			: aiVector3D(1.f);
				const aiVector3D& biTangent = _mesh->HasTangentsAndBitangents()		? _mesh->mBitangents[i]			: aiVector3D(1.f);

				Vertex vertex;
				vertex.Position		=	{ position.x, position.y, position.z};
				vertex.Normal		=	{ normal.x, normal.y , normal.z };
				vertex.TextCoord	=	{ textCoord.x, textCoord.y };
				vertex.Tangent		=	{ tangent.x, tangent.y, tangent.z };
				vertex.BiTangent	=	{ biTangent.x, biTangent.y, biTangent.z };

				vertices.push_back(vertex);
			}

			for (size_t i = 0; i < _mesh->mNumFaces; ++i)
			{
				aiFace face = _mesh->mFaces[i];

				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}

			MeshData data{};
			data.Vertices = vertices;
			data.Indices = indices;

			if (!_mesh->HasTangentsAndBitangents())
			{
				IMeshLoader::ProcessTangentBitangent(data);
			}

			if (_mesh->mMaterialIndex >= 0)
			{
				if(_extension.string() == ".gltf" || _extension.string() == ".glb")
				{
					data.MaterialID = ProcessGLTFMaterial(_mesh, _scene);
					data.MaterialIndex = _scene->mNumMaterials == 1 ? 0 : _mesh->mMaterialIndex;
				}
				else
				{
					data.MaterialID = ProcessMaterial(_mesh, _scene);
					data.MaterialIndex = _scene->mNumMaterials == 1 ? 0 : _mesh->mMaterialIndex;
				}
			}

			_meshData.push_back(data);
		}

		void AssimpLoader::ProcessMeshZUP(std::vector<MeshData>& _meshData, aiMesh* _mesh, const aiScene* _scene)
		{
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;

			// Vertex duplication still present

			for (size_t i = 0; i < _mesh->mNumVertices; ++i)
			{
				const aiVector3D& position = _mesh->mVertices[i];
				const aiVector3D& normal = _mesh->HasNormals() ? _mesh->mNormals[i] : aiVector3D(1.f);
				const aiVector3D& textCoord = _mesh->HasTextureCoords(0) ? _mesh->mTextureCoords[0][i] : aiVector3D(1.f);
				const aiVector3D& tangent = _mesh->HasTangentsAndBitangents() ? _mesh->mTangents[i] : aiVector3D(1.f);
				const aiVector3D& biTangent = _mesh->HasTangentsAndBitangents() ? _mesh->mBitangents[i] : aiVector3D(1.f);

				Vertex vertex;
				vertex.Position = { position.x, -position.z, position.y };
				vertex.Normal = { normal.x, -normal.z , normal.y };
				vertex.TextCoord = { textCoord.x, textCoord.y };
				vertex.Tangent = { tangent.x, -tangent.z, tangent.y };
				vertex.BiTangent = { biTangent.x, -biTangent.z, biTangent.y };

				vertices.push_back(vertex);
			}

			for (size_t i = 0; i < _mesh->mNumFaces; ++i)
			{
				aiFace face = _mesh->mFaces[i];

				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}

			MeshData data{};
			data.Vertices = vertices;
			data.Indices = indices;

			if (!_mesh->HasTangentsAndBitangents())
			{
				IMeshLoader::ProcessTangentBitangent(data);
			}

			if (_mesh->mMaterialIndex >= 0)
			{
				_scene;
			}

			_meshData.push_back(data);
		}

		std::string AssimpLoader::ProcessMaterial(const aiMesh* _mesh, const aiScene* _scene)
		{
			aiMaterial* material = _scene->mMaterials[_mesh->mMaterialIndex];

			aiString path;

			ResourceManager* mgr = ResourceManager::GetInstance();

			std::string defaultName = "DefaultTexture.jpg";

			Texture* defaulTtexture = mgr->Get<Texture>(defaultName);

			material->GetTexture(aiTextureType_BASE_COLOR, 0, &path);
			std::filesystem::path mpath = path.C_Str();
			mpath = mpath.filename();

			if (mpath.empty() && mgr->GetInstance()->Get<Material>("Default Material"))
				return "Default Material";

			if (mgr->GetInstance()->Get<Material>(mpath.string() + " Material"))
				return (mpath.string() + " Material");

			Texture* diffuse = nullptr;
			Texture* roughness = nullptr;
			Texture* metallic = nullptr;
			Texture* normal = nullptr;

			if (!mpath.empty())
			{
				TextureLoadingParameters params{};
				params.ColorFormat = RHIColorFormat::IE_RHI_FORMAT_R8G8B8A8_SRGB;
				diffuse = mgr->Create<Texture>(FIND_PATH(mpath.string()), true, params);
				diffuse = diffuse ? diffuse : defaulTtexture;
			}
			else
			{
				diffuse = defaulTtexture;
			}

			material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &path);
			mpath = path.C_Str();
			mpath = mpath.filename();

			if (!mpath.empty())
			{
				roughness = mgr->Create<Texture>(FIND_PATH(mpath.string()));
				roughness = roughness ? roughness : defaulTtexture;
			}
			else
			{
				roughness = defaulTtexture;
			}

			material->GetTexture(aiTextureType_METALNESS, 0, &path);
			mpath = path.C_Str();
			mpath = mpath.filename();

			if (!mpath.empty())
			{
				metallic = mgr->Create<Texture>(FIND_PATH(mpath.string()));
				metallic = metallic ? metallic : defaulTtexture;
			}
			else
			{
				metallic = defaulTtexture;
			}

			material->GetTexture(aiTextureType_NORMALS, 0, &path);
			mpath = path.C_Str();
			mpath = mpath.filename();

			if (!mpath.empty())
			{
				normal = mgr->Create<Texture>(FIND_PATH(mpath.string()));
				normal = normal ? normal : defaulTtexture;
			}
			else
			{
				normal = mgr->Get<Texture>("DefaultNormal.jpg");
			}

			LitPBRInputs inputs;
			inputs.SetAlbedo(diffuse);
			inputs.SetRoughnessTexture(roughness);
			inputs.SetMetallicTexture(metallic);
			inputs.SetNormal(normal);

			MaterialData data{};
			data.ContainerName = (mpath.string() + " Material");
			data.Inputs = &inputs;

			mgr->CreateResourceContainer<Material>(&data);

			return data.ContainerName;
		}

		std::string AssimpLoader::ProcessGLTFMaterial(const aiMesh* _mesh, const aiScene* _scene)
		{
			aiMaterial* material = _scene->mMaterials[_mesh->mMaterialIndex];

			aiString path;

			ResourceManager* mgr = ResourceManager::GetInstance();

			std::string defaultName = "DefaultTexture.jpg";

			Texture* defaulTtexture = mgr->Get<Texture>(defaultName);

			material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			std::filesystem::path mpath = path.C_Str();

			if (mpath.empty() && mgr->GetInstance()->Get<Material>("Default Material"))
				return "Default Material";

			if (mgr->GetInstance()->Get<Material>(mpath.string() + " Material"))
				return (mpath.string() + " Material");

			const Texture* diffuse = nullptr;
			const Texture* metallicRoughness = nullptr;
			const Texture* normal = nullptr;

			if (!mpath.empty())
			{
				TextureLoadingParameters params{};
				params.ColorFormat = RHIColorFormat::IE_RHI_FORMAT_R8G8B8A8_SRGB;
				diffuse = mgr->Create<Texture>(FIND_PATH(mpath.filename().string()), true, params);
			}
			else
			{
				diffuse = defaulTtexture;
			}

			material->GetTexture(aiTextureType_GLTF_METALLIC_ROUGHNESS, 0, &path);
			mpath = path.C_Str();

			if (!mpath.empty())
			{
				metallicRoughness = mgr->Create<Texture>(FIND_PATH(mpath.filename().string()));
			}
			else
			{
				metallicRoughness = defaulTtexture;
			}

			material->GetTexture(aiTextureType_NORMALS, 0, &path);
			mpath = path.C_Str();

			if (!mpath.empty())
			{
				normal = mgr->Create<Texture>(FIND_PATH(mpath.filename().string()));
			}
			else
			{
				normal = mgr->Get<Texture>("DefaultNormal.jpg");
			}

			LitPBRInputs inputs;
			inputs.SetAlbedo(diffuse);
			inputs.SetRoughnessTexture(metallicRoughness);
			inputs.SetMetallicTexture(metallicRoughness);
			inputs.SetNormal(normal);

			MaterialData data{};
			data.ContainerName = (mpath.string() + " Material");
			data.Inputs = &inputs;
			data.IsGLTF = true;

			mgr->CreateResourceContainer<Material>(&data);

			return data.ContainerName;
		}
	}
}