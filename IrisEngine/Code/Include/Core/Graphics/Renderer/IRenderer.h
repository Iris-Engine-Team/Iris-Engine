#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"
#include "Core/Graphics/RHI/CommonTypes.h"
#include "SECS/Components/LightComponent.h"

#include "Matrix/Matrix4.h"
#include <array>

namespace Iris
{
	namespace Engine
	{
		class RenderingContext;
		class ResourceContext;
		class IWindow;
		class ICommandBuffer;
		class ISwapChain;
		class Mesh;
		class IDescriptor;
		class IBuffer;
		class IRenderPass;
		class IPipeline;
		class IFramebuffer;
		class IBuffer;
		class ISampler;
		class IImage;
		class Texture;
		class CameraComponent;
		class Material;
		class SkyboxObject;

		struct CamDataUBO
		{
			Matrix4 ProjectionView;
			Matrix4 View;
			Matrix4 Projection;
			Vector4 CameraPosition;
		};

		struct ExtendedCamDataUBO
		{
			Matrix4 View;
			Matrix4 Projection;
		};

		struct WireframeUBO
		{
			Matrix4 Transform;
			Vector4 Color;
		};

		struct CubemapUBO
		{
			Matrix4 Projection{};
			std::array<Matrix4, 6> Views{};
		};

		enum class RendererType : int
		{
			IE_RENDERER_FORWARD		= 0,
			IE_RENDERER_DEFERRED	= 1,
		};

		class IRenderer
		{
		public:
			/// <summary>
			/// IRenderer default destructor
			/// </summary>
			IRIS_ENGINE_API virtual ~IRenderer() = default;
			
			IRIS_ENGINE_API IE_FORCE_INLINE RendererType GetType() const { return p_Type; }

			IRIS_ENGINE_API IE_FORCE_INLINE IRenderPass* GetMainRenderPass() const { return p_MainRenderPass; }

			IRIS_ENGINE_API IE_FORCE_INLINE IRenderPass* GetFinalRenderPass() const { return m_FinalRenderPass; }
			IRIS_ENGINE_API IE_FORCE_INLINE IRenderPass* GetShadowRenderPass() const { return p_ShadowRP; }
			IRIS_ENGINE_API IE_FORCE_INLINE IPipeline* GetPointShadowPipeline() const { return p_PointShadowPipeline; }
			IRIS_ENGINE_API IE_FORCE_INLINE IPipeline* GetDirectionalShadowPipeline() const { return p_DirectionalShadowPipeline; }
			IRIS_ENGINE_API IE_FORCE_INLINE void SetFinalRenderPass(IRenderPass* _finalRenderPass) { m_FinalRenderPass = _finalRenderPass; }
			IRIS_ENGINE_API IE_FORCE_INLINE ICommandBuffer* GetCurrentCommandBuffer() const { return p_CurrentCommandBuffer; }

			IE_FORCE_INLINE virtual IPipeline* GetMainPipeline() const { return p_MainPipeline; }
			IE_FORCE_INLINE virtual IPipeline* GetLightPipeline() const { return p_MainPipeline; }
			IE_FORCE_INLINE virtual IPipeline* GetCurrentPipeline() const { return p_CurrentPipeline; }
			IE_FORCE_INLINE virtual IPipeline* GetCurrentPointShadowPipeline() const { return p_PointShadowPipeline; }
			IE_FORCE_INLINE virtual IPipeline* GetSkybox() const { return p_SkyboxPipeline; }

			IE_FORCE_INLINE unsigned int GetCameraLayoutIndex()		const { return p_CameraLayoutIndex; }
			IE_FORCE_INLINE unsigned int GetTransformLayoutIndex()	const { return p_TransformLayoutIndex; }
			IE_FORCE_INLINE unsigned int GetMaterialLayoutIndex()	const { return p_MaterialLayoutIndex; }
			IE_FORCE_INLINE unsigned int GetLightLayoutIndex()		const { return p_LightLayoutIndex; }
			IE_FORCE_INLINE unsigned int GetShadowLayoutIndex()		const { return p_ShadowLayoutIndex; }
			IE_FORCE_INLINE unsigned int GetIBLLayoutIndex()		const { return p_IBLLayoutIndex; }

