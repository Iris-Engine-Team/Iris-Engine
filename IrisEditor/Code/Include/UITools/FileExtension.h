#pragma once

#include <vector>
#include <string>

namespace Iris
{
	namespace Editor
	{
		class FileExtension
		{
		public:
			inline static std::vector<std::string> GetMeshExtention() { return meshExtension; }
			inline static std::vector<std::string> GetTextureExtention() { return textureExtension; }
		private:
			inline static std::vector<std::string> meshExtension = { ".obj", ".fbx", ".gltf", ".ply", ".glb"};
			inline static std::vector<std::string> textureExtension = { ".png", ".jpg", ".jpeg", ".bmp", ".hdr", ".tga", ".psd"};
		};
	}
}