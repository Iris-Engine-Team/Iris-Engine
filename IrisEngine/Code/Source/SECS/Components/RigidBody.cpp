#include "SECS/Components/RigidBody.h"
#include "SECS/Systems/PhysicsSystem.h"
#include "SECS/Components/Collider.h"
#include "SECS/Components/ScriptComponent.h"
#include "Core/Physics/ContactListener.h"

#include "Resources/Metadata.h"

namespace Iris
{
	namespace Engine
	{
		Collider* IPhysicBody::GetCollider()
		{
			if (p_Collider)
				return p_Collider->UseComponent();

			return nullptr;
		}

		void IPhysicBody::SetCollider(Collider* _collider)
		{
			if (!p_Collider)
				p_Collider = new ComponentHandle<Collider>(_collider);
			else if (!_collider && p_Collider)
			{
				Collider* col = p_Collider->UseComponent();
				if (col)
					col->LinkRigidBody(nullptr);
				delete p_Collider;
				p_Collider = nullptr;
			}
		}

		void RigidBody::Serialize(IMetadata* _metadata)
		{
			_metadata->Write("Enable", p_Enable);

			Vector3 constraint = m_Constraint.GetRotationConstraint();
			std::vector<bool> constraintVec = { static_cast<bool> (constraint[0]),static_cast<bool> (constraint[1]), static_cast<bool> (constraint[2]) };
			_metadata->Write("Rotation Constraint", constraintVec);

			_metadata->Write("Layer", m_Layer);

			_metadata->Write("Motion Type", m_MotionType);

			_metadata->Write("Mass", m_Mass);

			_metadata->Write("Friction", m_Friction);

			_metadata->Write("Is Trigger", m_IsTrigger);
		}

		void RigidBody::Deserialization(IMetadata* _metadata, InitEditorContext& _initializer)
		{
			m_PhysicsSystem = _initializer.InitScene->GetPhysicsSystem();

			std::vector<std::string> path = { "Scene Data", "Components", std::to_string(static_cast<int>(GetID())), GetName(), std::to_string(p_IrisObject->GetID()), "" };

			path[5] = "Motion Type";
			m_MotionType = _metadata->GetData(path);

			path[5] = "Is Trigger";
			m_IsTrigger = _metadata->GetData(path);

			path[5] = "Layer";
			m_Layer = _metadata->GetData(path);

			path[5] = "Mass";
			m_Mass = _metadata->GetData(path);

			path[5] = "Friction";
			m_Friction = _metadata->GetData(path);

			path[5] = "Rotation Constraint";
			std::vector<bool> constraintVec = _metadata->GetData(path);
			m_Constraint.SetRotationConstraint(constraintVec[0], constraintVec[1], constraintVec[2]);

			Collider* col = p_IrisObject->GetComponent<Collider>();
			if (col)
			{
				m_PhysicBody = m_PhysicsSystem->CreateBody();

				m_PhysicBody->Init(m_PhysicsSystem, col, p_IrisObject->GetTransform(), m_MotionType, m_Layer);
				col->LinkRigidBody(this);

				m_PhysicBody->SetFriction(m_Friction);
				m_PhysicBody->SetIsTrigger(m_IsTrigger);

				if (m_MotionType != MotionType::IE_Static)
					m_PhysicBody->SetMass(m_Mass);

				std::vector<std::string> pathTrans = { "Scene Data", "Components", std::to_string(static_cast<int>(ITransform::GetID())), ITransform::GetName(), std::to_string(p_IrisObject->GetID()), "Global Rotation" };
				std::vector<float> quat = _metadata->GetData(pathTrans);
				SetRotation(Quaternion(quat[0], quat[1], quat[2], quat[3]));
				SetRotationConstraint(m_Constraint, false);
				OnActivated();
			}

			path[5] = "Enable";
			p_Enable = !_metadata->GetData(path);
			SetEnable(!p_Enable);
		}

		void RigidBody::Init(IPhysicsSystem* _physicsSystem, MotionType _motionType, Collider* _collider)
		{
			m_PhysicsSystem = _physicsSystem;
			m_MotionType = _motionType;

			m_Layer = Layers::IE_MOVING;
			if (m_MotionType == MotionType::IE_Static)
				m_Layer = Layers::IE_NON_MOVING;

			Collider* col = _collider;
			if (!col)
				col = p_IrisObject->GetComponent<Collider>();

			if (col)
			{
				m_PhysicBody = m_PhysicsSystem->CreateBody();


				m_PhysicBody->Init(m_PhysicsSystem, col, p_IrisObject->GetTransform(), m_MotionType, m_Layer);
				col->LinkRigidBody(this);

				m_PhysicBody->SetFriction(m_Friction);
				m_PhysicBody->SetIsTrigger(m_IsTrigger);

				if (m_MotionType != MotionType::IE_Static)
					m_PhysicBody->SetMass(m_Mass);

				SetRotationConstraint(m_Constraint, false);
				OnActivated();
			}
		}

