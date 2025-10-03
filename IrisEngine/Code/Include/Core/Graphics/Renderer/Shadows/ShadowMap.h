#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"

#include <vector>

#include "Matrix/Matrix4.h"

namespace Iris
{
	namespace Engine
	{

#define IE_POINT_SHADOW_WIDTH 1024
#define IE_POINT_SHADOW_HEIGHT 1024

		class IRenderer;
		class IRenderPass;
		class IFramebuffer;
		class IPipeline;
		class ISampler;
		class IBuffer;
		class IDescriptor;
		class RenderingContext;
		class ResourceContext;

		struct DirectionalLightUBO
		{
			Matrix4 ProjectionView = Matrix4::Identity;
		};

		struct PointLightUBO
		{
			Matrix4 ProjectionViews[6];
			Vector4 LightPosition;
			float Far;
		};

		class ShadowMap
		{
		public:

			IE_FORCE_INLINE std::vector<IDescriptor*> GetShadowDataDescriptors() const { return m_ShadowMapDataDescriptors; }
			IE_FORCE_INLINE std::vector<IDescriptor*> GetShadowmMapDescriptors() const { return m_ShadowMapDescriptors; }
			IE_FORCE_INLINE std::vector<IFramebuffer*> GetFramebuffers() const { return m_ShadowFramebuffers; }
			IE_FORCE_INLINE ISampler* GetSampler() const { return m_ShadowMapSampler; }

			void DestroyShadowMap(RenderingContext* _renderingContext, ResourceContext* _resourceContext);
			void CreateDirectionalShadowMap(IPipeline* _lightPipeline, IPipeline* _shadowPipeline, IRenderPass* _renderpass, RenderingContext* _renderingContext, ResourceContext* _resourceContext, unsigned int _shadowLayout);
			void CreateSpotShadowMap(IPipeline* _lightPipeline, IPipeline* _shadowPipeline, IRenderPass* _renderpass, RenderingContext* _renderingContext, ResourceContext* _resourceContext, unsigned int _shadowLayout);
			void CreatePointShadowMap(IPipeline* _lightPipeline, IPipeline* _shadowPipeline, IRenderPass* _renderpass, RenderingContext* _renderingContext, ResourceContext* _resourceContext, unsigned int _shadowLayout);

			void UpdateDirectionalData(IRenderer* _renderer, Vector3 _lightPosition, Vector3 _lightDirection, float _near, float _far, float _top, float _bot, float _right, float _left);
			void UpdateSpotData(IRenderer* _renderer, Vector3 _lightPosition, Vector3 _lightDirection, float _near, float _far);
			void UpdatePointData(IRenderer* _renderer, Vector3 _lightPosition, float _near, float _far);

		private:
			void CreateShadowMap(IPipeline* _lightPipeline, IRenderPass* _renderpass, RenderingContext* _renderingContext, ResourceContext* _resourceContext, unsigned int _shadowLayout);


		private:
			std::vector<IFramebuffer*> m_ShadowFramebuffers{};
			ISampler* m_ShadowMapSampler = nullptr;
			std::vector<IDescriptor*> m_ShadowMapDescriptors{};

			std::vector<IDescriptor*> m_ShadowMapDataDescriptors{};
			std::vector<IBuffer*> m_ShadowMapDataBuffer{};

		};
	}
}