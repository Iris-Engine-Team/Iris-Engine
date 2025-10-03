#pragma once
#include "SECS/Components/IComponent.h"
#include "Vector/Vector3.h"
#include "Matrix/Matrix4.h"

namespace Iris
{
	namespace Engine
	{
		class IPhysicsSystem;
		class RigidBody;
		class Collider;

		class Mesh;

		enum class PhysicShape :int
		{
			IE_NO_SHAPE = -1,
			IE_SHAPE_BOX = 0,
			IE_SHAPE_SPHERE = 1,
			IE_SHAPE_CAPSULE = 2,
		};

		struct JoltShapeSettings;
		/// <summary>
		/// Structure that allows abstract information from the shape to be passed to the rigidbody
		/// </summary>
		struct IShapeSettings
		{
			IE_FORCE_INLINE virtual JoltShapeSettings* CastJolt() { return nullptr; }
		};

		/// <summary>
		/// Defines the geometric characteristics of shapes
		/// </summary>
		class ShapeParam
		{

		public:
			ShapeParam() = default;
			ShapeParam(int _shapeType, float _sphereRadius, Vector3 _cubeSize, float _capsuleRadius, float _capsuleHeight);

			IRIS_ENGINE_API IE_FORCE_INLINE void InitBoxInfo(const Vector3& _size) {
				m_ShapeType = PhysicShape::IE_SHAPE_BOX;
				m_CubeSize = _size;
			}

			IRIS_ENGINE_API IE_FORCE_INLINE void InitSphereInfo(float _radius) {
				m_ShapeType = PhysicShape::IE_SHAPE_SPHERE;
				m_SphereRadius = _radius;
			}

			IRIS_ENGINE_API IE_FORCE_INLINE void InitCapsuleInfo(float _radius, float _height) {
				m_ShapeType = PhysicShape::IE_SHAPE_CAPSULE;
				m_CapsuleRadius = _radius;
				m_CapsuleHeight = _height;
			}

			IRIS_ENGINE_API IE_FORCE_INLINE void SetShapeType(PhysicShape _shape) { m_ShapeType = _shape; }
			IRIS_ENGINE_API IE_FORCE_INLINE PhysicShape GetShapeType() const { return m_ShapeType; }

			IRIS_ENGINE_API IE_FORCE_INLINE float GetSphereRadius() const { return m_SphereRadius; }

			IRIS_ENGINE_API IE_FORCE_INLINE Vector3 GetBoxSize() const { return m_CubeSize; }

			IRIS_ENGINE_API IE_FORCE_INLINE float GetCapsuleRadius() const { return m_CapsuleRadius; }
			IRIS_ENGINE_API IE_FORCE_INLINE float GetCapsuleHeight() const { return m_CapsuleHeight; }

		private:
			PhysicShape m_ShapeType = PhysicShape::IE_NO_SHAPE;

			//~~~~~~~~~~~~~~~~~~//
			//	Sphere Params	//
			//~~~~~~~~~~~~~~~~~~//
			float m_SphereRadius = 1.f;

			//~~~~~~~~~~~~~~~~~~//
			//    Cube Params	//
			//~~~~~~~~~~~~~~~~~~//
			Vector3 m_CubeSize = Vector3::s_One;

			//~~~~~~~~~~~~~~~~~~//
			//  Capsule Params	//
			//~~~~~~~~~~~~~~~~~~//
			float m_CapsuleRadius = 1.f;
			float m_CapsuleHeight = 1.f;
		};


		class IShape
		{
		public:
			IE_FORCE_INLINE virtual ~IShape()
			{
				if (p_Collider)
					delete p_Collider;
			}

			/// <summary>
			/// Writes data in the given metadata
			/// </summary>
			/// <param name="_metadata"></param>
			virtual void Serialize(IMetadata* _metadata) = 0;

			/// <summary>
			/// Init the components with the given data
			/// </summary>
			virtual void Deserialization(IMetadata* _metadata, std::vector<std::string>& _currentPath) = 0;

