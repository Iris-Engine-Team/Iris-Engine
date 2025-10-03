#pragma once
#include <map>
#include <string>
#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"

namespace Iris
{
	namespace Engine
	{
		class ThreadManager;

		class Scene;
		class CameraSystem;
		class RenderSystem;
		class IPhysicsSystem;
		class ScriptSystem;

		struct InitEditorContext;

		enum class PhysicType :int;

		class ResourceContext;
		class RenderingContext;
		class MetaDataManager;
		class IMetadata;

		class SceneManager
		{
		public:
			SceneManager() = default;
			~SceneManager() = default;

			void Init(MetaDataManager* _metadaManager, const ThreadManager* _threadManager, ResourceContext* _resourceContext, RenderingContext* _renderingContext, PhysicType _physicType);
			void Destroy();


		 	IRIS_ENGINE_API	IE_FORCE_INLINE Scene* GetCurrentScene() { return m_CurrentScene; }

			IRIS_ENGINE_API void SwitchScene(std::string _sceneName);

			IRIS_ENGINE_API Scene* AddScene(std::string _seneName);

			IRIS_ENGINE_API	IE_FORCE_INLINE CameraSystem* GetCameraSystem() const { return m_CameraSystem; }
			IRIS_ENGINE_API	IE_FORCE_INLINE RenderSystem* GetRenderSystem() const { return m_RenderSystem; }
			IRIS_ENGINE_API	IE_FORCE_INLINE IPhysicsSystem* GetPhysicsSystem() const { return m_PhysicsSystem; }
			IRIS_ENGINE_API	IE_FORCE_INLINE ScriptSystem* GetScriptSystem() const { return m_ScriptSystem; }

			IRIS_ENGINE_API	void LoadCurrentScene(InitEditorContext* _initializer);
			IRIS_ENGINE_API	void ReLoadCurrentScene(InitEditorContext* _initializer);
			IRIS_ENGINE_API	void SaveCurrentScene();

		private:
			IMetadata* CreateCurrentSceneMetadata();
		private:

			MetaDataManager* m_MetadaManagerRef = nullptr;
			ResourceContext* m_ResourceContextRef = nullptr;
			RenderingContext* m_RenderingContextRef = nullptr;

			std::map<std::string, Scene*> m_Scenes;
			Scene* m_CurrentScene = nullptr;

			CameraSystem* m_CameraSystem = nullptr;
			RenderSystem* m_RenderSystem = nullptr;
			IPhysicsSystem* m_PhysicsSystem = nullptr;
			ScriptSystem* m_ScriptSystem = nullptr;

			const ThreadManager* m_ThreadManagerRef = nullptr;
		};
	}
}