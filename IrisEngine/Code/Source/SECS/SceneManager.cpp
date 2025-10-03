#include "SECS/SceneManager.h"

#include "Resources/MetadataManager.h"
#include "CSharpScripting/ScriptManager.h"

#include "SECS/Systems/RenderSystem.h"
#include "SECS/Systems/CameraSystem.h"
#include "SECS/Systems/PhysicsSystem.h"
#include "Core/Physics/JoltImpl/JoltPhysicsSysteme.h"
#include "SECS/Systems/ScriptSystem.h"

#include "SECS/Scene.h"


namespace Iris
{
	namespace Engine
	{
		void SceneManager::Init(MetaDataManager* _metadaManager, const ThreadManager* _threadManager, ResourceContext* _resourceContext, RenderingContext* _renderingContext, PhysicType _physicType)
		{
			m_ThreadManagerRef = _threadManager;
			m_MetadaManagerRef = _metadaManager;
			m_ResourceContextRef = _resourceContext;
			m_RenderingContextRef = _renderingContext;

			m_ScriptSystem = new ScriptSystem(nullptr);
			m_CameraSystem = new CameraSystem(nullptr);
			m_RenderSystem = new RenderSystem(nullptr);

			switch (_physicType)
			{
			case Iris::Engine::PhysicType::IE_TYPE_JOLT:
			{
				m_PhysicsSystem = new JoltPhysicsSystem(nullptr);
				break;
			}
			default:
				break;
			}

			size_t nbThread = std::thread::hardware_concurrency() - 1;
			m_PhysicsSystem->Init(nbThread - m_ThreadManagerRef->GetNbThread());
		}

		void SceneManager::Destroy()
		{
			m_ScriptSystem->Destroy();
			delete m_ScriptSystem;

			m_CameraSystem->Destroy(*m_ResourceContextRef, *m_RenderingContextRef);
			delete m_CameraSystem;

			m_RenderSystem->Destroy();
			delete m_RenderSystem;

			m_PhysicsSystem->Destroy();
			delete m_PhysicsSystem;

			for (auto it = m_Scenes.begin(); it != m_Scenes.end(); ++it)
			{
				//it->second->Save(m_MetadaManagerRef);
				it->second->Destroy();
				delete it->second;
			}

			m_CurrentScene = nullptr;
		}

		IMetadata* SceneManager::CreateCurrentSceneMetadata()
		{
			std::string currentSceneName = m_CurrentScene->GetSceneName();

			std::filesystem::path path = FIND_PATH(currentSceneName + ".meta");

			if (path == "")
				path = "Game\\Scenes\\" + currentSceneName + ".meta";
				//path = "..\\Assets\\Content\\ScenesSaves\\" + currentSceneName + ".meta";

			IMetadata* sceneData = m_MetadaManagerRef->CreateMetadata(path, currentSceneName);
			return sceneData;
		}

		void SceneManager::SwitchScene(std::string _sceneName)
		{
			auto scene = m_Scenes.find(_sceneName);
			if (scene == m_Scenes.end())
				DEBUG_LOG(("This scene doesn't exist \"" + _sceneName + "\"").c_str());

			if (m_CurrentScene)
			{
				m_ScriptSystem->Clear();
				m_CameraSystem->Clear(*m_ResourceContextRef, *m_RenderingContextRef);
				m_RenderSystem->Clear();
				m_PhysicsSystem->Clear();
			}

			m_ScriptSystem->SetScene(m_CurrentScene);
			m_CameraSystem->SetScene(m_CurrentScene);
			m_RenderSystem->SetScene(m_CurrentScene);
			m_PhysicsSystem->SetScene(m_CurrentScene);

			m_CurrentScene = scene->second;
		}

		Scene* SceneManager::AddScene(std::string _sceneName)
		{
			auto scene = m_Scenes.find(_sceneName);
			if (scene == m_Scenes.end())
			{
				if (m_CurrentScene)
				{
					m_ScriptSystem->Clear();
					m_CameraSystem->Clear(*m_ResourceContextRef, *m_RenderingContextRef);
					m_RenderSystem->Clear();
					m_PhysicsSystem->Clear();
				}

				m_CurrentScene = new Scene(_sceneName);
				m_ScriptSystem->SetScene(m_CurrentScene);
				m_CameraSystem->SetScene(m_CurrentScene);
				m_RenderSystem->SetScene(m_CurrentScene);
				m_PhysicsSystem->SetScene(m_CurrentScene);
				m_Scenes[_sceneName] = m_CurrentScene;

				m_CurrentScene->Init(CreateCurrentSceneMetadata(), m_ThreadManagerRef, m_ResourceContextRef, m_CameraSystem, m_RenderSystem, m_PhysicsSystem, m_ScriptSystem);
				return m_CurrentScene;
			}

			return nullptr;
		}

		void SceneManager::LoadCurrentScene(InitEditorContext* _initializer)
		{
			if (m_CurrentScene)
				m_CurrentScene->Load(*_initializer);
		}

		void SceneManager::ReLoadCurrentScene(InitEditorContext* _initializer)
		{
			if (m_CurrentScene)
				m_CurrentScene->ReLoad(*_initializer);
		}

		void SceneManager::SaveCurrentScene()
		{
			if (m_CurrentScene)
				m_CurrentScene->Save(m_MetadaManagerRef);
		}
	}
}