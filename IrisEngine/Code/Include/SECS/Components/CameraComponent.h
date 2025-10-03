#pragma once
#include "IComponent.h"

#include "Vector/Vector2.h"
#include "Vector/Vector3.h"
#include "Matrix/Matrix4.h"

namespace Iris
{
	namespace Engine
	{
		class ResourceContext;
		class RenderingContext;
		class IRenderer;
		class IBuffer;
		class IDescriptor;
		class IFramebuffer;

#define IE_CAMERA_FLAG_DRAW_COLLIDERS	(0x1 << 0x0)
#define IE_CAMERA_FLAG_DRAW_GRID		(0x1 << 0x1)
#define IE_CAMERA_FLAG_DRAW_SKYBOX		(0x1 << 0x2)

		class CameraComponent : public IComponent
		{
		public:
			DEFINE_COMPONENT(CameraComponent);
			static IE_FORCE_INLINE std::string GetName() { return "Camera"; }
			static IE_FORCE_INLINE ComponentID GetID() { return ComponentID::IE_CAMERA; }

			CameraComponent() = default;

			/// <summary>
			/// Writes data in the given metadata
			/// </summary>
			/// <param name="_metadata"></param>
			IRIS_ENGINE_API void Serialize(IMetadata* _metadata) final override;

			/// <summary>
			/// Init the components with the given data
			/// </summary>
			IRIS_ENGINE_API void Deserialization(IMetadata* _metadata, InitEditorContext& _initializer) final override;

			/// <param name="_position">: world position</param>
			/// <param name="_fov">(in radian)</param>
			/// <param name="_ortho">: projection is ortho or perspective</param>
			IRIS_ENGINE_API void Init(IRenderer& _renderer, ResourceContext& _resourceContext, RenderingContext& _renderingContext, const Vector3& _position, float _fov, float _aspect, float _near, float _far, bool _ortho);

			/// <summary>
			/// Init function
			/// </summary>
			/// <param name="_renderer"></param>
			/// <param name="_resourceContext"></param>
			IRIS_ENGINE_API void Init(IRenderer& _renderer, ResourceContext& _resourceContext, RenderingContext& _renderingContext);
			/// <summary>
			/// Init function for editor
			/// </summary>
			IRIS_ENGINE_API void Init(InitEditorContext& _initializer) override;

			IRIS_ENGINE_API void Copy(IComponent* _componentToCopy) final override;
			IRIS_ENGINE_API void Duplicate(IComponent* _componentToDuplicate, InitEditorContext* _initializer) final override;
			void Destroy() final override {};

			void DeleteBuffers(ResourceContext& _resourceContext, RenderingContext& _renderingContext);

			/// <summary>
			/// Calls by the application if its the main camera editor, allows to move and rotate the camera 
			/// </summary>
			IRIS_ENGINE_API void Update(double _deltaTime);

			void CheckTransformChange();

			/// <param name="_fov">: fov to set (in radian)</param>
			IRIS_ENGINE_API IE_FORCE_INLINE void SetFov(float _fov) { m_Fov = _fov; m_RecalculateP = true; }
		
			/// <returns>The fov in radian</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE float GetFov() const { return m_Fov; }

			/// <param name="_camSpeed">: camera speed to set</param>
			IRIS_ENGINE_API IE_FORCE_INLINE void SetSpeed(float _camSpeed) { m_CamSpeed = _camSpeed; }
			
			/// <returns>The cam speed</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE float GetSpeed() const { return m_CamSpeed; }

			/// <param name="_aspect">: new aspect ratio to set</param>
			IRIS_ENGINE_API IE_FORCE_INLINE void SetAspect(float _aspect) { m_AspectRatio = _aspect; m_RecalculateP = true; }

			/// <returns>The aspect ratio</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE float GetAspect() const { return m_AspectRatio; }

			/// <param name="_near">: near value to set</param>
			IRIS_ENGINE_API IE_FORCE_INLINE void SetNear(float _near) { m_Near = _near; m_RecalculateP = true; }

			/// <returns>The near value</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE float GetNear() const { return m_Near; }

