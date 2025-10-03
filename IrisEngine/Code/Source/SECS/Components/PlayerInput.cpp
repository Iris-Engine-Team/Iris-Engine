#include "SECS/Components/PlayerInput.h"

#include "Core/Input/InputEnum.h"
#include "Core/Input/InputAction.h"
#include "Core/Input/InputMappingContext.h"
#include "Core/Input/InputManager.h"

#include "Core/IWindow.h"

#include "SECS/IrisObject.h"
#include "SECS/Components/RigidBody.h"
#include "SECS/Components/CameraComponent.h"
#include "SECS/Systems/CameraSystem.h"

#include <functional>
namespace Iris
{
	namespace Engine
	{
		PlayerInput::PlayerInput(CameraSystem* _camSystem, IWindow* _window)
		{
			m_CamSystem = _camSystem;
			m_Window = _window;
		}

		void PlayerInput::SetInput(InputManager* _inputManager)
		{
			m_ImcGame = new InputMappingContext();

			InputAction* action = new InputAction();
			std::function<void()> function = [this]() { this->MoveForward(); };
			action->AddRepeatedAction(function);
			m_ImcGame->AddActionToInput(action, IrisInput::IE_KEY_W);

			action = new InputAction();
			function = [this]() { this->MoveBackward(); };
			action->AddRepeatedAction(function);
			m_ImcGame->AddActionToInput(action, Engine::IrisInput::IE_KEY_S);

			action = new InputAction();
			function = [this]() { this->MoveLeft(); };
			action->AddRepeatedAction(function);
			m_ImcGame->AddActionToInput(action, Engine::IrisInput::IE_KEY_A);

			action = new InputAction();
			function = [this]() { this->MoveRight(); };
			action->AddRepeatedAction(function);
			m_ImcGame->AddActionToInput(action, Engine::IrisInput::IE_KEY_D);

			action = new InputAction();
			function = [this]() { this->Jump(); };
			action->AddPressedAction(function);
			m_ImcGame->AddActionToInput(action, Engine::IrisInput::IE_KEY_SPACE);

			_inputManager->AddInputMappingContext("IMC_Game", m_ImcGame);
		}

		void PlayerInput::Update(double _deltaTime)
		{
			_deltaTime;

			if (!m_ImcGame->IsActive())
			{
				m_ImcEnable = false;
				return;
			}
			
			ComponentHandle<CameraComponent>* camHandle = m_CamSystem->GetMainCamera();
			if (camHandle)
			{
				CameraComponent* cam = camHandle->UseComponent();
				if (cam)
				{
					if (!m_ImcEnable)
					{
						m_ImcEnable = true;
						StartCamRotate(cam);
					}

					CamRotate(cam);

					ITransform* camParentTrans = cam->GetIrisObject()->GetTransform()->GetParent();
					if (m_RotationToApply.Norm() > 0.f)
					{
						camParentTrans->SetRotation(Quaternion(m_RotationToApply[1] * Deg2Rad, Vector3::s_Up));
						cam->RotateCallback(Vector2(m_RotationToApply[0], 0.f));
					}

					RigidBody* rb = camParentTrans->GetIrisObject()->GetComponent<RigidBody>();
					if (rb)
					{
						if (m_MoveToApply.Norm() > 0.f)
						{
							Vector3 dir = m_MoveToApply.Normalize() * m_PlayerSpeed;

							Vector3 moveVector = camParentTrans->GetRight() * dir[0];
							moveVector += camParentTrans->GetForward() * dir[2] * (dir[2] < 0.f ? 0.6f : 1.f);
							moveVector += Vector3(0.f, rb->GetVelocity()[1], 0.f);

							rb->SetVelocity(moveVector);
						}
						else
						{
							rb->SetVelocity(Vector3(0.f, rb->GetVelocity()[1], 0.f));
						}

						rb->SetAngularVelocity(Vector3::s_Zero);
					}
				}
			}

			m_RotationToApply = Vector2::s_Zero;
			m_MoveToApply = Vector3::s_Zero;
		}
		
		void PlayerInput::MoveForward()
		{
			m_MoveToApply += Vector3::s_Forward;
		}
		
		void PlayerInput::MoveBackward()
		{
			m_MoveToApply += Vector3::s_Back;
		}
		
		void PlayerInput::MoveLeft()
		{
			m_MoveToApply += Vector3::s_Right;
		}
		
		void PlayerInput::MoveRight()
		{
			m_MoveToApply += Vector3::s_Left;
		}

		void PlayerInput::Jump()
		{
			ComponentHandle<CameraComponent>* camHandle = m_CamSystem->GetMainCamera();
			if (camHandle)
			{
				CameraComponent* cam = camHandle->UseComponent();
				if (cam)
				{
					ITransform* camParentTrans = cam->GetIrisObject()->GetTransform()->GetParent();
					RigidBody* rb = camParentTrans->GetIrisObject()->GetComponent<RigidBody>();
					if (rb)
					{
						rb->AddForce(Vector3::s_Up * m_JumpForce);
					}
				}
			}
		}

		void PlayerInput::StartCamRotate(CameraComponent* _cam)
		{
			Vector2 mousePos = Vector2(m_Window->GetWidth() / 2.0f, m_Window->GetHeight() / 2.0f);

			Vector2 lockpos = _cam->GetLockPositionInWindow();
			if (lockpos[0] > 0 && lockpos[1] > 0.f)
				mousePos = lockpos;

			ITransform* camTrans = _cam->GetIrisObject()->GetTransform();
			currentRotaXY = Vector2(camTrans->GetEuler()[0], camTrans->GetParent()->GetEuler()[1]);
			
			m_Window->SetCursorPosition(mousePos);
			m_Window->SetShowCursor(false);
		}

		void PlayerInput::CamRotate(CameraComponent* _cam)
		{

			Vector2 basePos = Vector2(m_Window->GetWidth() / 2.0f, m_Window->GetHeight() / 2.0f);
			Vector2 lockpos = _cam->GetLockPositionInWindow();
			if (lockpos[0] > 0 && lockpos[1] > 0.f)
				basePos = lockpos;

			Vector2 distance = (basePos - m_Window->GetCursorPosition());

			if (distance.Norm() > 1.f)
			{
				currentRotaXY.SetY(currentRotaXY[1] + distance[0] * m_MouseSensitivity);
				currentRotaXY.SetX(currentRotaXY[0] - distance[1] * m_MouseSensitivity);
				
				if (currentRotaXY[0] > 80.f)
					currentRotaXY.SetX(80.f);

				if (currentRotaXY[0] < -80.f)
					currentRotaXY.SetX(-80.f);

				m_RotationToApply = currentRotaXY;
			}

			m_Window->SetShowCursor(false);
			m_Window->SetCursorPosition(basePos);
		}
	}
}