		void RigidBody::Init(IPhysicsSystem* _physicsSystem)
		{
			Init(_physicsSystem, MotionType::IE_Dynamic);
		}

		void RigidBody::Init(InitEditorContext& _initializer)
		{
			Init(_initializer.InitScene->GetPhysicsSystem());
		}

		void RigidBody::Destroy()
		{
			PermanentlyRemoveFromPhysicsSystem();
		}

		void RigidBody::AddToPhysicsSystem()
		{
			if (m_PhysicBody)
			{
				m_PhysicBody->AddBodyToPhysicsSystem(m_PhysicsSystem);
				m_PhysicBody->SetRotationConstraint(m_PhysicsSystem, m_Constraint);
			}
		}

		void RigidBody::RemoveFromPhysicsSystem()
		{
			if (m_PhysicBody)
			{
				m_PhysicBody->RemoveConstraint(m_PhysicsSystem);
				m_PhysicBody->RemoveFromPhysicsSystem(m_PhysicsSystem);
			}
		}

		void RigidBody::PermanentlyRemoveFromPhysicsSystem()
		{
			if (m_PhysicBody)
			{
				m_PhysicBody->RemoveConstraint(m_PhysicsSystem);
				m_PhysicBody->PermanentlyRemoveFromPhysicsSystem(m_PhysicsSystem);
				m_PhysicBody->Destroy();
				delete m_PhysicBody;
				m_PhysicBody = nullptr;
				m_PhysicsSystem = nullptr;
			}
		}

		void RigidBody::Copy(IComponent* _componentToCopy)
		{
			RigidBody* rb = dynamic_cast<RigidBody*>(_componentToCopy);
			if (rb)
			{
				IComponent::Copy(_componentToCopy);
				m_PhysicBody = rb->m_PhysicBody;
				rb->m_PhysicBody = nullptr;
				m_PhysicsSystem = rb->m_PhysicsSystem;

				m_Layer = rb->m_Layer;

				m_MotionType = rb->m_MotionType;

				m_ToUpdateShape = rb->m_ToUpdateShape;

				m_StartPos = rb->m_StartPos;
				m_CurrentPos = rb->m_CurrentPos;
				m_TargetPos = rb->m_TargetPos;

				m_Interpolate = rb->m_Interpolate;
				m_Mass = rb->m_Mass;
				m_Friction = rb->m_Friction;
				m_IsTrigger = rb->m_IsTrigger;

				m_Constraint = rb->m_Constraint;
			}
			else
			{
				DEBUG_LOG("cant copy this component ");
			}
		}

		void RigidBody::Duplicate(IComponent* _componentToDuplicate, InitEditorContext* _initializer)
		{
			RigidBody* rb = dynamic_cast<RigidBody*>(_componentToDuplicate);
			if (rb)
			{
				IComponent::Duplicate(_componentToDuplicate, _initializer);

				SetMass(rb->m_Mass);
				SetIsTrigger(rb->IsTrigger());
				SetFriction(rb->GetFriction());
				Init(rb->m_PhysicsSystem, rb->m_MotionType);
				
				m_Constraint = rb->m_Constraint;
				m_Layer = rb->m_Layer;
				if (m_PhysicBody)
				{
					m_PhysicBody->SetRotationConstraint(m_PhysicsSystem, m_Constraint);

					m_PhysicBody->SetLayer(m_PhysicsSystem, m_Layer);
				}

				m_Interpolate = rb->m_Interpolate;
			}
		}

		void RigidBody::SetEnable(bool _enable)
		{
			if (p_Enable == _enable)
				return;

			IComponent::SetEnable(_enable);
			
			if (!m_PhysicBody)
				return;

			p_UpdateActivation = true;
		}

