#pragma once

#include "SECS/Components/CameraComponent.h"

namespace Iris
{
    namespace Engine
    {
        class Scene;
        class IRenderer;

        template<class T>
        class ComponentTable;

        class CameraSystem
        {
        public:
            CameraSystem(Scene* _scene);

            /// <summary>
            /// Link scene and take the ref of the camera table
            /// </summary>
            void SetScene(Scene* _scene);

            /// <returns>All cameras in the scene</returns>
            IRIS_ENGINE_API std::vector<CameraComponent>& GetAllCameras();

            /// <summary>
            /// Bind a camera to render
            /// </summary>
            void BindCamera(CameraComponent* _cam, IRenderer* _renderer);

            /// <summary>
            /// Destroy function
            /// </summary>
            void Destroy(ResourceContext& _reosourceContext, RenderingContext& _renderingContext);
            
            /// <summary>
            /// Clear all table, handle, scene ref 
            /// </summary>
            void Clear(ResourceContext& _resourceContext, RenderingContext& _renderingContext, bool _ignoreEditorCamera = false);

            IRIS_ENGINE_API void Update();

            /// <returns>The main game camera</returns>
            IRIS_ENGINE_API IE_FORCE_INLINE ComponentHandle<CameraComponent>* GetMainCamera() const { return m_CurrentGameCamera; }
            
            /// <summary>
            /// Sets the given game camera as the main one
            /// </summary>
            IRIS_ENGINE_API void SetMainCamera(CameraComponent* _camera);

            /// <returns>The editor camera</returns>
            IRIS_ENGINE_API IE_FORCE_INLINE ComponentHandle<CameraComponent>* GetEditorCamera() const { return m_CurrentEditorCamera; }
            
            /// <summary>
            /// Sets the given camera as the editor camera
            /// </summary>
            IRIS_ENGINE_API void SetEditorCamera(CameraComponent* _camera);

        private:

            Scene* m_SceneRef = nullptr;

            ComponentTable<CameraComponent>* m_Cameras = nullptr;

            ComponentHandle<CameraComponent>* m_CurrentGameCamera = nullptr;
            ComponentHandle<CameraComponent>* m_CurrentEditorCamera = nullptr;
        };
    }
}