			/// <summary>
			/// Init a shape with the default shape param
			/// </summary>
			virtual void Init() = 0;

			/// <param name="_params">: defines the geometric characteristics of shapes</param>
			virtual	void Init(const ShapeParam& _params) = 0;

			/// <returns>Shape settings to use for the rigidbody</returns>
			virtual IShapeSettings* GetShapeSettings() = 0;

			/// <summary>
			/// Modify the geometric characteristics of the shape
			/// </summary>
			/// <param name="_params"></param>
			virtual void SetShapeInfo(const ShapeParam& _params) = 0;

			/// <returns>The geometric characteristics of the shape</returns>
			virtual ShapeParam GetShapeInfo() = 0;

			/// <returns>The shape type (like: sphere, box, ...)</returns>
			IE_FORCE_INLINE PhysicShape GetShapeType() const { return p_ShapeType; }

			/// <returns>The mesh to draw for debugging</returns>
			virtual std::vector<Mesh*> GetMesh() = 0;

			/// <returns>The model matrix to draw the collider shape</returns>
			virtual std::vector<Matrix4> GetPhysicTRS();

			/// <summary>
			/// Creates a handle of the collider to allow interaction between the shape and his collider
			/// </summary>
			/// <param name="_collider">: collider to create a handle</param>
			void SetColliderRef(Collider* _collider);

			/// <returns>Global scale of the shape</returns>
			virtual Vector3 GetScale() = 0;

		protected:
			/// <summary>
			/// Reference on the collider
			/// </summary>
			ComponentHandle<Collider>* p_Collider = nullptr;
			PhysicShape p_ShapeType = PhysicShape::IE_NO_SHAPE;
		};

		class BoxCollider : public IShape
		{
		public:
			virtual ~BoxCollider() override = default;

			/// <summary>
			/// Writes data in the given metadata
			/// </summary>
			/// <param name="_metadata"></param>
			void Serialize(IMetadata* _metadata) final override;

			/// <summary>
			/// Init the components with the given data
			/// </summary>
			void Deserialization(IMetadata* _metadata, std::vector<std::string>& _currentPath) final override;

			/// <summary>
			/// Init function. Creates a basic box (1,1,1)
			/// </summary>
			virtual	void Init() override = 0;

			/// <summary>
			/// Init function
			/// </summary>
			/// <param name="_params">: specify the size for the box</param>
			virtual	void Init(const ShapeParam& _params) override = 0;

			/// <returns>Shape settings to use for the rigidbody</returns>
			virtual IShapeSettings* GetShapeSettings() override = 0;

			/// <summary>
			/// Modify the size of the box
			/// </summary>
			/// <param name="_params"></param>
			void SetShapeInfo(const ShapeParam& _params) final override;

			/// <returns>The geometric characteristics of the shape</returns>
			ShapeParam GetShapeInfo()  final override;

			/// <returns>The default cube mesh</returns>
			std::vector<Mesh*> GetMesh() final override;

			/// <returns>Box size</returns>
			Vector3 GetScale() final override { return p_Size; }

		protected:
			/// <summary>
			/// World size of the box
			/// </summary>
			Vector3 p_Size = Vector3::s_One; 
		};

		class SphereCollider : public IShape
		{
		public:

			virtual ~SphereCollider() override = default;

			/// <summary>
			/// Writes data in the given metadata
			/// </summary>
			/// <param name="_metadata"></param>
			void Serialize(IMetadata* _metadata) final override;

			/// <summary>
			/// Init the components with the given data
			/// </summary>
			void Deserialization(IMetadata* _metadata, std::vector<std::string>& _currentPath) final override;

			/// <summary>
			/// Init function. Creates a basic sphere (radius = 1)
			/// </summary>
			virtual	void Init() = 0;

			/// <summary>
			/// Init function
			/// </summary>
			/// <param name="_params">: specify the radius for the sphere</param>
			virtual	void Init(const ShapeParam& _params) override = 0;