			/// <param name="_far">: far value to set</param>
			IRIS_ENGINE_API IE_FORCE_INLINE void SetFar(float _far) { m_Far = _far; m_RecalculateP = true; }

			IRIS_ENGINE_API IE_FORCE_INLINE void SetDrawColliders(bool _drawColliders) { _drawColliders ? m_CameraFlags |= IE_CAMERA_FLAG_DRAW_COLLIDERS : m_CameraFlags &= ~IE_CAMERA_FLAG_DRAW_COLLIDERS; }
			IRIS_ENGINE_API IE_FORCE_INLINE void SetDrawGrid(bool _drawGrid) { _drawGrid ? m_CameraFlags |= IE_CAMERA_FLAG_DRAW_GRID : m_CameraFlags &= ~IE_CAMERA_FLAG_DRAW_GRID; }
			IRIS_ENGINE_API IE_FORCE_INLINE void SetDrawSkybox(bool _drawSkybox) { _drawSkybox ? m_CameraFlags |= IE_CAMERA_FLAG_DRAW_SKYBOX : m_CameraFlags &= ~IE_CAMERA_FLAG_DRAW_SKYBOX; }

			IRIS_ENGINE_API IE_FORCE_INLINE bool GetDrawColliders() const { return m_CameraFlags & IE_CAMERA_FLAG_DRAW_COLLIDERS; }
			IRIS_ENGINE_API IE_FORCE_INLINE bool GetDrawGrid() const { return m_CameraFlags & IE_CAMERA_FLAG_DRAW_GRID; }
			IRIS_ENGINE_API IE_FORCE_INLINE bool GetDrawSkybox() const { return m_CameraFlags & IE_CAMERA_FLAG_DRAW_SKYBOX; }

			IRIS_ENGINE_API IE_FORCE_INLINE std::vector<IFramebuffer*> GetCameraFramebuffers() const { return m_CameraFramebuffers; }
			IRIS_ENGINE_API IE_FORCE_INLINE std::vector<IDescriptor*> GetFramebufferDescriptors() const { return m_FramebufferDescriptors; }
			
			IRIS_ENGINE_API void CreateFramebuffers(IRenderer* _renderer, RenderingContext* _renderingContext, unsigned int _width, unsigned int _height);
			IRIS_ENGINE_API void RecreateFramebuffers(IRenderer* _renderer, RenderingContext* _renderingContext, unsigned int _width, unsigned int _height);
			IRIS_ENGINE_API void DeleteFramebuffers(RenderingContext* _renderingContext);

			/// <returns>The far value</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE float GetFar() const { return m_Far; }

			/// <returns>The view matrix (const version)</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE Matrix4 GetView() const { return m_ViewMatrix; }

			/// <returns>The projection matrix according to the boolean m_Ortho (const version)</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE Matrix4 GetProjection() const { return m_ProjectionMatrix; }

			/// <summary>
			/// Set the projection matrix between ortho and pespective
			/// </summary>
			IRIS_ENGINE_API IE_FORCE_INLINE void SetIsOrtho(bool _ortho) { m_Ortho = _ortho; m_RecalculateP = true; }

			/// <returns>If ortho or not</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE bool GetIsOrtho() const { return m_Ortho; }

			/// <returns>The view matrix</returns>
			IRIS_ENGINE_API Matrix4 GetViewMatrix();

			/// <returns>The projection matrix according to the boolean m_Ortho </returns>
			IRIS_ENGINE_API Matrix4 GetProjectionMatrix();

			/// <returns>The view * projection matrix</returns>
			IRIS_ENGINE_API Matrix4 GetVPMatrix();
			
			/// <summary>
			/// Movement to apply to the camera by the editor
			/// </summary>
			IRIS_ENGINE_API void MoveCallback(Vector3 _move);

			/// <summary>
			/// Rotation to apply to the camera by the editor
			/// </summary>
			IRIS_ENGINE_API void RotateCallback(Vector2 _rotate);

			/// <summary>
			/// Bind camera to render
			/// </summary>
			IRIS_ENGINE_API void Bind(IRenderer& _renderer);

