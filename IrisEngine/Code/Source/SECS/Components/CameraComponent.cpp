#include "SECS/Components/CameraComponent.h"
#include "SECS/IrisObject.h"
#include "SECS/Components/Transform.h"
#include "Core/Application.h"
#include "Core/GLFWWindow.h"
#include "Core/Tools/Log.h"

#include "Core/Graphics/Renderer/IRenderer.h"
#include "Core/Graphics/Context/ResourceContext.h"
#include "Core/Graphics/Context/RenderingContext.h"
#include "Core/Graphics/RHI/ISwapChain.h"
#include "Core/Graphics/RHI/IPipeline.h"

#include "Resources/Metadata.h"

namespace Iris
{
	namespace Engine
	{
		void CameraComponent::Serialize(IMetadata* _metadata)
		{
			_metadata->Write("Enable", p_Enable);

			_metadata->Write("Editor Cam", m_IsEditorCam);

			_metadata->Write("Main Game Cam", m_IsMainCam);

			_metadata->Write("FOV", m_Fov);

			_metadata->Write("Aspect Ratio", m_AspectRatio);

			_metadata->Write("Near", m_Near);

			_metadata->Write("Far", m_Far);

			_metadata->Write("Is Ortho", m_Ortho);

			_metadata->Write("Speed", m_CamSpeed);

			_metadata->Write("Camera Flags", m_CameraFlags);
		}

		void CameraComponent::Deserialization(IMetadata* _metadata, InitEditorContext& _initializer)
		{
			std::vector<std::string> path = { "Scene Data", "Components", std::to_string(static_cast<uint16_t>(GetID())), GetName(), std::to_string(p_IrisObject->GetID()), "Parent" };

			path[5] = "Editor Cam";
			m_IsEditorCam = _metadata->GetData(path);

			path[5] = "Main Game Cam";
			m_IsMainCam = _metadata->GetData(path);

			path[5] = "FOV";
			m_Fov = _metadata->GetData(path);

			path[5] = "Aspect Ratio";
			m_AspectRatio = _metadata->GetData(path);

			path[5] = "Near";
			m_Near = _metadata->GetData(path);

			path[5] = "Far";
			m_Far = _metadata->GetData(path);

			path[5] = "Is Ortho";
			m_Ortho = _metadata->GetData(path);

			path[5] = "Speed";
			m_CamSpeed = _metadata->GetData(path);

			path[5] = "Camera Flags";
			m_CameraFlags = _metadata->GetData(path);

			path[5] = "Enable";
			p_Enable = _metadata->GetData(path);

			m_CamBuffers.resize(MAX_FRAMES_IN_FLIGHT);
			m_CamDescriptors.resize(MAX_FRAMES_IN_FLIGHT);

			CamDataUBO camData{};
			m_CamBuffers = _initializer.InitResourceContext->CreateBuffers(Engine::RHIBufferType::IE_RHI_UNIFORM_BUFFER, sizeof(Engine::CamDataUBO), &camData);
			m_CamDescriptors = _initializer.InitResourceContext->CreateBufferDescriptors(_initializer.InitPipeline->GetDescriptorLayout(_initializer.InitRenderer->GetCameraLayoutIndex()), m_CamBuffers, sizeof(Engine::CamDataUBO), _initializer.InitRenderer->GetCameraLayoutIndex());
	
			CreateFramebuffers(_initializer.InitRenderer, _initializer.InitRenderingContext, 1920, 1080);
		}

		void CameraComponent::Init(IRenderer& _renderer, ResourceContext& _resourceContext, RenderingContext& _renderingContext, const Vector3& _position, float _fov, float _aspect, float _near, float _far, bool _ortho)
		{
			p_IrisObject->GetTransform()->SetPosition(_position);

			SetFov(_fov);
			SetAspect(_aspect);
			SetNear(_near);
			SetFar(_far);
			SetIsOrtho(_ortho);

			m_CamBuffers.resize(MAX_FRAMES_IN_FLIGHT);
			m_CamDescriptors.resize(MAX_FRAMES_IN_FLIGHT);

			CamDataUBO camData{};
			camData.ProjectionView = GetVPMatrix();
			camData.CameraPosition = Vector4(_position[0], _position[1], _position[2], 1.f);

			m_CamBuffers = _resourceContext.CreateBuffers(Engine::RHIBufferType::IE_RHI_UNIFORM_BUFFER, sizeof(Engine::CamDataUBO), &camData);
			m_CamDescriptors = _resourceContext.CreateBufferDescriptors(_renderer.GetMainPipeline()->GetDescriptorLayout(_renderer.GetCameraLayoutIndex()), m_CamBuffers, sizeof(Engine::CamDataUBO), _renderer.GetCameraLayoutIndex());
			
			SetDrawSkybox(true);
			CreateFramebuffers(&_renderer, &_renderingContext, 1920, 1080);
		}

