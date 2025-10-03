#include "SECS/Systems/ScriptSystem.h"
#include "SECS/Components/ScriptComponent.h"
#include "SECS/IrisObject.h"
#include "SECS/Scene.h"
#include "CSharpScripting/ScriptManager.h"

namespace Iris
{
	namespace Engine
	{
		ScriptSystem::ScriptSystem(Scene* _scene)
		{
			SetScene(_scene);
		}

		void ScriptSystem::SetScene(Scene* _scene)
		{
			if (!_scene)
				return;

			m_SceneRef = _scene;

			if (m_SceneRef->ContainTable<ScriptComponent>())
				m_Scripts = m_SceneRef->GetComponentTable<ScriptComponent>();
			else
				m_Scripts = m_SceneRef->AddTable<ScriptComponent>();
		}

		void ScriptSystem::SetScriptInstances()
		{
			for (ScriptComponent& script : m_Scripts->GetAllComponents())
				script.SetInstance(ResourceManager::GetInstance()->GetScriptManager()->InitCSharpScript(script.GetIrisObject()->GetHandle<ComponentHandle<ScriptComponent>, ScriptComponent>()));
		}

		void ScriptSystem::Destroy()
		{
		}

		void ScriptSystem::Clear()
		{
			m_SceneRef = nullptr;

			m_ScriptOnCollisionEnter.clear();
			m_ScriptOnCollisionStay.clear();
			m_ScriptOnCollisionExit.clear();
			m_ScriptOnTriggerEnter.clear();
			m_ScriptOnTriggerStay.clear();
			m_ScriptOnTriggerExit.clear();
			m_ScriptToAwake.clear();
			m_ScriptToStart.clear();
		}

		void ScriptSystem::CallAwakeStart(std::vector<ScriptComponent>& _scripts, ScriptManager* _scriptManager)
		{
			size_t size = m_ScriptToAwake.size();
			for (size_t i = 0; i < size; ++i)
			{
				ScriptComponent* script = &_scripts[m_Scripts->ContainsEntity(m_ScriptToAwake[i])];
				_scriptManager->RunCSFunction(script, FuncType::AWAKE);
			}
			m_ScriptToAwake.clear();

			size = m_ScriptToStart.size();
			for (size_t i = 0; i < size; ++i)
			{
				ScriptComponent* script = &_scripts[m_Scripts->ContainsEntity(m_ScriptToStart[i])];
				_scriptManager->RunCSFunction(script, FuncType::START);
			}
			m_ScriptToStart.clear();
		}

		void ScriptSystem::CallPhysicEvent(std::vector<ScriptComponent>& _scripts, ScriptManager* _scriptManager)
		{
			for (auto it = m_ScriptOnCollisionEnter.begin(); it != m_ScriptOnCollisionEnter.end(); ++it)
			{
				ScriptComponent* script = &_scripts[m_Scripts->ContainsEntity(it->first)];

				std::vector<RigidBody*>& others = it->second;
				size_t otherCount = others.size();
				for (size_t j = 0; j < otherCount; ++j)
				{
					RigidBody* rb = others[j];
					_scriptManager->RunCSCallback(script, FuncType::COL_ENTER, rb->GetIrisObject()->GetHandle<ComponentHandle<RigidBody>, RigidBody>());
				}
			}
			m_ScriptOnCollisionEnter.clear();

			for (auto it = m_ScriptOnCollisionStay.begin(); it != m_ScriptOnCollisionStay.end(); ++it)
			{
				ScriptComponent* script = &_scripts[m_Scripts->ContainsEntity(it->first)];

				std::vector<RigidBody*>& others = it->second;
				size_t otherCount = others.size();
				for (size_t j = 0; j < otherCount; ++j)
				{
					RigidBody* rb = others[j];
					_scriptManager->RunCSCallback(script, FuncType::COL_STAY, rb->GetIrisObject()->GetHandle<ComponentHandle<RigidBody>, RigidBody>());
				}
			}
			m_ScriptOnCollisionStay.clear();

			for (auto it = m_ScriptOnCollisionExit.begin(); it != m_ScriptOnCollisionExit.end(); ++it)
			{
				ScriptComponent* script = &_scripts[m_Scripts->ContainsEntity(it->first)];

				std::vector<RigidBody*>& others = it->second;
				size_t otherCount = others.size();
				for (size_t j = 0; j < otherCount; ++j)
				{
					RigidBody* rb = others[j];
					_scriptManager->RunCSCallback(script, FuncType::COL_EXIT, rb->GetIrisObject()->GetHandle<ComponentHandle<RigidBody>, RigidBody>());
				}
			}
			m_ScriptOnCollisionExit.clear();


			for (auto it = m_ScriptOnTriggerEnter.begin(); it != m_ScriptOnTriggerEnter.end(); ++it)
			{
				ScriptComponent* script = &_scripts[m_Scripts->ContainsEntity(it->first)];

				std::vector<RigidBody*>& others = it->second;
				size_t otherCount = others.size();
				for (size_t j = 0; j < otherCount; ++j)
				{
					RigidBody* rb = others[j];
					_scriptManager->RunCSCallback(script, FuncType::TRIG_ENTER, rb->GetIrisObject()->GetHandle<ComponentHandle<RigidBody>, RigidBody>());
				}
			}
			m_ScriptOnTriggerEnter.clear();

			for (auto it = m_ScriptOnTriggerStay.begin(); it != m_ScriptOnTriggerStay.end(); ++it)
			{
				ScriptComponent* script = &_scripts[m_Scripts->ContainsEntity(it->first)];

				std::vector<RigidBody*>& others = it->second;
				size_t otherCount = others.size();
				for (size_t j = 0; j < otherCount; ++j)
				{
					RigidBody* rb = others[j];
					_scriptManager->RunCSCallback(script, FuncType::TRIG_STAY, rb->GetIrisObject()->GetHandle<ComponentHandle<RigidBody>, RigidBody>());
				}
			}
			m_ScriptOnTriggerStay.clear();

			for (auto it = m_ScriptOnTriggerExit.begin(); it != m_ScriptOnTriggerExit.end(); ++it)
			{
				ScriptComponent* script = &_scripts[m_Scripts->ContainsEntity(it->first)];

				std::vector<RigidBody*>& others = it->second;
				size_t otherCount = others.size();
				for (size_t j = 0; j < otherCount; ++j)
				{
					RigidBody* rb = others[j];
					_scriptManager->RunCSCallback(script, FuncType::TRIG_EXIT, rb->GetIrisObject()->GetHandle<ComponentHandle<RigidBody>, RigidBody>());
				}
			}
			m_ScriptOnTriggerExit.clear();
		}

