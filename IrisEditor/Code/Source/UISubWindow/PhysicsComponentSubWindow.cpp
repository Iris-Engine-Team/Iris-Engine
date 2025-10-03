#include "UISubWindow/PhysicsComponentSubWindow.h"

#include "SECS/Components/RigidBody.h"
#include "SECS/Components/Collider.h"

#include "UITools/ImGuiWidgets.h"

#include "imgui.h"

namespace Iris
{
	namespace Editor
	{
		PhysicsComponentSubWindow::PhysicsComponentSubWindow()
		{
			p_Name = "Physics";
		}

		PhysicsComponentSubWindow::~PhysicsComponentSubWindow()
		{
			m_CurrentRigidBody = nullptr;
			m_CurrentCollider = nullptr;
		}

		void PhysicsComponentSubWindow::StartSubWindow()
		{
			ImGuiTreeNodeFlags flags = 0;
			//flags |= ImGuiTreeNodeFlags_OpenOnArrow;
			flags |= ImGuiTreeNodeFlags_Selected;
			flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
			if (m_CurrentRigidBody && m_CurrentCollider)
			{

				if (ImGui::TreeNodeEx(p_Name.c_str(), flags))
				{
					m_SubWindowOpened = true;
					if (!m_CurrentRigidBody->IsEnable() && !m_CurrentCollider->IsEnable())
						ImGui::BeginDisabled();
				}
			}
		}

		void PhysicsComponentSubWindow::CloseSubWindow()
		{
			if (m_SubWindowOpened)
			{
				if (!m_CurrentRigidBody->IsEnable() && !m_CurrentCollider->IsEnable())
					ImGui::EndDisabled();
				ImGui::TreePop();
				m_SubWindowOpened = false;
			}
		}

		bool PhysicsComponentSubWindow::DrawSubWindow()
		{
			if (m_SubWindowOpened)
			{
				if (m_CurrentRigidBody)
				{
					DrawRigidBodyOption();
				}

				if (m_CurrentRigidBody && m_CurrentCollider)
				{
					ImGui::Separator();
				}

				if (m_CurrentCollider)
				{
					DrawColliderOption();
				}
			}
			return p_ModifiedValue;
		}

		void PhysicsComponentSubWindow::DrawRigidBodyOption()
		{
			ImGui::SeparatorText("RigidBody");

			DrawMotionTypeOption();

			float mass = m_CurrentRigidBody->GetMass();
			ImGui::Text("Mass");
			ImGui::SameLine();
			ImGui::Spacing();
			ImGui::SameLine();
			ImGui::DragFloat("Kg##RigidbodyMass", &mass, 0.05f, EPSILON, FLT_MAX, "%.3f");

			if (mass != m_CurrentRigidBody->GetMass())
				p_ModifiedValue = true;

			m_CurrentRigidBody->SetMass(mass);

			float friction = m_CurrentRigidBody->GetFriction();
			ImGui::Text("Friction");
			ImGui::SameLine();
			ImGui::Spacing();
			ImGui::SameLine();
			ImGui::DragFloat("N##RigidbodyFriction", &friction, 0.05f, -FLT_MAX, FLT_MAX, "%.3f");

			if (friction != m_CurrentRigidBody->GetFriction())
				p_ModifiedValue = true;

			m_CurrentRigidBody->SetFriction(friction);

			bool trigger = m_CurrentRigidBody->IsTrigger();
			ImGui::Checkbox("Trigger##Rigidbody", &trigger);

			if (trigger != m_CurrentRigidBody->IsTrigger())
				p_ModifiedValue = true;

			m_CurrentRigidBody->SetIsTrigger(trigger);

			bool interpolatePosition = m_CurrentRigidBody->IsInterpolatePosition();
			ImGui::Checkbox("Interpolate Position##Rigidbody", &interpolatePosition);

			if (interpolatePosition != m_CurrentRigidBody->IsInterpolatePosition())
				p_ModifiedValue = true;

			m_CurrentRigidBody->SetInterpolatePosition(interpolatePosition);

			Engine::Constraint constraint = m_CurrentRigidBody->GetConstraint();

			Vector3 rotationConstraints = constraint.GetRotationConstraint();
			bool xRotationConstraints = rotationConstraints.GetX() > 0;
			bool yRotationConstraints = rotationConstraints.GetY() > 0;
			bool zRotationConstraints = rotationConstraints.GetZ() > 0;

			ImGui::Text("Constraints");

			ImGui::Text("Rotation");
			ImGui::SameLine();
			ImGui::Spacing();
			ImGui::SameLine();
			ImGui::Checkbox("x##RigidbodyRotationConstraints", &xRotationConstraints);
			ImGui::SameLine();
			ImGui::Checkbox("y##RigidbodyRotationConstraints", &yRotationConstraints);
			ImGui::SameLine();
			ImGui::Checkbox("z##RigidbodyRotationConstraints", &zRotationConstraints);

			if (rotationConstraints != constraint.GetRotationConstraint())
				p_ModifiedValue = true;

			constraint.SetRotationConstraint(xRotationConstraints, yRotationConstraints, zRotationConstraints);

			m_CurrentRigidBody->SetRotationConstraint(constraint);
		}