			/// <returns>Shape settings to use for the rigidbody</returns>
			virtual IShapeSettings* GetShapeSettings() override = 0;

			/// <summary>
			/// Modify the radius of the sphere
			/// </summary>
			/// <param name="_params"></param>
			void SetShapeInfo(const ShapeParam& _params) final override;

			/// <returns>The geometric characteristics of the shape</returns>
			ShapeParam GetShapeInfo()  final override;

			/// <returns>The default sphere mesh</returns>
			std::vector<Mesh*> GetMesh() final override;

			/// <returns>Sphere scale based on the radius</returns>
			Vector3 GetScale() final override { return Vector3(p_Radius); }

		protected:

			/// <summary>
			/// World radius of the sphere
			/// </summary>
			float p_Radius = 1.f;
		};

		class CapsuleCollider : public IShape
		{
		public:

			virtual ~CapsuleCollider() override = default;

			/// <summary>
			/// Writes data in the given metadata
			/// </summary>
			/// <param name="_metadata"></param>
			void Serialize(IMetadata* _metadata) final override;

			/// <summary>
			/// Init the components with the given data
			/// </summary>
			void Deserialization(IMetadata* _metadata, std::vector<std::string>& _currentPath) final override;

			/// <summary>
			/// Init function. Creates a basic capsule (radius = 1, height = 1)
			/// </summary>
			virtual	void Init() = 0;

			/// <summary>
			/// Init function
			/// </summary>
			/// <param name="_params">: specify the radius and the height for the capsule</param>
			virtual	void Init(const ShapeParam& _params) override = 0;

			/// <returns>Shape settings to use for the rigidbody</returns>
			virtual IShapeSettings* GetShapeSettings() override = 0;

			/// <summary>
			/// Modify the radius and the height of the capsule
			/// </summary>
			/// <param name="_params"></param>
			void SetShapeInfo(const ShapeParam& _params) final override;

			/// <returns>The geometric characteristics of the shape</returns>
			ShapeParam GetShapeInfo()  final override;

			/// <returns>The default capsule mesh</returns>
			std::vector<Mesh*> GetMesh() final override;

			/// <returns>The model matrix to draw the collider shape</returns>
			std::vector<Matrix4> GetPhysicTRS() final override;

			/// <returns>Capsule scale based on the radius and the height</returns>
			Vector3 GetScale() final override { return Vector3(p_Radius, p_Height, p_Radius); }

		protected:

			/// <summary>
			/// World radius of the capsule
			/// </summary>
			float p_Radius = 1.f;

			/// <summary>
			/// World height of the capsule
			/// </summary>
			float p_Height = 2.f;
		};

		class Collider : public IComponent
		{
		public:

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
			//							Component DEF							 //
			DEFINE_COMPONENT(Collider);											 //
			static IE_FORCE_INLINE std::string GetName() { return "Collider"; }	 //
			static IE_FORCE_INLINE ComponentID GetID() { return ComponentID::IE_COLLIDER; }			 //
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

			/// <summary>
			/// Writes data in the given metadata
			/// </summary>
			/// <param name="_metadata"></param>
			void Serialize(IMetadata* _metadata) final override;

			/// <summary>
			/// Init the components with the given data
			/// </summary>
			void Deserialization(IMetadata* _metadata, InitEditorContext& _initializer) final override;

			/// <summary>
			/// Init function, creates a shape and link the collider to the rigidbody
			/// </summary>
			/// <param name="_physicsSystem">: allows to instantiate the wanted shape</param>
			/// <param name="_shapeType">: shape you want to instantiate</param>
			/// <param name="_params">: geometric characteristics of shapes</param>
			IRIS_ENGINE_API void Init(IPhysicsSystem* _physicsSystem, PhysicShape _shapeType, const ShapeParam& _params);

