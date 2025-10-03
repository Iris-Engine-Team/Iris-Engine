#pragma once
#include "UISubWindow/ImGuiSubWindow.h"

#include "imgui.h"

//STD
#include <string>

namespace Iris
{
	namespace Engine
	{
		class Material;
	}
	namespace Editor
	{
		class MaterialSubWindow : public ImGuiSubWindow
		{
		public:
			MaterialSubWindow();
			~MaterialSubWindow() override final = default;

			void StartSubWindow() override;
			void CloseSubWindow() override;
			bool DrawSubWindow() override;

			void DrawOverallMaterialParameters();

			void DrawLitExclusiveMaterialParameters();

			void DrawMaterialType();
			void DrawTransparency();
			void DrawDrawMode();
			void DrawAlbedo();

			//LIT
			void DrawRoughness();
			void DrawGLTFMetallic();
			void DrawMetallic();
			void DrawNormal();

			void SetMaterial(Engine::Material* _mat);

			void SetDragDropTarget(int _materialPart);

		private:
			Engine::Material* m_Material = nullptr;

			bool m_RoughnessFloat = true;
			bool m_MetallicFloat = true;

			float m_Step = 0.005f;
			ImTextureID m_DefaultNoneTexture = 0;
			bool m_CurrentMatIsLit = false;
			float m_MinValue = 0;
			float m_MaxValue = 1;

			bool m_SubWindowOpened = false;

			std::vector<ImTextureID> m_TexturesID;
		};
	}
}