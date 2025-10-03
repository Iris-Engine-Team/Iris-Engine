#pragma once

#include "Core/Tools/Macros.h"

#include "Loader/ILoader.h"
#include "Loader/MeshLoader/IMeshLoader.h"
#include "Loader/TextureLoader/ITextureLoader.h"
#include "Loader/IrisShaderTools.h"

#include <typeindex>

#include "Resources/Mesh.h"
#include "Resources/Material.h"
#include "Resources/Texture.h"
#include "Resources/Shader.h"

namespace Iris
{
	namespace Engine
	{
		enum class MeshLoaderType : int
		{
			IE_MESH_LOADER_TYPE_ASSIMP = 0
		};

		enum class TextureLoaderType : int
		{
			IE_TEXTURE_LOADER_TYPE_STBIMAGE = 0
		};


		class IrisLoader
		{
		private:
			IMeshLoader* m_MeshLoader = nullptr;
			ITextureLoader* m_TextureLoader = nullptr;
			IrisShaderTools m_ShaderTools{};


		public:
			void Init(MeshLoaderType _meshLoader = MeshLoaderType::IE_MESH_LOADER_TYPE_ASSIMP, TextureLoaderType _textureLoader = TextureLoaderType::IE_TEXTURE_LOADER_TYPE_STBIMAGE,
				ShaderLoaderType _shaderLoader = ShaderLoaderType::IE_SHADER_LOADER_TYPE_IRIS, ShaderCompilerType _shaderCompiler = ShaderCompilerType::IE_SHADER_COMPILER_TYPE_SHADERC, ShaderInspectorType _shaderInspector = ShaderInspectorType::IE_SHADER_INSPECTOR_TYPE_SPIRV);

			void Terminate();

			IRIS_ENGINE_API IE_FORCE_INLINE IMeshLoader* GetMeshLoader() { return m_MeshLoader; }
			IRIS_ENGINE_API IE_FORCE_INLINE ITextureLoader* GetTextureLoader() { return m_TextureLoader; }
			IRIS_ENGINE_API IE_FORCE_INLINE IrisShaderTools* GetShaderTools() { return &m_ShaderTools; }

			template<typename T>
			ILoader* GetLoader()
			{
				std::type_index type = typeid(T);

				if (type == typeid(Mesh) || type == typeid(IMeshLoader))
					return m_MeshLoader;

				else if (type == typeid(Texture) || type == typeid(ITextureLoader))
					return m_TextureLoader;
				
				else if (type == typeid(Shader) || type == typeid(IrisShaderTools) || type == typeid(IShaderLoader)
					|| type == typeid(IShaderCompiler) || type == typeid(IShaderInspector))
					return m_ShaderTools.GetSubPart<T>();
				
				return nullptr;
			}
		};
	}
}
