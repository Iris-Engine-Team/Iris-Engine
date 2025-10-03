#include "SECS/Components/Transform.h"
#include "SECS/IrisObject.h"
#include "Core/Tools/Log.h"
#include "Resources/Metadata.h"

namespace Iris
{
	namespace Engine
	{

		ITransform::ITransform()
		{
			SetPosition(Vector3::s_Zero);
			SetRotation(Quaternion::s_Identity);
			SetScale(Vector3::s_One);
		}
		
		ITransform::ITransform(const ITransform& _transform, IrisObject* _irisObject)
		{
			p_IrisObject = _irisObject;

			m_Position = _transform.m_Position;
			m_EulerAngles = _transform.m_EulerAngles;
			m_Rotation = _transform.m_Rotation;
			m_Scale = _transform.m_Scale;

			m_RecalculateLocal = true;
			SetParent(_transform.m_Parent->UseComponent());
		}

		ITransform::~ITransform()
		{
			p_IrisObject = nullptr;
		}

		void ITransform::Serialize(IMetadata* _metadata)
		{
			_metadata->Write("Enable", p_Enable);

			Vector3 gPos = GetGlobalPosition();
			std::vector<float> vec3 = { gPos[0],gPos[1] ,gPos[2] };
			_metadata->Write("Global Position", vec3);

			Quaternion gRota = GetGlobalRotation();
			std::vector<float> rotation = { gRota[0],gRota[1] ,gRota[2] ,gRota[3] };
			_metadata->Write("Global Rotation", rotation);

			Quaternion gEuler = GetRotation();
			std::vector<float> euler = { gEuler[0],gEuler[1] ,gEuler[2], gEuler[3]};
			_metadata->Write("Local Euler", euler);

			Vector3 gScale = GetGlobalScale();
			vec3 = { gScale[0], gScale[1], gScale[2] };
			_metadata->Write("Global Scale", vec3);

			//-1 if root
			int parentId = -1;
			if (m_Parent)
				parentId = static_cast<int>(m_Parent->GetIrisObject()->GetID());

			_metadata->Write("Parent", parentId);
		}

		void ITransform::Deserialization(IMetadata* _metadata, InitEditorContext& _initializer)
		{
			if (p_IrisObject->GetID() == 0)
				return;

			//the set of position and scaling has been done at the creation of the iris object
			std::vector<std::string> path = { "Scene Data", "Components", std::to_string(static_cast<int>(GetID())), GetName(), std::to_string(p_IrisObject->GetID()), "Parent" };
			int parentId = _metadata->GetData(path);
			if (parentId != 0)
				SetParent(_initializer.InitScene->GetObjectByID(parentId)->GetTransform());
			else
				SetParent(_initializer.InitScene->GetRoot()->GetTransform());

			path[5] = "Local Euler";
			std::vector<float> euler = _metadata->GetData(path);
			SetRotation(Quaternion(euler[0], euler[1], euler[2], euler[3]));
			
			path[5] = "Enable";
			p_Enable = _metadata->GetData(path);
		}

		void ITransform::Init(const Vector3& _position, ITransform* _parent)
		{
			SetPosition(_position);

			SetParent(_parent);
		}

		void ITransform::Init(const Vector3& _position, const Quaternion& _rotation, const Vector3& _scale, ITransform* _parent)
		{
			SetPosition(_position);
			SetRotation(_rotation);
			SetScale(_scale);

			SetParent(_parent);
		}

		void ITransform::Init(const Vector3& _position, const Vector3& _rotation, const Vector3& _scale, ITransform* _parent)
		{
			SetPosition(_position);
			SetEuler(_rotation);
			SetScale(_scale);

			SetParent(_parent);
		}

		void ITransform::Copy(IComponent* _componentToCopy)
		{
			ITransform* t = dynamic_cast<ITransform*>(_componentToCopy);
			if (t)
			{
				IComponent::Copy(_componentToCopy);
				ComponentHandle<ITransform>* parent = t->m_Parent;
				if (parent)
				{
					parent->UseComponent()->RemoveChild(t);

					SetParent(parent->UseComponent());

					delete t->m_Parent;
					t->m_Parent = nullptr;
				}
				else
					SetParent(nullptr);

				SetPosition(t->GetPosition());
				SetRotation(t->GetRotation());
				SetScale(t->GetScale());

			}
			else
				DEBUG_LOG("cant copy this component ");
		}

		void ITransform::Destroy()
		{
			if (m_Parent)
				SetParent(nullptr);
		}

