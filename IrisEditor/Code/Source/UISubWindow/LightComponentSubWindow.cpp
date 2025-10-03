#include "UISubWindow/LightComponentSubWindow.h"

//SECS
#include "SECS/Components/LightComponent.h"

//UITOOLS
#include "UITools/ImGuiWidgets.h"

//IMGUI
#include "imgui.h"

namespace Iris
{
	namespace Editor
	{
		LightComponentSubWindow::LightComponentSubWindow()
		{
			p_Name = "Light";
		}

		LightComponentSubWindow::~LightComponentSubWindow()
		{
			m_CurrentLight = nullptr;
			delete m_CurrentLight;
		}

		void LightComponentSubWindow::StartSubWindow()
		{
			ImGuiTreeNodeFlags flags = 0;
			//flags |= ImGuiTreeNodeFlags_OpenOnArrow;
			flags |= ImGuiTreeNodeFlags_Selected;
			flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
			if (m_CurrentLight)
			{
				if (ImGui::TreeNodeEx(p_Name.c_str(), flags))
				{
					if (!m_CurrentLight->IsEnable())
						ImGui::BeginDisabled();
					m_SubWindowOpened = true;
				}
			}
		}

		void LightComponentSubWindow::CloseSubWindow()
		{
			if (m_SubWindowOpened)
			{
				if (!m_CurrentLight->IsEnable())
					ImGui::EndDisabled();
				ImGui::TreePop();
				m_SubWindowOpened = false;
			}
		}

		bool LightComponentSubWindow::DrawSubWindow()
		{
			if (m_SubWindowOpened)
			{
				const char* items[] = { "PointLight", "SpotLight", "DirectionalLight" };
				int itemSelectedIdx = 0; // Here we store our selection data as an index.

				switch (m_CurrentLight->GetType())
				{
				case Engine::LightType::IE_POINT_LIGHT:
					itemSelectedIdx = 0;
					break;
				case Engine::LightType::IE_SPOT_LIGHT:
					itemSelectedIdx = 1;
					break;
				case Engine::LightType::IE_DIRECTIONAL_LIGHT:
					itemSelectedIdx = 2;
					break;
				default:
					break;
				}
				// Pass in the preview value visible before opening the combo
				const char* comboPreviewValue = items[itemSelectedIdx];
				if (ImGui::BeginCombo("Type##Light", comboPreviewValue))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						const bool isSelected = (itemSelectedIdx == n);
						if (ImGui::Selectable(items[n], isSelected))
						{
							itemSelectedIdx = n;

							switch (itemSelectedIdx)
							{
							case 0:
								m_CurrentLight->SetType(Engine::LightType::IE_POINT_LIGHT);
								break;
							case 1:
								m_CurrentLight->SetType(Engine::LightType::IE_SPOT_LIGHT);
								break;
							case 2:
								m_CurrentLight->SetType(Engine::LightType::IE_DIRECTIONAL_LIGHT);
								break;
							default:
								break;
							}

							p_ModifiedValue = true;
						}
						// Set the initial focus when opening the combo
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				DrawLightParameters();
			}
			return p_ModifiedValue;
		}

		void LightComponentSubWindow::DrawLightParameters()
		{
			ImGui::PushItemWidth(100);
			Vector4 col = m_CurrentLight->GetColor();
			ImGui::ColorPicker4("LightColor", &col.GetXRef());

			if(col != m_CurrentLight->GetColor())
				p_ModifiedValue = true;

			m_CurrentLight->SetColor(col);
			ImGui::PopItemWidth();

			float intensity = m_CurrentLight->GetIntensity();
			float cutOff = m_CurrentLight->GetCutOff();
			float outerCutOff = m_CurrentLight->GetOuterCutOff();
			float lightNear = m_CurrentLight->GetNear();
			float lightFar = m_CurrentLight->GetFar();

			ImGui::DragFloat("Intensity", &intensity, 0.005f, 0.f, 100.f, "%.3f");

			if (intensity != m_CurrentLight->GetIntensity())
				p_ModifiedValue = true;

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf;

			if (m_CurrentLight->GetType() == Engine::LightType::IE_SPOT_LIGHT)
			{
				if (ImGui::TreeNodeEx("SpotAngle", flags))
				{
					ImGui::DragFloat("Cutoff", &cutOff, 0.05f, 1.f, 180.f, "%.3f");

					if (cutOff != m_CurrentLight->GetCutOff())
						p_ModifiedValue = true;

					ImGui::DragFloat("Outer Cutoff", &outerCutOff, 0.05f, 1.f, 180.f, "%.3f");

					if (outerCutOff != m_CurrentLight->GetOuterCutOff())
						p_ModifiedValue = true;

					ImGui::TreePop();
				}
			}
			if (ImGui::TreeNodeEx("Range", flags))
			{
				ImGui::DragFloat("Near", &lightNear, 0.05f, 0.01f, FLT_MAX, "%.3f");

				if (lightNear != m_CurrentLight->GetNear())
					p_ModifiedValue = true;

				ImGui::DragFloat("Far", &lightFar, 0.05f, 0.01f, FLT_MAX, "%.3f");

				if (lightFar != m_CurrentLight->GetFar())
					p_ModifiedValue = true;

				ImGui::TreePop();
			}
			m_CurrentLight->SetIntensity(intensity);
			m_CurrentLight->SetCutOff(cutOff);
			m_CurrentLight->SetOuterCutOff(outerCutOff);
			m_CurrentLight->SetNear(lightNear);
			m_CurrentLight->SetFar(lightFar);

			if (m_CurrentLight->GetType() == Engine::LightType::IE_DIRECTIONAL_LIGHT)
			{
				ImGui::SeparatorText("Shadow Parameter");
				float bottom = m_CurrentLight->GetBottom();
				float top = m_CurrentLight->GetTop();
				float left = m_CurrentLight->GetLeft();
				float right = m_CurrentLight->GetRight();

				ImGui::DragFloat("Bottom##Shadow", &bottom, 0.5f, -1000.f, 1000.f, "%.3f");
				ImGui::DragFloat("Top##Shadow", &top, 0.5f, -1000.f, 1000.f, "%.3f");
				ImGui::DragFloat("Left##Shadow", &left, 0.5f, -1000.f, 1000.f, "%.3f");
				ImGui::DragFloat("Right##Shadow", &right, 0.5f, -1000.f, 1000.f, "%.3f");

				m_CurrentLight->SetBottom(bottom);
				m_CurrentLight->SetTop(top);
				m_CurrentLight->SetLeft(left);
				m_CurrentLight->SetRight(right);
			}
		}

		void LightComponentSubWindow::SetLightComponent(Engine::LightComponent* _selectedLight)
		{
			m_CurrentLight = _selectedLight;
		}
	}
}