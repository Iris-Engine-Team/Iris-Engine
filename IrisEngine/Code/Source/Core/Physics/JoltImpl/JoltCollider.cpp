#include "Core/Physics/JoltImpl/JoltCollider.h"

#include "Jolt/Jolt.h"
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>

JPH_SUPPRESS_WARNINGS

namespace Iris
{
	namespace Engine
	{
		JoltBoxCollider::JoltBoxCollider()
		{
			p_ShapeType = PhysicShape::IE_SHAPE_BOX;
		}

		void JoltBoxCollider::Init()
		{
			Init(ShapeParam());
		}

		void JoltBoxCollider::Init(const ShapeParam& _params)
		{
			SetShapeInfo(_params);
			
			JPH::BoxShapeSettings boxSettings(JPH::Vec3Arg(1.f, 1.f, 1.f));
			boxSettings.SetEmbedded();
			m_Box = boxSettings.Create().Get();
		}

		IShapeSettings* JoltBoxCollider::GetShapeSettings()
		{
			m_settings.SetSettings(m_Box->ScaleShape(JPH::Vec3(p_Size[0], p_Size[1], p_Size[2])).Get());
			return &m_settings;
		}

		JoltSphereCollider::JoltSphereCollider()
		{
			p_ShapeType = PhysicShape::IE_SHAPE_SPHERE;
		}

		void JoltSphereCollider::Init()
		{
			Init(ShapeParam());
		}

		void JoltSphereCollider::Init(const ShapeParam& _params)
		{
			SetShapeInfo(_params);
			
			JPH::SphereShapeSettings sphereSettings(1.f);
			sphereSettings.SetEmbedded();
			m_Sphere = sphereSettings.Create().Get();
		}

		IShapeSettings* JoltSphereCollider::GetShapeSettings()
		{
			m_settings.SetSettings(m_Sphere->ScaleShape(JPH::Vec3(p_Radius, p_Radius, p_Radius)).Get()); 
			return &m_settings;
		}


		JoltCapsuleCollider::JoltCapsuleCollider()
		{
			p_ShapeType = PhysicShape::IE_SHAPE_CAPSULE;
		}

		void JoltCapsuleCollider::Init()
		{
			Init(ShapeParam());
		}

		void JoltCapsuleCollider::Init(const ShapeParam& _params)
		{
			SetShapeInfo(_params);

			JPH::CapsuleShapeSettings capsuleSettings(1.f, 1.f);
			capsuleSettings.SetEmbedded();
			m_Capsule = capsuleSettings.Create().Get();
		}

		IShapeSettings* JoltCapsuleCollider::GetShapeSettings()
		{
			m_settings.SetSettings(m_Capsule->ScaleShape(JPH::Vec3(p_Radius, p_Height, p_Radius)).Get());
			return &m_settings;
		}
	}
}