		void ITransform::SetEnable(bool _enable)
		{
			if (p_Enable == _enable)
				return;

			IComponent::SetEnable(_enable);
			
			p_IrisObject->SetActive(_enable);

			size_t size = m_Children.size();
			for (size_t i = 0; i < size; ++i)
				m_Children[i].GetIrisObject()->SetActive(_enable);
		}

		Vector3 ITransform::SetPosition(const Vector3& _position)
		{
			if (m_Position != _position)
			{
				m_Position = _position;
				m_RecalculateLocal = true;
			}

			return m_Position;
		}

		Vector3 ITransform::SetGlobalPosition(const Vector3& _position)
		{
			Vector3 posToSet = _position;
			if (m_Parent)
			{
				ITransform* t = m_Parent->UseComponent();
				posToSet = _position - t->GetGlobalPosition();
				posToSet /= t->GetGlobalScale();
				posToSet = Quaternion::Rotate(posToSet, t->GetGlobalRotation().Inverse());
			}

			SetPosition(posToSet);
			return m_Position;
		}

		Vector3 ITransform::GetGlobalPosition()
		{
			if (m_RecalculateLocal || m_RecalculateGlobal)
				CalculateGlobalTRS();

			Vector4 gPos = m_GlobalMatrix.GetColumn(3);
			m_GlobalPosition = Vector3(gPos[0], gPos[1], gPos[2]);
			return m_GlobalPosition;
		}

		Vector3 ITransform::SetScale(const Vector3& _scale)
		{
			if (m_Scale != _scale)
			{
				m_Scale = _scale;
				m_RecalculateLocal = true;
			}
			return m_Scale;
		}

		Vector3 ITransform::SetGlobalScale(const Vector3& _scale)
		{
			if (m_Parent)
				m_Scale = _scale / m_Parent->UseComponent()->GetGlobalScale();
			else
				m_Scale = _scale;
			m_RecalculateLocal = true;
			return m_Scale;
		}

		Vector3 ITransform::GetGlobalScale()
		{
			Vector3 scale = m_Scale;
			if (m_Parent)
				scale *= m_Parent->UseComponent()->GetGlobalScale();

			m_GlobalScale = scale;
			return m_GlobalScale;
		}

		Vector3 ITransform::SetEuler(const Vector3& _rotation)
		{
			if (m_EulerAngles != _rotation)
			{
				m_EulerAngles = _rotation;
				m_Rotation = Quaternion(m_EulerAngles.GetX() * Deg2Rad, m_EulerAngles.GetY() * Deg2Rad, m_EulerAngles.GetZ() * Deg2Rad);
				m_RecalculateLocal = true;
			}
			return m_EulerAngles;
		}

		Quaternion ITransform::SetRotation(const Quaternion& _rotation)
		{
			if (m_Rotation != _rotation)
			{
				m_Rotation = _rotation;
				m_EulerAngles = m_Rotation.ToEulerAngle() * Rad2Deg;
				m_RecalculateLocal = true;
			}
			return m_Rotation;
		}

		Vector3 ITransform::SetGlobalEuler(const Vector3& _rotation)
		{
			SetGlobalRotation(Quaternion(_rotation.GetX() * Deg2Rad
				, _rotation.GetY() * Deg2Rad
				, _rotation.GetZ() * Deg2Rad));
			return m_EulerAngles;
		}

		Quaternion ITransform::SetGlobalRotation(const Quaternion& _rotation)
		{
			Quaternion parentGlobal = Quaternion(1.f, 0.f, 0.f, 0.f);
			if (m_Parent)
				parentGlobal = m_Parent->UseComponent()->GetGlobalRotation();
			parentGlobal = parentGlobal.Inverse();

			SetRotation(parentGlobal * _rotation);
			return m_Rotation;
		}

		Vector3 ITransform::GetGlobalEuler()
		{
			m_GlobalEulerAngles = GetGlobalRotation().ToEulerAngle() * Rad2Deg;
			return m_GlobalEulerAngles;
		}

		Quaternion ITransform::GetGlobalRotation()
		{
			Quaternion rota = m_Rotation;
			if (m_Parent)
				rota = m_Parent->UseComponent()->GetGlobalRotation() * rota;

			m_GlobalRotation = rota;
			return m_GlobalRotation;
		}

		Matrix4 ITransform::GetLocalTRS()
		{
			CalculateLocalTRS();
			return m_LocalMatrix;
		}

		Matrix4 ITransform::GetGlobalTRS()
		{
			CalculateGlobalTRS();
			return m_GlobalMatrix;
		}