		void CameraComponent::Init(IRenderer& _renderer, ResourceContext& _resourceContext, RenderingContext& _renderingContext)
		{
			m_RecalculateP = true;
			m_RecalculateV = true;


			m_CamBuffers.resize(MAX_FRAMES_IN_FLIGHT);
			m_CamDescriptors.resize(MAX_FRAMES_IN_FLIGHT);

			Vector3 pos = p_IrisObject->GetTransform()->GetGlobalPosition();

			CamDataUBO camData{};
			camData.ProjectionView = GetVPMatrix();
			camData.CameraPosition = Vector4(pos[0], pos[1], pos[2], 1.f);

			m_CamBuffers = _resourceContext.CreateBuffers(Engine::RHIBufferType::IE_RHI_UNIFORM_BUFFER, sizeof(Engine::CamDataUBO), &camData);
			m_CamDescriptors = _resourceContext.CreateBufferDescriptors(_renderer.GetMainPipeline()->GetDescriptorLayout(_renderer.GetCameraLayoutIndex()), m_CamBuffers, sizeof(Engine::CamDataUBO), _renderer.GetCameraLayoutIndex());

			SetDrawSkybox(true);
			CreateFramebuffers(&_renderer, &_renderingContext, 1920, 1080);
		}

		void CameraComponent::Init(InitEditorContext& _initializer)
		{
			Init(*_initializer.InitRenderer, *_initializer.InitResourceContext, *_initializer.InitRenderingContext);
		}

		void CameraComponent::Copy(IComponent* _componentToCopy)
		{
			CameraComponent* cam = dynamic_cast<CameraComponent*>(_componentToCopy);
			if (cam)
			{
				IComponent::Copy(_componentToCopy);

				p_IrisObject->GetTransform()->SetPosition(cam->GetIrisObject()->GetTransform()->GetPosition());

				SetFov(cam->GetFov());
				SetAspect(cam->GetAspect());
				SetNear(cam->GetNear());
				SetFar(cam->GetFar());
				SetIsOrtho(cam->GetIsOrtho());
				m_IsEditorCam = cam->m_IsEditorCam;
				m_IsMainCam = cam->m_IsMainCam;

				ITransform* tCam = cam->GetIrisObject()->GetTransform();
				p_IrisObject->GetTransform()->SetRotation(tCam->GetRotation());
				m_direction = cam->m_direction;
				m_Rotation = cam->m_Rotation;

				m_CamSpeed = cam->m_CamSpeed;
				m_ViewMatrix = GetViewMatrix();
				m_ProjectionMatrix = GetProjectionMatrix();

				m_CamBuffers = cam->m_CamBuffers;
				m_CamDescriptors = cam->m_CamDescriptors;
				m_CameraFlags = cam->m_CameraFlags;
				m_CameraFramebuffers = cam->m_CameraFramebuffers;
				m_FramebufferDescriptors = cam->m_FramebufferDescriptors;
			}
			else
			{
				DEBUG_LOG("cant copy this component ");
			}
		}

		void CameraComponent::Duplicate(IComponent* _componentToDuplicate, InitEditorContext* _initializer)
		{
			CameraComponent* cam = dynamic_cast<CameraComponent*>(_componentToDuplicate);
			if (cam)
			{
				IComponent::Duplicate(_componentToDuplicate, _initializer);

				Init(*_initializer->InitRenderer, *_initializer->InitResourceContext, *_initializer->InitRenderingContext
					, cam->GetIrisObject()->GetTransform()->GetGlobalPosition()
					, m_Fov, m_AspectRatio, m_Near, m_Far, m_Ortho);

				m_direction = cam->m_direction;
				m_Rotation = cam->m_Rotation;

				m_CamSpeed = cam->m_CamSpeed;

				m_IsEditorCam = cam->m_IsEditorCam;
				m_IsMainCam = cam->m_IsMainCam;
			}
		}

		void CameraComponent::DeleteBuffers(ResourceContext& _resourceContext, RenderingContext& _renderingContext)
		{
			_resourceContext.DestroyBuffers(m_CamBuffers);
			_resourceContext.DeleteDescriptors(m_CamDescriptors);
			_resourceContext.DeleteDescriptors(m_FramebufferDescriptors);

			DeleteFramebuffers(&_renderingContext);
		}
	
		void CameraComponent::Update(double _deltaTime)
		{
			ITransform* camTrans = p_IrisObject->GetTransform();
			
			if (m_Rotation.Norm() > 0.f)
			{
				camTrans->SetRotation(Quaternion(m_Rotation[1] * Deg2Rad, Vector3::s_Up) * Quaternion(m_Rotation[0] * Deg2Rad, Vector3::s_Right) );

				m_LastGlobalRotation = camTrans->GetGlobalRotation();
				m_RecalculateV = true;
			}

			if (m_direction.Norm() > 0.f)
			{
				Vector3 dir = m_direction.Normalize() * m_CamSpeed * static_cast<float>(_deltaTime);
				camTrans->SetPosition(camTrans->GetPosition() 
					- camTrans->GetRight() * dir[0] 
					+ camTrans->GetUp() * dir[1] 
					+ camTrans->GetForward() * dir[2]);

				m_LastGlobalPos = camTrans->GetPosition();
				m_RecalculateV = true;
			}

			m_direction = Vector3(0,0,0);
			m_Rotation = Vector2(0,0);
		}

