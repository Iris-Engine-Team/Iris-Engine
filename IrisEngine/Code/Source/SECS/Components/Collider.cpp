#include "SECS/Components/Collider.h"
#include "SECS/Components/RigidBody.h"
#include "SECS/Systems/PhysicsSystem.h"
#include "Resources/ResourceManager.h"
#include "Core/Graphics/Context/ResourceContext.h"

#include "Resources/Metadata.h"

namespace Iris
{
	namespace Engine
	{
		ShapeParam::ShapeParam(int _shapeType, float _sphereRadius, Vector3 _cubeSize, float _capsuleRadius, float _capsuleHeight)
			: m_ShapeType(static_cast<PhysicShape>(_shapeType)),
			m_SphereRadius(_sphereRadius), m_CubeSize(_cubeSize),
			m_CapsuleRadius(_capsuleRadius), m_CapsuleHeight(_capsuleHeight)
		{}

		void Collider::Serialize(IMetadata* _metadata)
		{
			_metadata->Write("Enable", p_Enable);

			std::vector<float> vec3 = { m_PosOffSet[0],m_PosOffSet[1] ,m_PosOffSet[2] };
			_metadata->Write("Offset", vec3);

			m_Shape->Serialize(_metadata);
		}

		void Collider::Deserialization(IMetadata* _metadata, InitEditorContext& _initializer)
		{

			std::vector<std::string> path = { "Scene Data", "Components", std::to_string(static_cast<uint16_t>(GetID())), GetName(), std::to_string(p_IrisObject->GetID()), "" };

			m_PhysicsSystem = _initializer.InitScene->GetPhysicsSystem();

			path[5] = "Type";
			m_Shape = m_PhysicsSystem->CreateShape(_metadata->GetData(path));

			path[5] = "Offset";
			std::vector<float> vec3 = _metadata->GetData(path);
			m_PosOffSet = Vector3(vec3[0], vec3[1], vec3[2]);

			path[5] = "Enable";
			p_Enable = _metadata->GetData(path);

			m_Shape->Deserialization(_metadata, path);

			m_Shape->SetColliderRef(this);
			
			LinkRigidBody(p_IrisObject->GetComponent<RigidBody>());
			NotifyColliderChanges();
		}

		void Collider::Init(IPhysicsSystem* _physicsSystem, PhysicShape _shapeType, const ShapeParam& _params)
		{
			if (m_Shape)
				delete m_Shape;

			m_PhysicsSystem = _physicsSystem;
			m_Shape = m_PhysicsSystem->CreateShape(_shapeType);
			m_Shape->SetColliderRef(this);
			m_Shape->Init(_params);
			LinkRigidBody(p_IrisObject->GetComponent<RigidBody>());
			NotifyColliderChanges();
		}

		void Collider::Init(IPhysicsSystem* _physicsSystem)
		{
			Init(_physicsSystem, PhysicShape::IE_SHAPE_BOX, ShapeParam());
		}

		void Collider::Init(InitEditorContext& _initializer)
		{
			Init(_initializer.InitScene->GetPhysicsSystem());
		}

		void Collider::Copy(IComponent* _componentToCopy)
		{
			Collider* col = dynamic_cast<Collider*>(_componentToCopy);
			if (col)
			{
				IComponent::Copy(_componentToCopy);
				m_Shape = col->m_Shape;
				col->m_Shape = nullptr;

				m_PhysicsSystem = col->m_PhysicsSystem;
				m_RigidBody = col->m_RigidBody;
				m_PosOffSet = col->m_PosOffSet;
			}
			else
			{
				DEBUG_LOG("cant copy this component ");
			}
		}

		void Collider::Duplicate(IComponent* _componentToDuplicate, InitEditorContext* _initializer)
		{
			Collider* col = dynamic_cast<Collider*>(_componentToDuplicate);
			if (col)
			{
				IComponent::Duplicate(_componentToDuplicate, _initializer);

				Init(col->m_PhysicsSystem, col->GetShape()->GetShapeType(), *col->GetShapeInfo());
				m_PosOffSet = col->m_PosOffSet;
			}
		}

		void Collider::Destroy()
		{
			if (m_Shape)
			{
				delete m_Shape;

				if (!m_RigidBody)
					return;

				RigidBody* rb = m_RigidBody->UseComponent();
				if (rb)
					rb->PermanentlyRemoveFromPhysicsSystem();

			}
		}
		
		void Collider::SetShapeInfo(PhysicShape _colliderType, const ShapeParam& _params)
		{
			if (m_Shape)
			{
				if (m_Shape->GetShapeType() == _colliderType)
					m_Shape->SetShapeInfo(_params);
				else
					Init(m_PhysicsSystem,_colliderType, _params);
			}
		}

