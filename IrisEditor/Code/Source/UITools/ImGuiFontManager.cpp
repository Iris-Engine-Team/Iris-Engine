#include "UITools/ImGuiFontManager.h"

namespace Iris
{
	namespace Editor
	{
		std::unordered_map<std::string, ImFont*> FontManager::m_Fonts;

		ImFont* FontManager::GetFontFromName(std::string _name)
		{
			return m_Fonts[_name];
		}
		void FontManager::AddFontWithName(std::string _name, ImFont* _font)
		{
			m_Fonts[_name] = _font;
		}
		void FontManager::DeleteEachFont()
		{
			auto it = m_Fonts.begin();
			for (it; it != m_Fonts.end(); it++)
			{
				it->second->ClearOutputData();
			}
			m_Fonts.clear();
		}
	}
}
