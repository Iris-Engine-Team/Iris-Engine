#pragma once
#include "SECS/Components/Collider.h"
#include "Jolt/Jolt.h"
#include <Jolt/Physics/Collision/Shape/Shape.h>

namespace Iris
{
	namespace Engine
	{
		struct JoltShapeSettings : public IShapeSettings
		{
		public:

			/// <returns>Returns a pointer to the current JoltShapeSettings object</returns>
			IE_FORCE_INLINE JoltShapeSettings* CastJolt() override { return this; }

			/// <summary>
			/// Sets the shape settings for the Jolt shape
			/// </summary>
			/// <param name="_settings">: the shape reference to be set</param>
			IE_FORCE_INLINE void SetSettings(JPH::ShapeRefC _settings) { m_Settings = _settings; }

			/// <returns>Returns the current shape settings</returns>
			IE_FORCE_INLINE JPH::ShapeRefC GetSettings() { return m_Settings; }

		private:
			/// <summary>
			/// The shape settings that define the geometry of the Jolt shape
			/// </summary>
			JPH::ShapeRefC m_Settings = nullptr;
		};

		class JoltBoxCollider :public BoxCollider
		{
		public:

			JoltBoxCollider();
			~JoltBoxCollider() override = default;
			
			/// <summary>
			/// Init function. Creates a basic box (1,1,1)
			/// </summary>
			void Init() final override;

			/// <summary>
			/// Init function
			/// </summary>
			/// <param name="_params">: specify the size for the box</param>
			void Init(const ShapeParam& _params) final override;
			
			/// <returns>Shape settings to use for the rigidbody</returns>
			IShapeSettings* GetShapeSettings() final override;
		
		private:

			/// <summary>
			/// Reference to the Jolt box shape
			/// </summary>
			JPH::ShapeRefC m_Box;
			
			JoltShapeSettings m_settings;
		};

		class JoltSphereCollider :public SphereCollider
		{
		public:
			
			JoltSphereCollider();
			~JoltSphereCollider() override = default;

			/// <summary>
			/// Init function. Creates a basic sphere (radius = 1)
			/// </summary>
			void Init() final override;

			/// <summary>
			/// Init function
			/// </summary>
			/// <param name="_params">: specify the radius for the sphere</param>
			void Init(const ShapeParam& _params) final override;

			/// <returns>Shape settings to use for the rigidbody</returns>
			IShapeSettings* GetShapeSettings() final override;

		private:

			/// <summary>
			/// Reference to the Jolt box shape
			/// </summary>
			JPH::ShapeRefC m_Sphere;

			JoltShapeSettings m_settings;
		};

		class JoltCapsuleCollider :public CapsuleCollider
		{
		public:

			JoltCapsuleCollider();
			~JoltCapsuleCollider() override = default;

			/// <summary>
			/// Init function. Creates a basic sphere (radius = 1)
			/// </summary>
			void Init() final override;

			/// <summary>
			/// Init function
			/// </summary>
			/// <param name="_params">: specify the radius for the sphere</param>
			void Init(const ShapeParam& _params) final override;

			/// <returns>Shape settings to use for the rigidbody</returns>
			IShapeSettings* GetShapeSettings() final override;

		private:

			/// <summary>
			/// Reference to the Jolt box shape
			/// </summary>
			JPH::ShapeRefC m_Capsule;

			JoltShapeSettings m_settings;
		};
	}
}