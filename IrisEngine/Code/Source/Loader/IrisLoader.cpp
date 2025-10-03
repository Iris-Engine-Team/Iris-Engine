#include "Loader/IrisLoader.h"

#include "Loader/MeshLoader/AssimpLoader.h"
#include "Loader/TextureLoader/StbImageLoader.h"
#include "Core/Tools/Log.h"

namespace Iris
{
	namespace Engine
	{
		void IrisLoader::Init(MeshLoaderType _meshLoader, TextureLoaderType _textureLoader, ShaderLoaderType _shaderLoader, ShaderCompilerType _shaderCompiler, ShaderInspectorType _shaderInspector)
		{
			switch (_meshLoader)
			{
			case Iris::Engine::MeshLoaderType::IE_MESH_LOADER_TYPE_ASSIMP:
				DEBUG_LOG("Mesh loader of type %i created.", 0);
				m_MeshLoader = new AssimpLoader();
				break;
			default:
				DEBUG_WARN("Can't create mesh loader of type %i.", static_cast<int>(_meshLoader));
				break;
			}

			switch (_textureLoader)
			{
			case Iris::Engine::TextureLoaderType::IE_TEXTURE_LOADER_TYPE_STBIMAGE:
				DEBUG_LOG("Texture loader of type %i created.", 0);
				m_TextureLoader = new StbImageLoader();
				break;
			default:
				DEBUG_WARN("Can't create texture loader of type %i.", static_cast<int>(_textureLoader));
				break;
			}

			m_ShaderTools.Init(_shaderLoader, _shaderCompiler, _shaderInspector);
		}

		void IrisLoader::Terminate()
		{
			if (m_MeshLoader)
			{
				DEBUG_LOG("Mesh loader deleted.");
				delete m_MeshLoader;
				m_MeshLoader = nullptr;
			}

			if (m_TextureLoader)
			{
				DEBUG_LOG("Texture loader deleted.");
				delete m_TextureLoader;
				m_TextureLoader = nullptr;
			}

			m_ShaderTools.Terminate();
		}
	}
}