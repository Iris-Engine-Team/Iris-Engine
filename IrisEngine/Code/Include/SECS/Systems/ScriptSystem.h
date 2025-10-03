#pragma once

#include "SECS/Components/ScriptComponent.h"
#include <unordered_map>
#include <vector>

namespace Iris
{
    namespace Engine
    {
        class Scene;

        template<class T>
        class ComponentTable;

        class IrisObject;
        class RigidBody;
        class ScriptManager;

        class ScriptSystem
        {

        public :
            ScriptSystem(Scene* _scene);

            /// <summary>
           /// Link scene and take the ref of the camera table
           /// </summary>
            void SetScene(Scene* _scene);

            /// <summary>
            /// Set instance of all scripts in the scene
            /// </summary>
            IRIS_ENGINE_API void SetScriptInstances();

            /// <summary>
            /// Destroy function
            /// </summary>
            void Destroy();

            /// <summary>
            /// Clear all table, handle, scene ref 
            /// </summary>
            void Clear();

            /// <summary>
            /// Updates all available script
            /// </summary>
            void Update();

            /// <summary>
            /// Calls fixed update function of all available script
            /// </summary>
            void FixedUpdate();

            void AddToAwakeList(ScriptComponent* _script);

            void AddToColEnter(ScriptComponent* _script, RigidBody* _other);
            void AddToColStay(ScriptComponent* _script, RigidBody* _other);
            void AddToColExit(ScriptComponent* _script, RigidBody* _other);

            void AddToTriggerEnter(ScriptComponent* _script, RigidBody* _other);
            void AddToTriggerStay(ScriptComponent* _script, RigidBody* _other);
            void AddToTriggerExit(ScriptComponent* _script, RigidBody* _other);
            IE_FORCE_INLINE void SetInstanciateScript(bool _activate) { m_InstanciateScript = _activate; }
            IE_FORCE_INLINE bool GetInstanciateScript() { return m_InstanciateScript; }

        private:
            void CallAwakeStart(std::vector<ScriptComponent>& _scripts, ScriptManager* _scriptManager);
            void CallPhysicEvent(std::vector<ScriptComponent>& _scripts, ScriptManager* _scriptManager);
        private:
            Scene* m_SceneRef = nullptr;

            ComponentTable<ScriptComponent>* m_Scripts = nullptr;

            std::vector<IrisObject*> m_ScriptToAwake;
            std::vector<IrisObject*> m_ScriptToStart;

            /// <summary>
            /// key - script's irisobject, value - all rigidbodies he collided/trigger with
            /// </summary>
            
            std::mutex m_OnColEnterMutex;
            std::mutex m_OnColStayMutex;
            std::mutex m_OnColExitMutex;
            std::unordered_map<IrisObject*, std::vector<RigidBody*>> m_ScriptOnCollisionEnter;
            std::unordered_map<IrisObject*, std::vector<RigidBody*>> m_ScriptOnCollisionStay;
            std::unordered_map<IrisObject*, std::vector<RigidBody*>> m_ScriptOnCollisionExit;

            std::mutex m_OnTrigEnterMutex;
            std::mutex m_OnTrigStayMutex;
            std::mutex m_OnTrigExitMutex;
            std::unordered_map<IrisObject*, std::vector<RigidBody*>> m_ScriptOnTriggerEnter;
            std::unordered_map<IrisObject*, std::vector<RigidBody*>> m_ScriptOnTriggerStay;
            std::unordered_map<IrisObject*, std::vector<RigidBody*>> m_ScriptOnTriggerExit;

            bool m_InstanciateScript = false;
        };
    }
}