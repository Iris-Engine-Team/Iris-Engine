#pragma once
#include <vector> 
#include "Core/Tools/Macros.h"
#include "Core/Graphics/Renderer/DrawCommand.h"

namespace Iris
{
    namespace Engine
    {
        class Scene;
        class IRenderer;

		struct DrawCommand;
		class ModelRenderer;

		struct DirectionalLightData;
		struct PointLightData;
		struct SpotLightData;
		class LightComponent;
		
		class SkyboxObject;
		class CameraComponent;

		template<class T>
		class ComponentTable;

		class ResourceContext;
		class RenderingContext;


        class RenderSystem
        {
        public:
            RenderSystem(Scene* _scene);
			void SetScene(Scene* _scene);

            /// <summary>
			/// <para>Updates index lists (cast shadow, opaque, transparent)</para>
            /// <para>Updates index lights lists (directional, spot, point)</para> 
			/// </summary>
            /// <param name="_renderer"></param>
            void Update(IRenderer* _renderer, RenderingContext* _renderingContext, ResourceContext* _resourceContext);

            void UpdateLightData(IRenderer* _renderer);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_resourceContext"></param>
			void Destroy();

			void Clear();

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_renderer"></param>
			void RenderShadow(IRenderer* _renderer);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_renderer"></param>
			void RenderOpaque(IRenderer* _renderer, const CameraComponent& _cameraComponent, SkyboxObject* _skybox);
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_renderer"></param>
			void RenderTransparent(IRenderer* _renderer, const CameraComponent& _cameraComponent, SkyboxObject* _skybox);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_renderer"></param>
			void RenderColliders(IRenderer* _renderer);

			bool IsOpaqueListEmpty() { return m_OpaqueLitObjects.empty(); }

			std::vector<LightComponent> GetDirectionalLights();
			std::vector<LightComponent> GetSpotLights();
			std::vector<LightComponent> GetPointLights();

			IRIS_ENGINE_API SkyboxObject* GetSkybox() { return m_Skybox; }

        private:

			/// <summary>
			/// Gives the light info to the renderer
			/// </summary>
			/// <param name="_renderer"></param>
			void SendLightData(IRenderer* _renderer);

			/// <summary>
			/// Makes directional data for renderer with the given light component
			/// </summary>
			/// <param name="_lightComponent">: component to extract the data</param>
			/// <returns>The directional light data</returns>
			DirectionalLightData GetDirectionalLightData(LightComponent* _lightComponent);

			/// <summary>
			/// Makes point light data for renderer with the given light component
			/// </summary>
			/// <param name="_lightComponent">: component to extract the data</param>
			/// <returns>The point light data</returns>
			PointLightData GetPointLightData(LightComponent* _lightComponent);

			/// <summary>
			/// Makes directional data for renderer with the given light component
			/// </summary>
			/// <param name="_lightComponent">: component to extract the data</param>
			/// <returns>The spot light data</returns>
			SpotLightData GetSpotLightData(LightComponent* _lightComponent);

        private:

			SkyboxObject* m_Skybox;

            Scene* m_SceneRef = nullptr;

            ComponentTable<ModelRenderer>* m_ModelRenderers = nullptr;
			ComponentTable<LightComponent>* m_Lights = nullptr;

            std::vector<size_t> m_IndexDirectionalLights;
            std::vector<size_t> m_IndexSpotLights;
            std::vector<size_t> m_IndexPointLights;

			std::vector<size_t> m_IndexCastShadow{};
            std::vector<DrawCommand> m_OpaqueLitObjects{};
            std::vector<DrawCommand> m_OpaqueUnlitObjects{};
            std::vector<DrawCommand> m_TransparentObjects{};
        };
    }
}