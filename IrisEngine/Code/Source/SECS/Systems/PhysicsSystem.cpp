#include "SECS/Systems/PhysicsSystem.h"
#include "SECS/Scene.h"

#include "Core/Physics/ContactListener.h"
#include "Core/Physics/ActivationListener.h"

#include "SECS/Components/ComponentTable.h"

namespace Iris
{
	namespace Engine
	{
		IPhysicsSystem::IPhysicsSystem(Scene* _scene)
		{
			SetScene(_scene);
		}

		void IPhysicsSystem::SetScene(Scene* _scene)
		{
			if (!_scene)
				return;

			p_SceneRef = _scene;

			if (p_SceneRef->ContainTable<RigidBody>())
				p_RigidBodies = p_SceneRef->GetComponentTable<RigidBody>();
			else
				p_RigidBodies = p_SceneRef->AddTable<RigidBody>();

			if (p_SceneRef->ContainTable<Collider>())
				p_Colliders = p_SceneRef->GetComponentTable<Collider>();
			else
				p_Colliders = p_SceneRef->AddTable<Collider>();
		}

		void IPhysicsSystem::Destroy()
		{
			if(!p_ShapeToUpdate.empty())
				p_ShapeToUpdate.clear();

			delete m_ActivationListener;
			delete p_ContactListener;
		}
		
		void IPhysicsSystem::Clear()
		{
			p_ShapeToUpdate.clear();
			p_SceneRef = nullptr;
		}

		void IPhysicsSystem::Interpolate(float _timerInterpolate)
		{
			ComponentTable<ITransform>* trsTable = p_SceneRef->GetComponentTable<ITransform>();
			std::vector<RigidBody>& rbs = p_RigidBodies->GetAllComponents();
			size_t size = p_RigidBodies->ComponentCount();
			for (size_t i = 0; i < size; ++i)
			{
				if (rbs[i].GetMotionType() == MotionType::IE_Static || !rbs[i].IsInterpolatePosition())
					continue;

				rbs[i].InterpolatePosition(_timerInterpolate, dynamic_cast<ITransform*>(trsTable->GetComponent(rbs[i].GetIrisObject())));
			}
		}

		void IPhysicsSystem::UpdateModifiedShape()
		{
			size_t size = p_ShapeToUpdate.size();
			if (size == 0)
				return;

			for (size_t i = 0; i < size; i++)
			{
				RigidBody* rb = p_ShapeToUpdate[i].UseComponent();
				if (rb)
				{
					rb->GetPhysicalBody()->ChangeShape(this);
					rb->ShapeUpdated();
				}
			}
			p_ShapeToUpdate.clear();
		}

		void IPhysicsSystem::AddShapeToModidy(RigidBody* _rigidbody)
		{
			p_ShapeToUpdate.push_back(ComponentHandle<RigidBody>(_rigidbody));
		}
	}
}