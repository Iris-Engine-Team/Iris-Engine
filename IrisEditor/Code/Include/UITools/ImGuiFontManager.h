#pragma once

#include <unordered_map>
#include <string>
#include "imgui.h"

namespace Iris
{
	namespace Editor
	{
		class FontManager
		{
		public:
			static ImFont* GetFontFromName(std::string _name);
			static void AddFontWithName(std::string _name, ImFont* _font);
			static void DeleteEachFont();
		private:
			static std::unordered_map<std::string, ImFont*> m_Fonts;
		};
	}
}