		void PhysicsComponentSubWindow::DrawMotionTypeOption()
		{
			std::vector<std::string> itemsRigidbody{ "Static","Dynamic","Kinematic" };
			int itemSelectedIdxRigidbody = 0;

			switch (m_CurrentRigidBody->GetMotionType())
			{
			case Engine::MotionType::IE_Dynamic:
				itemSelectedIdxRigidbody = 1;
				break;
			case Engine::MotionType::IE_Kinematic:
				itemSelectedIdxRigidbody = 2;
				break;
			default:
				itemSelectedIdxRigidbody = 0;
				break;
			}
			const char* comboPreviewValueRigidbody = itemsRigidbody[itemSelectedIdxRigidbody].c_str();

			if (ImGui::BeginCombo("Motion Type##Rigidbody", comboPreviewValueRigidbody))
			{
				for (int n = 0; n < itemsRigidbody.size(); n++)
				{
					const bool isSelected = (itemSelectedIdxRigidbody == n);
					if (ImGui::Selectable(itemsRigidbody[n].c_str(), isSelected))
					{
						itemSelectedIdxRigidbody = n;
						switch (itemSelectedIdxRigidbody)
						{
						case 0:
							m_CurrentRigidBody->SetMotionType(Engine::MotionType::IE_Static);
							break;
						case 1:
							m_CurrentRigidBody->SetMotionType(Engine::MotionType::IE_Dynamic);
							break;
						case 2:
							m_CurrentRigidBody->SetMotionType(Engine::MotionType::IE_Kinematic);
							break;
						}
						p_ModifiedValue = true;
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}

		void PhysicsComponentSubWindow::DrawColliderOption()
		{
			ImGui::SeparatorText("Collider");

			DrawColliderShapeOption();

			if (m_CurrentCollider->GetShape()->GetShapeType() == Engine::PhysicShape::IE_SHAPE_BOX)
			{
				Vector3 boxSize = m_CurrentCollider->GetShapeInfo()->GetBoxSize();
				ImGui::PushItemWidth(100);
				ImGuiWidgets::ImGuiVector3("Size", "ColliderSize", boxSize.GetXRef(), boxSize.GetYRef(), boxSize.GetZRef(), 0.05f, EPSILON, FLT_MAX);
				ImGui::PopItemWidth();

				if (boxSize != m_CurrentCollider->GetShapeInfo()->GetBoxSize())
					p_ModifiedValue = true;

				Engine::ShapeParam sp;
				sp.InitBoxInfo(boxSize);
				m_CurrentCollider->SetShapeInfo(sp);
			}
			else if (m_CurrentCollider->GetShape()->GetShapeType() == Engine::PhysicShape::IE_SHAPE_SPHERE)
			{
				float size = m_CurrentCollider->GetShapeInfo()->GetSphereRadius();

				ImGui::Text("Radius");
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::DragFloat("##Collider", &size, 0.05f, EPSILON, FLT_MAX, "%.3f");

				if (size != m_CurrentCollider->GetShapeInfo()->GetSphereRadius())
					p_ModifiedValue = true;

				Engine::ShapeParam sp;
				sp.InitSphereInfo(size);
				m_CurrentCollider->SetShapeInfo(sp);
			}
			else if (m_CurrentCollider->GetShape()->GetShapeType() == Engine::PhysicShape::IE_SHAPE_CAPSULE)
			{
				float casuleHeight = m_CurrentCollider->GetShapeInfo()->GetCapsuleHeight();
				float capsuleRadius = m_CurrentCollider->GetShapeInfo()->GetCapsuleRadius();

				ImGui::Text("Height");
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::DragFloat("##ColliderHeight", &casuleHeight, 0.05f, EPSILON, FLT_MAX, "%.3f");

				ImGui::Text("Radius");
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::DragFloat("##ColliderRadius", &capsuleRadius, 0.05f, EPSILON, FLT_MAX, "%.3f");

				if (casuleHeight != m_CurrentCollider->GetShapeInfo()->GetCapsuleHeight() || capsuleRadius != m_CurrentCollider->GetShapeInfo()->GetCapsuleRadius())
					p_ModifiedValue = true;

				Engine::ShapeParam sp;
				sp.InitCapsuleInfo(capsuleRadius, casuleHeight);
				m_CurrentCollider->SetShapeInfo(sp);
			}

			Vector3 offSetPos = m_CurrentCollider->GetOffSet();
			ImGui::PushItemWidth(100);
			ImGuiWidgets::ImGuiVector3("OffSet", "ColliderOffset", offSetPos.GetXRef(), offSetPos.GetYRef(), offSetPos.GetZRef(), 0.005f, -FLT_MAX, FLT_MAX);
			ImGui::PopItemWidth();

			if (offSetPos != m_CurrentCollider->GetOffSet())
				p_ModifiedValue = true;

			m_CurrentCollider->SetOffSet(offSetPos);
		}

		void PhysicsComponentSubWindow::DrawColliderShapeOption()
		{
			std::vector<std::string> itemsCollider{ "Box Collider","Sphere Collider", "Capsule Collider" };
			int itemSelectedIdxCollider = 0;

			switch (m_CurrentCollider->GetShape()->GetShapeType())
			{
			case Engine::PhysicShape::IE_SHAPE_BOX:
				itemSelectedIdxCollider = 0;
				break;
			case Engine::PhysicShape::IE_SHAPE_SPHERE:
				itemSelectedIdxCollider = 1;
				break;
			case Engine::PhysicShape::IE_SHAPE_CAPSULE:
				itemSelectedIdxCollider = 2;
				break;
			default:
				itemSelectedIdxCollider = 0;
				break;
			}

			const char* comboPreviewValueCollider = itemsCollider[itemSelectedIdxCollider].c_str();

			if (ImGui::BeginCombo("Collider Shape##Collider", comboPreviewValueCollider))
			{
				for (int n = 0; n < itemsCollider.size(); n++)
				{
					const bool isSelected = (itemSelectedIdxCollider == n);
					if (ImGui::Selectable(itemsCollider[n].c_str(), isSelected))
					{
						itemSelectedIdxCollider = n;
						switch (itemSelectedIdxCollider)
						{
						case 0:
							m_CurrentCollider->SetShapeInfo(Engine::PhysicShape::IE_SHAPE_BOX, Engine::ShapeParam());
							break;
						case 1:
							m_CurrentCollider->SetShapeInfo(Engine::PhysicShape::IE_SHAPE_SPHERE, Engine::ShapeParam());
							break;
						case 2:
							m_CurrentCollider->SetShapeInfo(Engine::PhysicShape::IE_SHAPE_CAPSULE, Engine::ShapeParam());
							break;
						}
						p_ModifiedValue = true;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}

		void PhysicsComponentSubWindow::SetRigidBodyComponent(Engine::RigidBody* _selectedRigidBody, Engine::Collider* _selectedCollider)
		{
			m_CurrentRigidBody = _selectedRigidBody;
			m_CurrentCollider = _selectedCollider;
		}
	}
}
