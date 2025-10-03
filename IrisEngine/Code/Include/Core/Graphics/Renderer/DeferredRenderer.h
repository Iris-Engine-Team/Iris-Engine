#pragma once

#include "Core/Graphics/Renderer/IRenderer.h"

namespace Iris
{
	namespace Engine
	{
		class DeferredRenderer : public IRenderer
		{
		public:
			/// <summary>
			/// 
			/// </summary>
			IRIS_ENGINE_API ~DeferredRenderer() override final = default;

			IE_FORCE_INLINE IPipeline* GetLightPipeline() const override final { return m_LightPipeline; }

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

			IRIS_ENGINE_API std::vector<IDescriptor*> OnFramebufferResize(const std::vector<IFramebuffer*>& _framebuffers, std::vector<IDescriptor*>& _framebufferDescriptors) override;

			void StartRenderingOpaque(const CameraComponent& _camera, const SkyboxObject* _skybox) override;
			void EndRenderingOpaque(const CameraComponent& _camera, const SkyboxObject* _skybox) override;

			void StartRenderingTransparent(const CameraComponent& _camera, const SkyboxObject* _skybox) override;
			void EndRenderingTransparent(const CameraComponent& _camera) override;

			void StartDrawCommand(IPipeline* _pipeline, const CameraComponent& _camera, SkyboxObject* _skybox, bool _isOpaque) override;
		
		private:
			IPipeline* m_LightPipeline = nullptr;
		};
	}
}