		void RigidBody::ApplyActivation()
		{
			m_PhysicBody->ApplyActivation(m_PhysicsSystem);

			if (!p_UpdateActivation)
				return;

			p_UpdateActivation = false;

			if (p_Enable)
			{
				m_PhysicBody->SetLayer(m_PhysicsSystem, m_Layer);
				m_PhysicBody->SetMotionType(m_PhysicsSystem, m_MotionType);
				AddToPhysicsSystem();
			}
			else
			{
				m_PhysicBody->SetLayer(m_PhysicsSystem, Layers::IE_DISABLE);
				RemoveFromPhysicsSystem();
			}
		}
		
		void RigidBody::SetPosition(const Vector3& _position)
		{
			if (m_MotionType == MotionType::IE_Static)
			{
				m_TargetPos = _position;
				SetPhysiquePosition();
				return;
			}

			if (m_CurrentPos == _position)
				return;

			Vector3 diff = _position - m_CurrentPos;
			m_CurrentPos = _position;
			m_StartPos += diff;
			m_TargetPos += diff;
		}

		void RigidBody::SetPhysiquePosition()
		{
			if (!m_PhysicBody)
				return;

			m_StartPos = m_TargetPos;
			m_CurrentPos = m_TargetPos;
			if (m_PhysicBody->GetPosition() == m_TargetPos)
				return;

			m_PhysicBody->SetPosition(m_PhysicsSystem, m_TargetPos);
		}

		void RigidBody::SetTargetPosition(const Vector3& _position)
		{
			m_TargetPos = _position; 
		}

		Vector3 RigidBody::GetPosition()
		{
			if (!m_PhysicBody)
				return Vector3::s_Zero;

			m_Position = m_PhysicBody->GetPosition();
			return m_Position;
		}

		void RigidBody::SetRotation(const Quaternion& _rotation)
		{
			if (!m_PhysicBody)
				return;

			if (m_PhysicBody->GetRotation() == _rotation)
				return;

			m_PhysicBody->SetRotation(m_PhysicsSystem, _rotation);
		}

		Quaternion RigidBody::GetRotation()
		{
			if (!m_PhysicBody)
				return Quaternion::s_Identity;

			m_Rotation = m_PhysicBody->GetRotation();
			return m_Rotation;
		}

		Vector3 RigidBody::GetVelocity()
		{
			if (!m_PhysicBody)
				return Vector3::s_Zero;

			m_Velocity = m_PhysicBody->GetVelocity();
			return m_Velocity;
		}

		void RigidBody::SetVelocity(Vector3 _velocity)
		{
			if (!m_PhysicBody)
				return;

			if (m_MotionType == MotionType::IE_Static)
				return;
			m_PhysicBody->SetVelocity(_velocity);
		}

		void RigidBody::SetAngularVelocity(Vector3 _velocity)
		{
			if (!m_PhysicBody)
				return;

			if (m_MotionType == MotionType::IE_Static)
				return;

			m_PhysicBody->SetAngularVelocity(_velocity);
		}

		void RigidBody::AddForce(Vector3 _force)
		{
			if (!m_PhysicBody)
				return;

			if (m_MotionType != MotionType::IE_Dynamic)
				return;
			m_PhysicBody->AddForce(_force);
		}

		void RigidBody::ApplyVelocity()
		{
			if (!m_PhysicBody)
				return;

			if (m_MotionType == MotionType::IE_Static)
				return;
			m_PhysicBody->ApplyVelocity(m_PhysicsSystem);
		}

		void RigidBody::ApplyAddForce()
		{
			if (!m_PhysicBody)
				return;

			if (m_MotionType != MotionType::IE_Dynamic)
				return;

			m_PhysicBody->ApplyAddForce(m_PhysicsSystem);
		}

		void RigidBody::InterpolatePosition(float _timeLerp, ITransform* _transform)
		{
			if (!m_PhysicBody)
				return;

			if (m_StartPos == m_TargetPos && m_StartPos != m_CurrentPos)
				return;

			Collider* col = m_PhysicBody->GetCollider();

			Vector3 pos = _transform->GetGlobalPosition();
			Vector3 offsetPos = col->GetPhysicPositionWithOffSet(_transform);

			m_CurrentPos = m_StartPos + (m_TargetPos - m_StartPos) * _timeLerp;
			_transform->SetGlobalPosition(m_CurrentPos - (offsetPos - pos));
		}

		void RigidBody::SetMotionType(MotionType _motionType)
		{
			if (!m_PhysicBody)
				return;

			if (m_MotionType == _motionType)
				return;

			if (_motionType == MotionType::IE_Static)
				SetMass(m_Mass);

			m_MotionType = _motionType;

			if(p_Enable)
				m_PhysicBody->SetMotionType(m_PhysicsSystem, _motionType);
		}
	
