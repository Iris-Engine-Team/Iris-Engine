#include "SECS/Systems/CameraSystem.h"

#include "Core/Graphics/RHI/IBuffer.h"
#include "Core/Graphics/RHI/IDescriptor.h"
#include "Core/Graphics/RHI/ISwapChain.h"
#include "Core/Graphics/Renderer/IRenderer.h"
#include "Core/Graphics/ContextInitializer.h"

namespace Iris
{
	namespace Engine
	{
		CameraSystem::CameraSystem(Scene* _scene)
		{
			SetScene(_scene);
		}

		void CameraSystem::SetScene(Scene* _scene)
		{
			if (!_scene)
				return;

			m_SceneRef = _scene;

			if(m_SceneRef->ContainTable<CameraComponent>())
				m_Cameras = m_SceneRef->GetComponentTable<CameraComponent>();
			else
				m_Cameras = m_SceneRef->AddTable<CameraComponent>();
		}

		std::vector<CameraComponent>& CameraSystem::GetAllCameras()
		{
			return m_Cameras->GetAllComponents();
		}

		void CameraSystem::BindCamera(CameraComponent* _cam, IRenderer* _renderer)
		{
			_cam->Bind(*_renderer);
		}

		void CameraSystem::Destroy(ResourceContext& _resourceContext, RenderingContext& _renderingContext)
		{
			if (m_CurrentGameCamera)
			{
				delete m_CurrentGameCamera;
				m_CurrentGameCamera = nullptr;

			}

			if (m_CurrentEditorCamera)
			{
				delete m_CurrentEditorCamera;
				m_CurrentEditorCamera = nullptr;
			}

			if (m_Cameras)
			{
				std::vector<CameraComponent>& cams = m_Cameras->GetAllComponents();

				size_t size = cams.size();
				for (size_t i = 0; i < size; ++i)
					cams[i].DeleteBuffers(_resourceContext, _renderingContext);
				m_Cameras = nullptr;
			}

			m_SceneRef = nullptr;
		}
		
		void CameraSystem::Clear(ResourceContext& _resourceContext, RenderingContext& _renderingContext, bool _ignoreEditorCamera)
		{
			if (m_CurrentGameCamera)
			{
				delete m_CurrentGameCamera;
				m_CurrentGameCamera = nullptr;

			}

			if (!_ignoreEditorCamera && m_CurrentEditorCamera)
			{
				delete m_CurrentEditorCamera;
				m_CurrentEditorCamera = nullptr;
			}

			if (m_Cameras)
			{
				std::vector<CameraComponent>& cams = m_Cameras->GetAllComponents();

				size_t size = cams.size();
				for (size_t i = 0; i < size; ++i)
				{
					if (cams[i].IsEditorCam() && _ignoreEditorCamera)
						continue;

					cams[i].DeleteBuffers(_resourceContext, _renderingContext);
				}

				m_Cameras = nullptr;
			}

			m_SceneRef = nullptr;
		}

		void CameraSystem::Update()
		{
			std::vector<CameraComponent>& cams = m_Cameras->GetAllComponents();

			size_t size = cams.size();
			for (size_t i = 0; i < size; ++i)
				cams[i].CheckTransformChange();
		}

		void CameraSystem::SetMainCamera(CameraComponent* _camera)
		{
			if (_camera)
			{
				if (_camera->IsEditorCam())
					return;

				if (m_CurrentGameCamera)
				{
					CameraComponent* cam = m_CurrentGameCamera->UseComponent();
					if (cam)
						cam->SetMainGameCam(false);
					delete m_CurrentGameCamera;
				}

				m_CurrentGameCamera = new ComponentHandle<CameraComponent>(_camera);
				_camera->SetMainGameCam(true);
			}
			else
			{
				if (m_CurrentGameCamera)
				{
					m_CurrentGameCamera->UseComponent()->SetMainGameCam(false);
					delete m_CurrentGameCamera;
					m_CurrentGameCamera = nullptr;
				}
			}
		}

		void CameraSystem::SetEditorCamera(CameraComponent* _camera)
		{
			if (_camera->IsMainGameCam())
				return;

			if (m_CurrentEditorCamera)
				delete m_CurrentEditorCamera;
			m_CurrentEditorCamera = new ComponentHandle<CameraComponent>(_camera);
			_camera->SetEditorCam();
		}
	}
}
