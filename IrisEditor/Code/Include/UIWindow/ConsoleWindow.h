#pragma once

#include "UIWindow/ImGuiWindow.h"

//ENGINETOOLS
#include "Core/Tools/Log.h"

namespace Iris
{
	namespace Engine
	{

	}
	namespace Editor
	{
		class ConsoleWindow : public ImGuiEditorWindow
		{
		public:
			ConsoleWindow(std::string _hiddenID);
			~ConsoleWindow() override;

			void StartWindow(bool _lightMode) override;
			void CloseWindow() override;
			bool DrawWindow(unsigned int _cFrame) override;

			void AddLog(const std::string& _log, Vector3 _color);
		private:
			std::vector<std::pair<std::string, Vector3>> m_LogLines;

			Debug::Log::ListenerID m_ListenerID;
		};
	}
}