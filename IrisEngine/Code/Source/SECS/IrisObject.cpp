#include "SECS/IrisObject.h"
#include "SECS/Scene.h"
#include "SECS/Components/Transform.h"
#include "SECS/Components/IComponent.h"
#include "SECS/Components/ModelRenderer.h"
#include "SECS/Components/RigidBody.h"
#include "SECS/Components/Collider.h"

#include "Resources/Metadata.h"

#include "SECS/Components/ComponentTable.h"

namespace Iris
{
	namespace Engine
	{
		IrisObject::IrisObject(Scene* _scene)
		{
			m_Components.fill(nullptr);
			m_SceneRef = _scene;
			
			m_UniqueID = _scene->GenIrisObjectID();

			ITransform* t = m_SceneRef->AddComponent<ITransform>(this);
			if (m_SceneRef->GetRoot())
				t->SetParent(m_SceneRef->GetRoot()->GetTransform());

		}

		IrisObject::IrisObject(Scene* _scene, IMetadata* _sceneData, std::string _id, InitEditorContext& _initializer)
		{
			m_Components.fill(nullptr);

			m_SceneRef = _scene;

			std::vector<std::string> mainPath = { "Scene Data", "IrisObject" , _id , "" };

			m_UniqueID = std::stoi(_id);

			mainPath[3] = "Name";
			m_Name = _sceneData->GetData(mainPath);

			mainPath[3] = "Active";
			m_Active = _sceneData->GetData(mainPath);

			mainPath[3] = "ShowInHierarchy";
			m_ShowInHierarchy = _sceneData->GetData(mainPath);

			ITransform* t = m_SceneRef->AddComponent<ITransform>(this);
			if (m_SceneRef->GetRoot())
				t->SetParent(m_SceneRef->GetRoot()->GetTransform());

			std::vector<std::string> transformPath = { "Scene Data", "Components", std::to_string(static_cast<int>(ITransform::GetID())), ITransform::GetName(), _id };
			MetaDataType transformData = _sceneData->GetData(transformPath);

			std::vector<float> pos = transformData["Global Position"];
			t->SetPosition(Vector3(pos[0], pos[1], pos[2]));

			std::vector<float> scale = transformData["Global Scale"];
			t->SetScale(Vector3(scale[0], scale[1], scale[2]));

			mainPath[3] = "Components";
			std::vector<std::string> compsId = _sceneData->GetData(mainPath);
			for (std::string compId : compsId)
			{
				if (compId == std::to_string(static_cast<int>(ComponentID::IE_TRANSFORM)))
					continue;

				IComponent* newComp = m_SceneRef->AddComponent(this, static_cast<ComponentID>(std::stoi(compId)));
				newComp->Deserialization(_sceneData, _initializer);
			}
		}

		IrisObject::~IrisObject()
		{
			size_t size = m_Components.size();
			for (size_t i = 0; i < size;++i)
				delete m_Components[i];
		}

		void IrisObject::Serialize(IMetadata* _metadata)
		{
			_metadata->Write("Name", m_Name);

			_metadata->Write("Active", m_Active);

			_metadata->Write("ShowInHierarchy", m_ShowInHierarchy);

			std::vector<std::string> compoID;
			size_t size = m_Components.size();
			for (size_t i = 0; i < size; ++i)
			{
				if(m_Components[i])
					compoID.push_back(std::to_string(static_cast<int>(m_Components[i]->GetType())));
			}

			_metadata->Write("Components", compoID);
		}

		void IrisObject::SetActive(bool _activate)
		{
			if (m_Active == _activate)
				return;

			m_Active = _activate;

			size_t size = m_Components.size();
			for (size_t i = 0; i < size;++i)
			{
				if(m_Components[i])
					m_Components[i]->SetEnableComponent(m_Active);
			}
		}

		void IrisObject::Destroy()
		{
			m_Destroyed = true;

			size_t size = m_Components.size();
			for (size_t i = 0; i < size;++i)
			{
				if(m_Components[i])
					m_Components[i]->RemoveComponent();
			}

			m_SceneRef->RemoveIrisObject(this);
		}

		void IrisObject::Duplicate(InitEditorContext* _initializer, ITransform* _parent)
		{
			ITransform* t = GetTransform();
			IrisObject* newObject = m_SceneRef->Instantiate(t->GetGlobalPosition(), t->GetGlobalRotation(), t->GetGlobalScale(), _parent, m_Name + " (Copy)");
			
			newObject->GetTransform()->SetRotation(t->GetRotation());

			//0 is transform
			size_t size = m_Components.size();
			for (size_t i = 1; i < size; ++i)
			{
				if (m_Components[i])
				{
					IComponent* dupliComp = m_SceneRef->AddComponent(newObject, m_Components[i]->GetType());
					dupliComp->Duplicate(GetComponent(m_Components[i]->GetType()), _initializer);
				}
			}


			std::vector<ComponentHandle<ITransform>> children = t->GetChildren();
			size = children.size();

			t = newObject->GetTransform();
			for (size_t i = 0; i < size; ++i)
			{
				children[i].GetIrisObject()->Duplicate(_initializer, t);
			}
		}

		ITransform* IrisObject::GetTransform()
		{
			return m_SceneRef->GetComponent<ITransform>(this);
		}

		IComponent* IrisObject::GetComponent(ComponentID _componentId)
		{
			return m_SceneRef->GetComponent(this,_componentId);
		}

		void IrisObject::DestroyChildObject()
		{
			ITransform* t = GetTransform();
			std::vector<ComponentHandle<ITransform>> children = t->GetChildren();
			size_t size = children.size();
			for (size_t i = 0; i < size;++i)
				children[i].GetIrisObject()->Destroy();

		}
	}
}