		void Collider::SetShapeInfo(const ShapeParam& _params)
		{
			m_Shape->SetShapeInfo(_params);
		}

		ShapeParam* Collider::GetShapeInfo()
		{
			m_Params = m_Shape->GetShapeInfo();
			return &m_Params;
		}

		std::vector<Mesh*> Collider::GetMesh()
		{
			return m_Shape->GetMesh();
		}

		std::vector<Matrix4> Collider::GetPhysicTRS()
		{
			return m_Shape->GetPhysicTRS();
		}


		RigidBody* Collider::GetRigidBody()
		{
			if (m_RigidBody)
				return m_RigidBody->UseComponent();
			else
				return nullptr;
		}

		void Collider::LinkRigidBody(RigidBody* _rigidBody)
		{
			if (_rigidBody)
			{
				if(!m_RigidBody)
					m_RigidBody = new ComponentHandle<RigidBody>(_rigidBody);
				if (!_rigidBody->GetCollider())
					_rigidBody->LinkCollider(this);
			}
			else if (!_rigidBody && m_RigidBody)
			{
				delete m_RigidBody;
				m_RigidBody = nullptr;
			}

		}

		void Collider::NotifyColliderChanges()
		{
			if (m_RigidBody)
			{
				RigidBody* rb = m_RigidBody->UseComponent();
				if (rb)
					rb->UpdateColliderShape();
			}
		}

		Matrix4 Collider::GetTRSOffSet()
		{
			return Matrix4::Translation(m_PosOffSet);
		}

		Vector3 Collider::GetPhysicPositionWithOffSet(ITransform* _transform)
		{
			Vector3 pos = Vector3::s_Zero;
			if (m_PosOffSet != Vector3::s_Zero)
			{
				Matrix4 globalCol = _transform->GetGlobalTRS() * GetTRSOffSet();
				Vector4 gPos = globalCol.GetColumn(3);

				pos = Vector3(gPos[0], gPos[1], gPos[2]);
			}
			else
			{
				pos = _transform->GetGlobalPosition();
			}
			return pos;
		}

		std::vector<Matrix4> IShape::GetPhysicTRS()
		{
			std::vector<Matrix4> modelsMatrix;

			RigidBody* rb = p_Collider->UseComponent()->GetRigidBody();
			Vector3 pos = Vector3::s_Zero;
			Quaternion rota = Quaternion::s_Identity;

			if (rb)
			{
				pos = rb->GetPosition();
				rota = rb->GetRotation();
			}
			else
			{
				ITransform* t = p_Collider->GetIrisObject()->GetTransform();
				//apply offset
				pos = p_Collider->UseComponent()->GetPhysicPositionWithOffSet(t);
				rota = t->GetGlobalRotation();
			}

			modelsMatrix.push_back(Matrix4::TRS(pos, rota, GetScale()));
			return modelsMatrix;
		}

		void IShape::SetColliderRef(Collider* _collider)
		{
			p_Collider = new ComponentHandle<Collider>(_collider);
		}

		void BoxCollider::Serialize(IMetadata* _metadata)
		{
			_metadata->Write("Type", p_ShapeType);

			std::vector<float> vec3 = { p_Size[0],p_Size[1] ,p_Size[2] };
			_metadata->Write("Box Size", vec3);
		}

		void BoxCollider::Deserialization(IMetadata* _metadata, std::vector<std::string>& _currentPath)
		{
			_currentPath[_currentPath.size() - 1] = "Box Size";
			std::vector<float> vec3 = _metadata->GetData(_currentPath);

			p_Size = Vector3(vec3[0], vec3[1], vec3[2]);

			ShapeParam param;
			param.InitBoxInfo(p_Size);
			Init(param);
		}

		void BoxCollider::SetShapeInfo(const ShapeParam& _params)
		{
			if (_params.GetBoxSize() == p_Size)
				return;

			p_Size = _params.GetBoxSize();
			if (p_Size[0] == 0.f)
				p_Size.SetX(EPSILON);

			if (p_Size[1] == 0.f)
				p_Size.SetY(EPSILON);

			if (p_Size[2] == 0.f)
				p_Size.SetZ(EPSILON);

			p_Collider->UseComponent()->NotifyColliderChanges();
		}


		ShapeParam BoxCollider::GetShapeInfo()
		{
			ShapeParam params;
			params.InitBoxInfo(p_Size);
			return params;
		}

		std::vector<Mesh*> BoxCollider::GetMesh()
		{
			std::vector<Mesh*> meshes;
			meshes.push_back(ResourceManager::GetInstance()->Get<Mesh>("DefaultCube.obj"));
			return meshes;
		}