		void RigidBody::LinkCollider(Collider* _collider)
		{
			if (_collider)
			{
				m_PhysicBody = m_PhysicsSystem->CreateBody();
				m_PhysicBody->Init(m_PhysicsSystem, _collider, p_IrisObject->GetTransform(), m_MotionType, m_Layer);
				SetIsTrigger(m_IsTrigger);
				SetFriction(m_Friction);

				SetRotationConstraint(m_Constraint, false);
			}
			else if (!_collider && m_PhysicBody->GetCollider())
			{
				m_PhysicBody->SetCollider(nullptr);
			}

		}

		Collider* RigidBody::GetCollider()
		{
			if (!m_PhysicBody)
				return nullptr;
			return m_PhysicBody->GetCollider();
		}

		void RigidBody::UpdateColliderShape()
		{
			if (!m_ToUpdateShape)
			{
				m_ToUpdateShape = true;
				m_PhysicsSystem->AddShapeToModidy(this);
			}
		}
		
		void RigidBody::SetIsTrigger(bool _isTrigger)
		{
			if (m_IsTrigger == _isTrigger)
				return;

			m_IsTrigger = _isTrigger;

			if (!m_PhysicBody)
				return;
			m_PhysicBody->SetIsTrigger(_isTrigger);
		}

		void RigidBody::SetFriction(float _friction)
		{
			if (m_Friction == _friction)
				return;

			m_Friction = _friction;

			if (!m_PhysicBody)
				return;
			m_PhysicBody->SetFriction(_friction);
		}

		void RigidBody::SetMass(float _mass)
		{
			if (_mass == m_Mass)
				return;
			if (_mass == 0.f)
				_mass = EPSILON;
			m_Mass = _mass;

			if (m_MotionType == MotionType::IE_Static || !m_PhysicBody)
				return;
			m_PhysicBody->SetMass(_mass);
		}

		void RigidBody::SetRotationConstraint(const Constraint& _constraint, bool _checkSame)
		{
			if (_checkSame && m_Constraint.GetRotationConstraint() == _constraint.GetRotationConstraint())
				return;

			m_Constraint.SetRotationConstraint(_constraint.GetRotationConstraint());

			if (!m_PhysicBody)
				return;

			m_PhysicBody->SetRotationConstraint(m_PhysicsSystem, _constraint);
		}

		void RigidBody::RemoveConstraint()
		{
			m_PhysicBody->RemoveConstraint(m_PhysicsSystem);
		}

		#pragma region Events

		void RigidBody::OnCollisionEnter(const ContactInfo& _contactInfo)
		{
			ScriptComponent* script = p_IrisObject->GetComponent<ScriptComponent>();
			if (script)
				script->OnCollisionEnter(_contactInfo.Other);
		}

		void RigidBody::OnCollisionStay(const ContactInfo& _contactInfo)
		{
			if(m_MotionType != MotionType::IE_Static)
				m_PhysicBody->ActivateBody();

			ScriptComponent* script = p_IrisObject->GetComponent<ScriptComponent>();
			if (script)
				script->OnCollisionStay(_contactInfo.Other);
		}

		void RigidBody::OnCollisionExit(const ContactInfo& _contactInfo)
		{
			ScriptComponent* script = p_IrisObject->GetComponent<ScriptComponent>();
			if (script)
				script->OnCollisionExit(_contactInfo.Other);
		}

		void RigidBody::OnTriggerEnter(const ContactInfo& _contactInfo)
		{
			ScriptComponent* script = p_IrisObject->GetComponent<ScriptComponent>();
			if (script)
				script->OnTriggerEnter(_contactInfo.Other);
		}

		void RigidBody::OnTriggerStay(const ContactInfo& _contactInfo)
		{
			if (m_MotionType != MotionType::IE_Static)
				m_PhysicBody->ActivateBody();

			ScriptComponent* script = p_IrisObject->GetComponent<ScriptComponent>();
			if (script)
				script->OnTriggerStay(_contactInfo.Other);
		}

		void RigidBody::OnTriggerExit(const ContactInfo& _contactInfo)
		{
			ScriptComponent* script = p_IrisObject->GetComponent<ScriptComponent>();
			if (script)
				script->OnTriggerExit(_contactInfo.Other);
		}

		void RigidBody::OnActivated()
		{
		}

		void RigidBody::OnDeactivated()
		{
		}
		#pragma endregion

	}
}
