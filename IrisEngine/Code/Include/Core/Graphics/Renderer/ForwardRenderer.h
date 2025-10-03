#pragma once

#include "Core/Graphics/Renderer/IRenderer.h"

namespace Iris
{
	namespace Engine
	{
		class ISampler;

		class ForwardRenderer : public IRenderer
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			IRIS_ENGINE_API ~ForwardRenderer() override final = default;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_renderingContext"></param>
			/// <returns></returns>
			IRIS_ENGINE_API RHIResult Initialize(RenderingContext* _renderingContext, ResourceContext* _resourceContext) override final;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API RHIResult Terminate() override final;

			void StartRenderingOpaque(const CameraComponent& _camera, const SkyboxObject* _skybox) override;
			void EndRenderingOpaque(const CameraComponent& _camera, const SkyboxObject* _skybox) override;

			void StartRenderingTransparent(const CameraComponent& _camera, const SkyboxObject* _skybox) override;
			void EndRenderingTransparent(const CameraComponent& _camera) override;

			void StartDrawCommand(IPipeline* _pipeline, const CameraComponent& _camera, SkyboxObject* _skybox, bool _isOpaque) override;
		};
	}
}