			/// <summary>
			/// Init function, creates a box and link the collider to the rigidbody
			/// </summary>
			/// <param name="_physicsSystem">: allows to instantiate the wanted shape</param>
			IRIS_ENGINE_API void Init(IPhysicsSystem* _physicsSystem);

			/// <summary>
			/// Init function called from the editor
			/// </summary>
			/// <param name="_initializer">: allows to fetch the physics system</param>
			IRIS_ENGINE_API void Init(InitEditorContext& _initializer) override;

			/// <summary>
			/// Copy the given component, (after the operation you can't use the given component)
			/// </summary>
			/// <param name="_componentToCopy"></param>
			void Copy(IComponent* _componentToCopy) override;

			
			/// <param name="_componentToDuplicate">: component used to duplicate</param>
			/// <param name="_initializer">: allows to init the new collider</param>
			IRIS_ENGINE_API void Duplicate(IComponent* _componentToDuplicate, InitEditorContext* _initializer) final override;
			
			/// <summary>
			/// Destroy function
			/// </summary>
			void Destroy() override;

			/// <returns>The physic shape</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE IShape* GetShape() { return m_Shape; }

			/// <summary>
			/// Modify the shape type and the geometry of the shape
			/// <para>(if the physic shape is the same, it will modify the geometry of the current shape)</para>
			/// </summary>
			/// <param name="_colliderType">: shape type</param>
			/// <param name="_params">: geometric characteristics</param>
			IRIS_ENGINE_API void SetShapeInfo(PhysicShape _colliderType, const ShapeParam& _params);

			/// <summary>
			/// Modify the geometry of the current shape
			/// </summary>
			/// <param name="_params">: geometric characteristics</param>
			IRIS_ENGINE_API void SetShapeInfo(const ShapeParam& _params);

			/// <returns>The geometric characteristics of shapes</returns>
			IRIS_ENGINE_API ShapeParam* GetShapeInfo();
			
			/// <returns>The mesh to draw for debugging</returns>
			std::vector<Mesh*> GetMesh();

			/// <returns>The model matrix to draw the collider shape</returns>
			std::vector<Matrix4> GetPhysicTRS();

			/// <returns>The linked rigidbody (nullptr if no link)</returns>
			RigidBody* GetRigidBody();

			/// <summary>
			/// Creates a handle of the RigidBody
			/// <para>if the rigidbody he doesn't have the reference on the collider, it will link the collider to the rigidbody </para>
			/// </summary>
			/// <param name="_rigidBody">: rigid body to link</param>
			void LinkRigidBody(RigidBody* _rigidBody);

			/// <summary>
			/// Notify the rigidbody that it needs to update its internal shape
			/// </summary>
			void NotifyColliderChanges();

			/// <summary>
			/// Set the local offset position of the collider
			/// </summary>
			/// <param name="_offset">: position offset</param>
			IRIS_ENGINE_API IE_FORCE_INLINE void SetOffSet(const Vector3& _offset) { m_PosOffSet = _offset; }

			/// <returns>The offset position</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE Vector3 GetOffSet() { return m_PosOffSet; }
			IE_FORCE_INLINE Vector3* GetOffSetPtr() { GetOffSet();  return &m_PosOffSet; }

			/// <param name="_transform"></param>
			/// <returns>The position with the local offset of the collider</returns>
			IRIS_ENGINE_API Vector3 GetPhysicPositionWithOffSet(ITransform* _transform);
			
		private:
			/// <returns>The local model matrix of the collider</returns>
			Matrix4 GetTRSOffSet();
		
		private:
			IShape* m_Shape = nullptr;

			IPhysicsSystem* m_PhysicsSystem = nullptr;

			/// <summary>
			/// Reference on the rigidbody
			/// </summary>
			ComponentHandle<RigidBody>* m_RigidBody = nullptr;

			/// <summary>
			/// Local offset of the collider
			/// </summary>
			Vector3 m_PosOffSet = Vector3::s_Zero;

			ShapeParam m_Params;
		};

	}
}