		void SphereCollider::Serialize(IMetadata* _metadata)
		{
			_metadata->Write("Type", p_ShapeType);

			_metadata->Write("Sphere Radius", p_Radius);
		}

		void SphereCollider::Deserialization(IMetadata* _metadata, std::vector<std::string>& _currentPath)
		{
			_currentPath[_currentPath.size() - 1] = "Sphere Radius";
			p_Radius = _metadata->GetData(_currentPath);

			ShapeParam param;
			param.InitSphereInfo(p_Radius);
			Init(param);
		}

		void SphereCollider::SetShapeInfo(const ShapeParam& _params)
		{
			if (_params.GetSphereRadius() == p_Radius)
				return;
			p_Radius = _params.GetSphereRadius();
			if (p_Radius <= 0.f)
				p_Radius = EPSILON;
			p_Collider->UseComponent()->NotifyColliderChanges();
		}

		ShapeParam SphereCollider::GetShapeInfo()
		{
			ShapeParam params;
			params.InitSphereInfo(p_Radius);
			return params;
		}

		std::vector<Mesh*> SphereCollider::GetMesh()
		{
			std::vector<Mesh*> meshes;
			meshes.push_back(ResourceManager::GetInstance()->Get<Mesh>("SimpleSphere.obj"));
			return meshes;
		}

		void CapsuleCollider::Serialize(IMetadata* _metadata)
		{
			_metadata->Write("Type", p_ShapeType);

			_metadata->Write("Capsule Radius", p_Radius);

			_metadata->Write("Capsule Height", p_Height);
		}

		void CapsuleCollider::Deserialization(IMetadata* _metadata, std::vector<std::string>& _currentPath)
		{
			_currentPath[_currentPath.size() - 1] = "Capsule Radius";
			p_Radius = _metadata->GetData(_currentPath);

			_currentPath[_currentPath.size() - 1] = "Capsule Height";
			p_Height = _metadata->GetData(_currentPath);

			ShapeParam param;
			param.InitCapsuleInfo(p_Radius, p_Height);
			Init(param);
		}

		void CapsuleCollider::SetShapeInfo(const ShapeParam& _params)
		{
			bool modified = false;

			if (_params.GetCapsuleRadius() != p_Radius)
			{
				p_Radius = _params.GetCapsuleRadius();
				if (p_Radius <= 0.f)
					p_Radius = EPSILON;

				modified = true;
			}

			if (_params.GetCapsuleHeight() != p_Height)
			{
				p_Height = _params.GetCapsuleHeight();
				if (p_Height <= 0.f)
					p_Height = EPSILON;

				modified = true;
			}

			if(modified)
				p_Collider->UseComponent()->NotifyColliderChanges();
		}

		ShapeParam CapsuleCollider::GetShapeInfo()
		{
			ShapeParam params;
			params.InitCapsuleInfo(p_Radius, p_Height);
			return params;
		}

		std::vector<Mesh*> CapsuleCollider::GetMesh()
		{
			std::vector<Mesh*> meshes;
			//top sphere
			meshes.push_back(ResourceManager::GetInstance()->Get<Mesh>("SimpleSphere.obj"));
			//mid cynlinder
			meshes.push_back(ResourceManager::GetInstance()->Get<Mesh>("DefaultCylinder.obj"));
			//bottom sphere
			meshes.push_back(ResourceManager::GetInstance()->Get<Mesh>("SimpleSphere.obj"));
			return meshes;
		}

		std::vector<Matrix4> CapsuleCollider::GetPhysicTRS()
		{
			std::vector<Matrix4> modelsMatrix;

			RigidBody* rb = p_Collider->UseComponent()->GetRigidBody();
			Vector3 pos = Vector3::s_Zero;
			Quaternion rota = Quaternion::s_Identity;

			ITransform* t = p_Collider->GetIrisObject()->GetTransform();
			if (rb)
			{
				rota = rb->GetRotation();
				pos = rb->GetPosition();
			}
			else
			{
				//apply offset
				rota = t->GetGlobalRotation();
				pos = p_Collider->UseComponent()->GetPhysicPositionWithOffSet(t);
			}

			Vector3 upGlobal = (t->GetUp() * p_Height);

			modelsMatrix.push_back(Matrix4::TRS(pos + upGlobal, rota, Vector3(p_Radius)));
			modelsMatrix.push_back(Matrix4::TRS(pos, rota, Vector3(p_Radius, p_Height, p_Radius)));
			modelsMatrix.push_back(Matrix4::TRS(pos - upGlobal, rota, Vector3(p_Radius)));
			return modelsMatrix;
		}
	}
}