		void CameraComponent::CheckTransformChange()
		{
			ITransform* camTrans = p_IrisObject->GetTransform();

			Vector3 globalPos = camTrans->GetGlobalPosition();
			Quaternion globalRota = camTrans->GetGlobalRotation();

			if (globalPos != m_LastGlobalPos || globalRota != m_LastGlobalRotation)
			{
				m_LastGlobalPos = globalPos;
				m_LastGlobalRotation = globalRota;
				m_RecalculateV = true;
			}	
		}
		
		void CameraComponent::CreateFramebuffers(IRenderer* _renderer, RenderingContext* _renderingContext, unsigned int _width, unsigned int _height)
		{
			m_CameraFramebuffers = _renderingContext->CreateFramebuffers(_renderer->GetMainRenderPass(), _width, _height, _renderer->GetMainAttachments());
			m_FramebufferDescriptors = _renderer->OnFramebufferResize(m_CameraFramebuffers, m_FramebufferDescriptors);
		}

		void CameraComponent::RecreateFramebuffers(IRenderer* _renderer, RenderingContext* _renderingContext, unsigned int _width, unsigned int _height)
		{
			_renderingContext->DestroyFramebuffers(m_CameraFramebuffers);
			m_CameraFramebuffers = _renderingContext->CreateFramebuffers(_renderer->GetMainRenderPass(), _width, _height, _renderer->GetMainAttachments());
			m_FramebufferDescriptors = _renderer->OnFramebufferResize(m_CameraFramebuffers, m_FramebufferDescriptors);
		}

		void CameraComponent::DeleteFramebuffers(RenderingContext* _renderingContext)
		{
			_renderingContext->DestroyFramebuffers(m_CameraFramebuffers);
		}

		Matrix4 CameraComponent::GetViewMatrix()
		{
			if (m_RecalculateV)
			{
				ITransform* t = p_IrisObject->GetTransform();
				Vector3 camPos = t->GetGlobalPosition();

				m_ViewMatrix = Matrix4::LookAtMatrix(camPos, camPos + t->GetForward(), Vector3(0.f, 1.f, 0.f));
				m_RecalculateV = false;
				m_RecalculateVP = true;
			}

			return m_ViewMatrix;
		}

		Matrix4 CameraComponent::GetProjectionMatrix()
		{
			if (m_RecalculateP)
			{
				//a check
				if (m_Ortho)
					m_ProjectionMatrix = Matrix4::ProjectionOrthographicMatrix(m_Near, m_Far, 100.f, 100.f, 100.f, 100.f);
				else
					m_ProjectionMatrix = Matrix4::ProjectionPerspectiveMatrix(m_Near, m_Far, m_AspectRatio, m_Fov);
				m_RecalculateP = false;
				m_RecalculateVP = true;
			}
			return m_ProjectionMatrix;
		}

		Matrix4 CameraComponent::GetVPMatrix()
		{
			if (m_RecalculateVP || m_RecalculateV || m_RecalculateP)
			{
				m_VP = GetProjectionMatrix() * GetViewMatrix();
				m_RecalculateVP = false;
			}
			return m_VP;
		}

		void CameraComponent::MoveCallback(Vector3 _move)
		{
			m_direction += _move;
		}

		void CameraComponent::RotateCallback(Vector2 _rotate)
		{
			m_Rotation += _rotate;
			currentRotaXY = _rotate;
		}

		void CameraComponent::Bind(IRenderer& _renderer)
		{
			Vector3 camPosition = GetIrisObject()->GetTransform()->GetGlobalPosition();

			CamDataUBO camData{};
			camData.ProjectionView = GetVPMatrix();
			camData.Projection = GetProjection();
			camData.View = GetView();
			camData.CameraPosition = Vector4(camPosition[0], camPosition[1], camPosition[2], 1.f);

			_renderer.UpdateBuffer(m_CamBuffers, &camData, sizeof(camData));
		}

		Vector3 CameraComponent::GetForward()
		{
			Vector4 fwr = p_IrisObject->GetTransform()->GetGlobalTRS().GetRow(2);
			m_Forward = Vector3(fwr[0], fwr[1], fwr[2]).Normalize();
			return m_Forward;
		}

		void CameraComponent::SetEditorCam()
		{
			 m_IsEditorCam = true;

			 m_Rotation = Vector2::s_Zero;
			 p_IrisObject->GetTransform()->SetEuler(Vector3::s_Zero);

			 m_RecalculateV = true;
		}

		void CameraComponent::SetMainGameCam(bool _mainGameCamera)
		{
			m_IsMainCam = _mainGameCamera;
		}
	}
}