		void ScriptSystem::Update()
		{
			std::vector<ScriptComponent>& scripts = m_Scripts->GetAllComponents();
			ScriptManager* sm = ResourceManager::GetInstance()->GetScriptManager();

			CallAwakeStart(scripts, sm);
			CallPhysicEvent(scripts, sm);

			size_t size = m_Scripts->ComponentCount();
			for (size_t i = 0; i < size; ++i)
			{
				if (!scripts[i].IsEnable())
					continue;

				sm->RunCSFunction(&scripts[i], FuncType::UPDATE);
			}
		}

		void ScriptSystem::FixedUpdate()
		{
			std::vector<ScriptComponent>& scripts = m_Scripts->GetAllComponents();
			ScriptManager* sm = ResourceManager::GetInstance()->GetScriptManager();

			size_t size = m_Scripts->ComponentCount();
			for (size_t i = 0; i < size; ++i)
			{
				if (!scripts[i].IsEnable())
					continue;

				sm->RunCSFunction(&scripts[i], FuncType::FIXED_UPDATE);
			}
		}

		#pragma region Events
		void ScriptSystem::AddToAwakeList(ScriptComponent* _script)
		{
			m_ScriptToAwake.push_back(_script->GetIrisObject());
			m_ScriptToStart.push_back(_script->GetIrisObject());
		}

		void ScriptSystem::AddToColEnter(ScriptComponent* _script, RigidBody* _other)
		{
			std::lock_guard<std::mutex> lock(m_OnColEnterMutex);
			m_ScriptOnCollisionEnter[_script->GetIrisObject()].push_back(_other);
		}

		void ScriptSystem::AddToColStay(ScriptComponent* _script, RigidBody* _other)
		{
			std::lock_guard<std::mutex> lock(m_OnColStayMutex);
			m_ScriptOnCollisionStay[_script->GetIrisObject()].push_back(_other);
		}

		void ScriptSystem::AddToColExit(ScriptComponent* _script, RigidBody* _other)
		{
			std::lock_guard<std::mutex> lock(m_OnColExitMutex);
			m_ScriptOnCollisionExit[_script->GetIrisObject()].push_back(_other);
		}

		void ScriptSystem::AddToTriggerEnter(ScriptComponent* _script, RigidBody* _other)
		{
			std::lock_guard<std::mutex> lock(m_OnTrigEnterMutex);
			m_ScriptOnTriggerEnter[_script->GetIrisObject()].push_back(_other);
		}

		void ScriptSystem::AddToTriggerStay(ScriptComponent* _script, RigidBody* _other)
		{
			std::lock_guard<std::mutex> lock(m_OnTrigStayMutex);
			m_ScriptOnTriggerStay[_script->GetIrisObject()].push_back(_other);
		}

		void ScriptSystem::AddToTriggerExit(ScriptComponent* _script, RigidBody* _other)
		{
			std::lock_guard<std::mutex> lock(m_OnTrigExitMutex);
			m_ScriptOnTriggerExit[_script->GetIrisObject()].push_back(_other);
		}
		#pragma endregion
	}
}