		Vector3 ITransform::GetRight()
		{
			m_Right = Quaternion::Rotate(Vector3::s_Right, GetGlobalRotation());
			return m_Right;
		}

		Vector3 ITransform::GetUp()
		{
			m_Up = Quaternion::Rotate(Vector3::s_Up, GetGlobalRotation());
			return m_Up;
		}

		Vector3 ITransform::GetForward()
		{
			m_Forward = Quaternion::Rotate(Vector3::s_Forward, GetGlobalRotation());
			return m_Forward;
		}

		void ITransform::SetParent(ITransform* _parent)
		{
			if (!m_Parent || m_Parent->UseComponent() != _parent)
			{
				ComponentHandle<ITransform>* hT = m_Parent;
				ITransform* p = nullptr;
				if (hT)
					p = hT->UseComponent();
				if (_parent)
				{
					_parent->AddChild(this);
				}
				else
					m_Parent = nullptr;

				if (p)
				{
					p->RemoveChild(this);
					delete hT;
					hT = nullptr;
				}
			}
		}

		ITransform* ITransform::GetParent()
		{
			if (m_Parent)
				return m_Parent->UseComponent();
			else
				return nullptr;
		}

		void ITransform::AddChild(ITransform* _child)
		{
			if (!_child)
				return;

			if (_child && _child->m_Parent && _child->m_Parent->UseComponent() == this)
			{
				//already a child
				return;
			}

			m_Children.emplace_back(ComponentHandle<ITransform>(_child));
			Vector3 scaleG = _child->GetGlobalScale();
			Quaternion rotaG = _child->GetGlobalRotation();
			Vector3 posG = _child->GetGlobalPosition();

			_child->m_Parent = new ComponentHandle<ITransform>(this);
			if(!p_Enable)
				_child->SetEnable(false);

			_child->SetGlobalScale(scaleG);
			_child->SetGlobalRotation(rotaG);
			_child->SetGlobalPosition(posG);

			_child->ChildrenRecalculateGlobalTRS();
		}

		bool ITransform::RemoveChild(ITransform* _child)
		{
			size_t size = m_Children.size();
			for (size_t i = 0; i < size;++i)
			{
				ITransform* t = m_Children[i].UseComponent();
				if (t->GetIrisObject()->GetID() == _child->GetIrisObject()->GetID())
				{	
					m_Children.erase(m_Children.begin() + i);
					return true;
				}
			}
			return false;
		}

		void ITransform::CalculateLocalTRS()
		{
			if (m_RecalculateLocal)
			{
				m_LocalMatrix = Matrix4::TRS(m_Position, m_Rotation, m_Scale);
				m_RecalculateLocal = false;
				m_RecalculateGlobal = true;
				ChildrenRecalculateGlobalTRS();
			}
		}

		void ITransform::CalculateGlobalTRS()
		{
			CalculateLocalTRS();

			if (m_RecalculateGlobal)
			{
				if (m_Parent)
					m_GlobalMatrix = m_Parent->UseComponent()->GetGlobalTRS() * m_LocalMatrix;
				else
					m_GlobalMatrix = m_LocalMatrix;

				m_RecalculateGlobal = false;
			}
		}

		void ITransform::ChildrenRecalculateGlobalTRS()
		{
			size_t size = m_Children.size();
			for (size_t child = 0; child < size;++child)
				m_Children[child].UseComponent()->RecalculateOnParentChange();
			
		}

		void ITransform::RecalculateOnParentChange()
		{
			m_RecalculateGlobal = true;
			CalculateGlobalTRS();
			ChildrenRecalculateGlobalTRS();
		}

		std::string ITransform::PrintTransform()
		{
			std::ostringstream pos;
			pos << m_Position;
			std::ostringstream gPos;
			gPos << GetGlobalPosition();

			std::ostringstream rota;
			rota << m_EulerAngles;
			std::ostringstream gRota;
			gRota << GetGlobalEuler();

			std::ostringstream scale;
			scale << m_Scale;
			std::ostringstream gScale;
			gScale << GetGlobalScale();

			return "\n\tLPos: " + pos.str() + "\tGPos: " + gPos.str()
				+ "\n\tLRota: " + rota.str() + "\tGRota: " + gRota.str()
				+ "\n\tLScale: " + scale.str() + "\tGScale: " + gScale.str()+ "\n";
		}

		std::vector<ComponentHandle<ITransform>> ITransform::GetChildren()
		{
			return m_Children;
		}
	}
}