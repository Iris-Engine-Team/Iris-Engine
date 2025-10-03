#pragma once
#include "UISubWindow/ImGuiSubWindow.h"



//STD
#include <string>

namespace Iris
{
	namespace Engine
	{
		class RigidBody;
		class Collider;
	}

	namespace Editor
	{
		class PhysicsComponentSubWindow : public ImGuiSubWindow
		{
		public:
			PhysicsComponentSubWindow();
			~PhysicsComponentSubWindow() override final;

			void StartSubWindow() override;
			void CloseSubWindow() override;
			bool DrawSubWindow() override;

			void DrawRigidBodyOption();
			void DrawMotionTypeOption();

			void DrawColliderOption();
			void DrawColliderShapeOption();

			void SetRigidBodyComponent(Engine::RigidBody* _selectedRigidBody, Engine::Collider* _selectedCollider);
		private:

			Engine::RigidBody* m_CurrentRigidBody = nullptr;
			Engine::Collider* m_CurrentCollider = nullptr;

			bool m_SubWindowOpened = false;
		};
	}
}