			IE_FORCE_INLINE unsigned int GetOpaquePassIndex()			const { return p_OpaquePassIndex; }
			IE_FORCE_INLINE unsigned int GetTransparentPassIndex()		const { return p_TransparentPassIndex; }

			IE_FORCE_INLINE unsigned int GetOpaquePassAttachmentCount()				const { return p_OpaquePassAttachmentCount; }
			IE_FORCE_INLINE unsigned int GetTransparentPassAttachmentCount()		const { return p_TransparentPassAttachmentCount; }

			IE_FORCE_INLINE std::vector<RHIAttachment> GetMainAttachments() const { return p_MainAttachments; }

			/// <summary>
			/// Initializes the renderer
			/// </summary>
			/// <param name="_renderingContext"></param>
			/// <returns></returns>
			IRIS_ENGINE_API virtual RHIResult Initialize(RenderingContext* _renderingContext, ResourceContext* _resourceContext);

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API virtual RHIResult Terminate();

			virtual std::vector<IDescriptor*> OnFramebufferResize(const std::vector<IFramebuffer*>& _framebuffers, std::vector<IDescriptor*>& _framebufferDescriptors) { (void)_framebuffers; (void)_framebufferDescriptors; return std::vector<IDescriptor*>(); }

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_window"></param>
			/// <returns></returns>
			IRIS_ENGINE_API virtual void StartFrame(IWindow* _window, IRenderPass* _renderPass);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_window"></param>
			/// <returns></returns>
			IRIS_ENGINE_API virtual void EndFrame(IWindow* _window, IRenderPass* _renderPass);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_renderPass"></param>
			/// <param name="_framebuffer"></param>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			IRIS_ENGINE_API void StartRenderPass(IRenderPass* _renderPass, const std::vector<IFramebuffer*>& _framebuffer, unsigned int _width, unsigned int _height);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_renderPass"></param>
			/// <param name="_framebuffer"></param>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			IRIS_ENGINE_API void StartRenderPass(IRenderPass* _renderPass, IFramebuffer* _framebuffer, unsigned int _width, unsigned int _height);

			/// <summary>
			/// 
			/// </summary>
			IRIS_ENGINE_API void EndRenderPass();

			void StartRenderingShadows();
			void EndRenderingShadows();

			void StartRenderingDirectionalShadow(LightComponent& _light);

			void StartRenderingPointShadow(LightComponent& _light);

			void EndRenderingShadow();

			void StartRenderingColliders(const CameraComponent& _camera);
			void EndRenderingColliders();

			void UpdateShadowMaps(const std::vector<ShadowMap*>& _dirShadowMaps, const std::vector<ShadowMap*>& _spotShadowMaps, const std::vector<ShadowMap*>& _pointShadowMaps);

			/// <summary>
			/// 
			/// </summary>
			virtual void StartRenderingOpaque(const CameraComponent& _camera, const SkyboxObject* _skybox) = 0;
			
			/// <summary>
			/// 
			/// </summary>
			virtual void EndRenderingOpaque(const CameraComponent& _camera, const SkyboxObject* _skybox) = 0;

			/// <summary>
			/// 
			/// </summary>
			virtual void StartRenderingTransparent(const CameraComponent& _camera, const SkyboxObject* _skybox) = 0;

			/// <summary>
			/// 
			/// </summary>
			virtual void EndRenderingTransparent(const CameraComponent& _camera) = 0;

			void StartRenderingSkybox(IDescriptor* _skybox, const CameraComponent& _camera);

			/// <summary>
			/// 
			/// </summary>
			void EndRenderingSkybox(const CameraComponent& _camera);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_buffer"></param>
			/// <param name="_descriptor"></param>
			/// <param name="_set"></param>
			void BindDescriptor(IPipeline* _pipeline, const std::vector<IDescriptor*>& _descriptors, unsigned int _set);
			void BindDescriptor(IPipeline* _pipeline, IDescriptor* _descriptors, unsigned int _set);
			void BindBlockVariable(IPipeline* _pipeline, size_t _size, void* _data, RHIShaderType _shaderBind);

