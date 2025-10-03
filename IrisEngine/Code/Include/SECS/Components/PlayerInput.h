#pragma once
#include "Vector/Vector3.h"
#include "Vector/Vector2.h"
#include "Matrix/Matrix4.h"

namespace Iris
{
	namespace Engine
	{
		class ITransform;
		class CameraComponent;
		class InputMappingContext;
		class InputManager;
		class CameraSystem;
		class IWindow;


		//input in cpp and not in c#
		//we will manipulate the parent of the main game camera
		class PlayerInput
		{
		public:
			PlayerInput(CameraSystem* _camSystem, IWindow* _window);
			void SetInput(InputManager* _inputManager);
			void Update(double _deltaTime);

		private:
			void MoveForward();
			void MoveBackward();
			void MoveLeft();
			void MoveRight();
			void Jump();

			void StartCamRotate(CameraComponent* _cam);
			void CamRotate(CameraComponent* _cam);

		private:
			CameraSystem* m_CamSystem = nullptr;
			Vector3 m_MoveToApply = Vector3::s_Zero;
			Vector2 m_RotationToApply = Vector2::s_Zero;

			bool m_Jumped = false;

			IWindow* m_Window = nullptr;

			float m_MouseSensitivity = 0.05f;
			float m_PlayerSpeed = 5.f;
			float m_JumpForce = 350.f;

			InputMappingContext* m_ImcGame = nullptr;

			bool m_ImcEnable = false;

			Vector2 currentRotaXY = Vector2::s_Zero;
		};
	}
}