			/// <returns>The camera descriptors</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE std::vector<Engine::IDescriptor*> GetDescriptors() const { return m_CamDescriptors; }

			/// <returns>The forward vector from the camera's point of view</returns>
			IRIS_ENGINE_API Vector3 GetForward();
			IE_FORCE_INLINE Vector3* GetForwardPtr() { GetForward(); return &m_Forward; }

			/// <returns>If this camera is the editor camera</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE bool IsEditorCam() const { return m_IsEditorCam; }

			/// <summary>
			/// Sets camera as the editor camera (reset rotation)
			/// </summary>
			void SetEditorCam();

			/// <returns>If this camera is the main game camera</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE bool IsMainGameCam() const { return m_IsMainCam; }

			/// <summary>
			/// Sets camera as the main game camera
			/// <para>Called by camera system</para>
			/// </summary>
			void SetMainGameCam(bool _mainGameCamera);


			IRIS_ENGINE_API IE_FORCE_INLINE void SetLockPositionInWindow(Vector2 _pos) { m_LockPositionInWindow = _pos; }
			IRIS_ENGINE_API IE_FORCE_INLINE Vector2 GetLockPositionInWindow() const { return m_LockPositionInWindow; }

			IRIS_ENGINE_API IE_FORCE_INLINE Vector2 GetCurrentRotate() const { return currentRotaXY; }
		private:
			Vector2 currentRotaXY = Vector2::s_Zero;

			/// <summary>
			/// Defines if this camera is the camera editor
			/// </summary>
			bool m_IsEditorCam = false;

			/// <summary>
			/// Defines the main game camera
			/// </summary>
			bool m_IsMainCam = false;

			std::vector<Engine::IBuffer*> m_CamBuffers;
			std::vector<Engine::IDescriptor*> m_CamDescriptors;

			/// <summary>
			/// Fov in radian
			/// </summary>
			float m_Fov = PI / 2.f;

			/// <summary>
			/// Aspect ratio of the viewport
			/// </summary>
			float m_AspectRatio = 16.f/9.f;

			/// <summary>
			/// Near value
			/// </summary>
			float m_Near = 0.1f;

			/// <summary>
			/// Far value
			/// </summary>
			float m_Far = 100.f;

			Vector3 m_Forward = Vector3(1.f);

			/// <summary>
			/// Ortho or perspective
			/// </summary>
			bool m_Ortho = false;

			/// <summary>
			/// Move speed of the camera
			/// </summary>
			float m_CamSpeed = 10.f;

			/// <summary>
			/// View matrix
			/// </summary>
			Matrix4 m_ViewMatrix;
			
			/// <summary>
			/// Projection matrix (ortho or perspective)
			/// </summary>
			Matrix4 m_ProjectionMatrix;

			/// <summary>
			/// Multiplication between View and Projection matrix
			/// </summary>
			Matrix4 m_VP;

			/// <summary>
			/// Need to recalculate view matrix
			/// </summary>
			bool m_RecalculateV = true;
			/// <summary>
			/// Need to recalculate projection matrix
			/// </summary>
			bool m_RecalculateP = true;
			/// <summary>
			/// Need to recalculate view * proj
			/// </summary>
			bool m_RecalculateVP = true;

			/// <summary>
			/// Direction to apply
			/// </summary>
			Vector3 m_direction = Vector3::s_Zero;

			/// <summary>
			/// Rotation to apply
			/// </summary>
			Vector2 m_Rotation = Vector2::s_Zero;

			/// <summary>
			/// Position of camera when lock
			///<para>if Vector(-1, -1) set at middle of window</para>
			/// </summary>
			Vector2 m_LockPositionInWindow = Vector2(-1.f, -1.f);

			Vector3 m_LastGlobalPos = Vector3::s_Zero;
			Quaternion m_LastGlobalRotation = Quaternion::s_Identity;
			
			std::vector<IFramebuffer*> m_CameraFramebuffers{};
			std::vector<IDescriptor*> m_FramebufferDescriptors{};
			unsigned int m_CameraFlags = 0;
		};
	}
}