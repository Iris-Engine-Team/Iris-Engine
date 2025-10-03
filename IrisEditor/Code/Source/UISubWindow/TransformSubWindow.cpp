#include "UISubWindow/TransformSubWindow.h"

//UITools
#include "UITools/ImGuiWidgets.h"

//SECS
#include "SECS/Components/Transform.h"

//IMGUI
#include "imgui.h"

namespace Iris
{
	namespace Editor
	{
		TransformSubWindow::TransformSubWindow()
		{
			p_Name = "Transform";
		}

		TransformSubWindow::~TransformSubWindow()
		{
			m_CurrentTransform = nullptr;
			delete m_CurrentTransform;
		}

		void TransformSubWindow::StartSubWindow()
		{
			ImGuiTreeNodeFlags flags = 0;
			//flags |= ImGuiTreeNodeFlags_;
			flags |= ImGuiTreeNodeFlags_DefaultOpen;
			flags |= ImGuiTreeNodeFlags_Selected;
			flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
			if (ImGui::TreeNodeEx(p_Name.c_str(), flags))
			{
				m_SubWindowOpened = true;
			}
		}
		void TransformSubWindow::CloseSubWindow()
		{
			if (m_SubWindowOpened)
			{
				ImGui::TreePop();
				m_SubWindowOpened = false;
			}
		}
		bool TransformSubWindow::DrawSubWindow()
		{
			Vector3 position = m_CurrentTransform->GetPosition();
			Vector3 rotation = m_CurrentTransform->GetEuler();
			Vector3 scaling = m_CurrentTransform->GetScale();

			if (m_SubWindowOpened && m_CurrentTransform)
			{
				ImGui::PushItemWidth(100);
				ImGuiWidgets::ImGuiVector3("Position", "Pos", position.GetXRef(), position.GetYRef(), position.GetZRef(), 0.005f, -FLT_MAX, FLT_MAX);

				ImGuiWidgets::ImGuiVector3("Rotation", "Rot", rotation.GetXRef(), rotation.GetYRef(), rotation.GetZRef(), 0.5f, -360, 360);

				ImGuiWidgets::ImGuiVector3("Scale", "Scale", scaling.GetXRef(), scaling.GetYRef(), scaling.GetZRef(), 0.05f, -FLT_MAX, FLT_MAX);

				ImGui::PopItemWidth();
			}

			if (position != m_CurrentTransform->GetPosition())
				p_ModifiedValue = true;
			else if (rotation != m_CurrentTransform->GetEuler())
				p_ModifiedValue = true;
			else if (scaling != m_CurrentTransform->GetScale())
				p_ModifiedValue = true;

			m_CurrentTransform->SetPosition(position);
			m_CurrentTransform->SetEuler(rotation);
			m_CurrentTransform->SetScale(scaling);

			return p_ModifiedValue;
		}

		void TransformSubWindow::SetTransform(Engine::ITransform* _selectedTransform)
		{
			m_CurrentTransform = _selectedTransform;
		}
	}
}