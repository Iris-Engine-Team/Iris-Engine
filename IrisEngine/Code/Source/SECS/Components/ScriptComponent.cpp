#include "SECS/Components/ScriptComponent.h"
#include "SECS/Systems/ScriptSystem.h"
#include "Resources/Metadata.h"
#include "SECS/SceneManager.h"
#include "Resources/Script.h"
#include "CSharpScripting/ScriptManager.h"
#include "Resources/ResourceManager.h"

namespace Iris
{
	namespace Engine
	{
		void Iris::Engine::ScriptComponent::Serialize(IMetadata* _metadata)
		{
			_metadata->Write("Enable", p_Enable);

			_metadata->Write("ScriptName", m_ScriptName);
		}

		void ScriptComponent::Deserialization(IMetadata* _metadata, InitEditorContext& _initializer)
		{
			std::vector<std::string> path = { "Scene Data", "Components", std::to_string(static_cast<uint16_t>(GetID())), GetName(), std::to_string(p_IrisObject->GetID()), "" };

			path[5] = "Enable";
			p_Enable = _metadata->GetData(path);
			path[5] = "ScriptName";
			m_ScriptName = _metadata->GetData(path);

			ResourceManager::GetInstance()->Create<Script>(m_ScriptName);

			m_ScriptSystem = _initializer.SceneManager->GetScriptSystem();
			m_ScriptSystem->AddToAwakeList(this);
		}

		void ScriptComponent::Init(ScriptSystem* _system, std::string _className)
		{
			m_ScriptName = _className;
			m_ScriptSystem = _system;

			m_ScriptSystem->AddToAwakeList(this);
		}

		void ScriptComponent::Init(InitEditorContext& _initializer)
		{
			Init(_initializer.SceneManager->GetScriptSystem());
		}

		void ScriptComponent::Copy(IComponent* _componentToCopy)
		{
			ScriptComponent* s = dynamic_cast<ScriptComponent*>(_componentToCopy);
			if (s)
			{
				IComponent::Copy(_componentToCopy);

				m_ScriptName = s->m_ScriptName;
				m_ScriptInstance = nullptr;
				m_ScriptSystem = s->m_ScriptSystem;

				if (m_ScriptSystem->GetInstanciateScript())
					SetInstance(ResourceManager::GetInstance()->GetScriptManager()->InitCSharpScript(p_IrisObject->GetHandle<ComponentHandle<ScriptComponent>, ScriptComponent>()));
			}
			else
			{
				DEBUG_LOG("cant copy this component ");
			}
		}

		void ScriptComponent::Duplicate(IComponent* _componentToDuplicate, InitEditorContext* _initializer)
		{
			ScriptComponent* s = dynamic_cast<ScriptComponent*>(_componentToDuplicate);
			if (s)
			{
				IComponent::Duplicate(_componentToDuplicate, _initializer);
				Init(*_initializer);

				m_ScriptName = s->m_ScriptName;
				m_ScriptInstance = nullptr;
				m_ScriptSystem = s->m_ScriptSystem;
			}
		}

		void ScriptComponent::Destroy()
		{
			if (m_ScriptInstance)
				ResourceManager::GetInstance()->GetScriptManager()->DestroyCSharpScript(m_ScriptInstance);
		}

		void ScriptComponent::OnCollisionEnter(RigidBody* _rb)
		{
			if (p_Enable)
				m_ScriptSystem->AddToColEnter(this, _rb);
		}

		void ScriptComponent::OnCollisionStay(RigidBody* _rb)
		{
			if (p_Enable)
				m_ScriptSystem->AddToColStay(this, _rb);
		}

		void ScriptComponent::OnCollisionExit(RigidBody* _rb)
		{
			if (p_Enable)
				m_ScriptSystem->AddToColExit(this, _rb);
		}


		void ScriptComponent::OnTriggerEnter(RigidBody* _rb)
		{
			if(p_Enable)
				m_ScriptSystem->AddToTriggerEnter(this, _rb);
		}

		void ScriptComponent::OnTriggerStay(RigidBody* _rb)
		{
			if (p_Enable)
				m_ScriptSystem->AddToTriggerStay(this, _rb);
		}

		void ScriptComponent::OnTriggerExit(RigidBody* _rb)
		{
			if (p_Enable)
				m_ScriptSystem->AddToTriggerExit(this, _rb);
		}

	}
}