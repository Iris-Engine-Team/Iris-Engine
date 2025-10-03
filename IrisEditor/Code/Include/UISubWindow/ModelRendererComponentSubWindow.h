#pragma once
#include "UISubWindow/ImGuiSubWindow.h"

//STD
#include <string>

//IMGUI
#include "imgui.h"

namespace Iris
{
	namespace Engine
	{
		class ModelRenderer;
		struct InitEditorContext;
	}
	namespace Editor
	{
		class InspectorWindow;

		class ModelRendererComponentSubWindow : public ImGuiSubWindow
		{
		public:
			ModelRendererComponentSubWindow(Engine::InitEditorContext* _initializer, InspectorWindow* _inspector);
			~ModelRendererComponentSubWindow() override final = default;

			void StartSubWindow() override;
			void CloseSubWindow() override;
			bool DrawSubWindow() override;

			void DrawMeshParameters();

			void SetDragDropTarget();

			void SetModelRendererComponent(Engine::ModelRenderer* _selectedModel);
		private:

			Engine::ModelRenderer* m_CurrentModel = nullptr;

			bool m_SubWindowOpened = false;

			Engine::InitEditorContext* m_Initializer = nullptr;

			InspectorWindow* m_InspectorRef = nullptr;
		};
	}
}