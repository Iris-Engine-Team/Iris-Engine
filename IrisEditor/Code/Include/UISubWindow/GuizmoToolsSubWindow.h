#pragma once
#include "UISubWindow/ImGuiSubWindow.h"

//MATH
#include "Vector/Vector2.h"

//STD
#include <string>
#include "tuple"

//IMGUI
#include "imgui.h"
#include "ImGuizmo.h"

class Vector2;

namespace Iris
{
	namespace Editor
	{
		class SceneViewportWindow;

		class GuizmoToolsSubWindow : public ImGuiSubWindow
		{
		public:
			GuizmoToolsSubWindow(SceneViewportWindow* _sceneWindow);
			~GuizmoToolsSubWindow() override final = default;

			void StartSubWindow() override;
			void CloseSubWindow() override;
			bool DrawSubWindow() override;
			void ChangeWindowPos(Vector2 _pos);

		private:
			ImGuizmo::OPERATION m_CurrentOperation = ImGuizmo::OPERATION::TRANSLATE;
			ImGuizmo::MODE m_CurrentModicationMode = ImGuizmo::MODE::WORLD;

			Vector2 pos = Vector2(10, 10);

			SceneViewportWindow* m_SceneWindow = nullptr;

			bool m_SubWindowOpened = false;
		};
	}
}