			void DrawCollider(const Matrix4 _transform, const Vector4 _color, Mesh* _mesh);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_mesh"></param>
			void DrawModel(IBuffer* _vertexBuffer, IBuffer* _indexBuffer, size_t _indicesCount, const Material* _material);
			void DrawMesh(ICommandBuffer* _cmdBuffer, Mesh* _mesh);
			void DrawModel(Mesh* _mesh, const std::vector<Material*> _materials);

			virtual void StartDrawCommand(IPipeline* _pipeline, const CameraComponent& _camera, SkyboxObject* _skybox, bool _isOpaque) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_buffers"></param>
			/// <param name="_newData"></param>
			/// <param name="_size"></param>
			void UpdateBuffer(const std::vector<IBuffer*>& _buffers, void* _newData, size_t _size);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_newData"></param>
			/// <param name="_size"></param>
			void UpdateLightBuffer(void* _newData, size_t _size);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_window"></param>
			/// <returns></returns>
			IRIS_ENGINE_API void RecreateSwapChain();

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_verticesCount"></param>
			void Draw(ICommandBuffer* _cmdBuffer, unsigned int _verticesCount);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			void BindPipeline(IPipeline* _pipeline, unsigned int _width, unsigned int _height, bool _invert);

			void ConvertPanoramicTextureToCubemap(Texture* _baseTexture, IImage* _newTexture);
			void ComputeIBLEnvironmentdMap(Texture* _baseTexture, IImage* _newTexture);
			void ComputeIBLPrefilteredMap(Texture* _baseTexture, IImage* _newTexture);
			void ComputeIBLLookUpTexture(IImage* _newTexture);

			unsigned int lastWidth = 0, lastHeight = 0;

		protected:
			 virtual void CreateTemplates();

		protected:
			RendererType p_Type = RendererType::IE_RENDERER_FORWARD;

			RenderingContext* p_RenderingContext = nullptr;
			ResourceContext* p_ResourceContext = nullptr;

			IRenderPass* p_MainRenderPass = nullptr;
			IPipeline* p_MainPipeline = nullptr;
			IPipeline* p_TransparentPipeline = nullptr;
			IPipeline* p_GridPipeline = nullptr;
			IPipeline* p_WireframePipeline = nullptr;

			std::vector<RHIAttachment> p_MainAttachments{};
			
			std::vector<IBuffer*> p_LightBuffers;
			std::vector<IDescriptor*> p_LightBufferDescriptors;
			std::vector<IDescriptor*> p_ShadowsDescriptors;

			unsigned int p_CameraLayoutIndex = 0;
			unsigned int p_TransformLayoutIndex = 1;
			unsigned int p_MaterialLayoutIndex = 2;
			unsigned int p_LightLayoutIndex = 3;
			unsigned int p_ShadowLayoutIndex = 4;
			unsigned int p_IBLLayoutIndex = 5;

			unsigned int p_OpaquePassIndex = 0;
			unsigned int p_TransparentPassIndex = 0;

			unsigned int p_OpaquePassAttachmentCount = 1;
			unsigned int p_TransparentPassAttachmentCount = 1;
			
			unsigned int p_CurrentFrame = 0;
			ICommandBuffer* p_CurrentCommandBuffer = nullptr;
			ISwapChain* p_SwapChain = nullptr;

			IRenderPass* p_ShadowRP = nullptr;
			IRenderPass* p_PointShadowRP = nullptr;

			IPipeline* p_DirectionalShadowPipeline = nullptr;
			IPipeline* p_PointShadowPipeline = nullptr;
			IPipeline* p_SkyboxPipeline = nullptr;

			IPipeline* p_CurrentPipeline = nullptr;
		private:

			IRenderPass* m_FinalRenderPass = nullptr;
			IRenderPass* m_CurrentRenderPass = nullptr;

			IRenderPass* m_PanoramicToCubeRP = nullptr;
			IRenderPass* m_IBLPrefilteredRP = nullptr;
			IRenderPass* m_IBLLookUpTextureRP = nullptr;

			IPipeline* m_PanoramicToCube = nullptr;
			IPipeline* m_IBLIrradiancePipeline = nullptr;
			IPipeline* m_IBLPrefilteredPipeline = nullptr;
			IPipeline* m_IBLLookUpTexturePipeline = nullptr;

		};
	}
}