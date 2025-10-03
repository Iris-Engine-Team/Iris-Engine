#include "Loader/ShaderLoader/IrisShaderLoader.h"

namespace Iris
{
	namespace Engine
	{
		bool IShaderLoader::IsExtensionValid(std::filesystem::path _extension) const
		{
			if (_extension != ".vert" && _extension != ".frag" && _extension != ".geo")
				return false;
			
			return true;
		}
	}
}