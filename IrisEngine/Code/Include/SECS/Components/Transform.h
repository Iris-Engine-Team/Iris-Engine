#pragma once
#include "IComponent.h"

#include <string>
#include <vector>
#include "Vector/Vector3.h"
#include "Quaternion/Quaternion.h"
#include "Matrix/Matrix4.h"

namespace Iris
{
	namespace Engine
	{
		class IComponentHandle;

		template<class T>
		class ComponentHandle;

		class IrisObject;

		class ITransform : public IComponent
		{
		public:

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
			//							Component DEF							 //
			DEFINE_COMPONENT(ITransform);										 //
			static IE_FORCE_INLINE std::string GetName() { return "Transform"; } //
			static IE_FORCE_INLINE ComponentID GetID() { return ComponentID::IE_TRANSFORM; }			 //
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

			/// <summary>
			/// Creates a default transform
			/// </summary>
			/// <param name="_parent">: transform to set as parent</param>
			IRIS_ENGINE_API ITransform();

			/// <summary>
			/// Copy the information of the given transform without copying the children
			/// </summary>
			/// <param name="_transform">: transform to copy</param>
			/// <param name="_irisObject">: owner object</param>
			IRIS_ENGINE_API ITransform(const ITransform& _transform, IrisObject* _irisObject);
			IRIS_ENGINE_API ~ITransform() override;

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
			/// Creates a transform with the given position
			/// </summary>
			/// <param name="_position">: world position</param>
			/// <param name="_parent">: transform to set as parent</param>
			IRIS_ENGINE_API void Init(const Vector3& _position, ITransform* _parent = nullptr);

			/// <summary>
			/// Creates a transform with the given position, rotation and scale
			/// </summary>
			/// <param name="_position">: world position</param>
			/// <param name="_rotation">: world rotation in quaternion</param>
			/// <param name="_scale">: world scale</param>
			/// <param name="_parent">: transform to set as parent</param>
			IRIS_ENGINE_API void Init(const Vector3& _position, const Quaternion& _rotation, const Vector3& _scale, ITransform* _parent = nullptr);

			/// <summary>
			/// Creates a transform with the given position, rotation and scale
			/// </summary>
			/// <param name="_position">: world position</param>
			/// <param name="_rotation">: world euler angles in degrees</param>
			/// <param name="_scale">: world scale</param>
			/// <param name="_parent">: transform to set as parent</param>
			IRIS_ENGINE_API void Init(const Vector3& _position, const Vector3& _rotation, const Vector3& _scale, ITransform* _parent = nullptr);

			IRIS_ENGINE_API IE_FORCE_INLINE void Init(InitEditorContext& _initializer) override { _initializer; }

			void Copy(IComponent* _componentToCopy) override;
			void Destroy() final override;

			IRIS_ENGINE_API void SetEnable(bool _enable) override;

			/// <summary>
			/// Set a new local position
			/// </summary>
			/// <param name="_position">: new local position to set</param>
			/// <returns>The new local position</returns>
			IRIS_ENGINE_API Vector3 SetPosition(const Vector3& _position);

			/// <summary>
			/// 
			/// </summary>
			/// <returns>The local position</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE Vector3 GetPosition() const { return m_Position; }
			IE_FORCE_INLINE Vector3* GetPositionPtr() { return &m_Position; }

			/// <summary>
			/// Set the global position
			/// </summary>
			/// <param name="_position">: new global position to set</param>
			/// <returns>The new local position</returns>
			IRIS_ENGINE_API Vector3 SetGlobalPosition(const Vector3& _position);

			/// <summary>
			/// 
			/// </summary>
			/// <returns>The global position</returns>
			IRIS_ENGINE_API Vector3 GetGlobalPosition();
			IE_FORCE_INLINE Vector3* GetGlobalPositionPtr() { GetGlobalPosition();  return &m_GlobalPosition; }


			/// <summary>
			/// Set a new local scale
			/// </summary>
			/// <param name="_scale">: new local scale to set</param>
			/// <returns>The new local scale</returns>
			IRIS_ENGINE_API Vector3 SetScale(const Vector3& _scale);

			/// <returns>The local scale</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE Vector3 GetScale() const { return m_Scale; }
			IE_FORCE_INLINE Vector3* GetScalePtr() { return &m_Scale; }

			/// <summary>
			/// Set the global scale
			/// </summary>
			/// <param name="_scale">: new global scale to set</param>
			/// <returns>The local scale</returns>
			IRIS_ENGINE_API Vector3 SetGlobalScale(const Vector3& _scale);

			/// <returns>The global position</returns>
			IRIS_ENGINE_API Vector3 GetGlobalScale();
			IE_FORCE_INLINE Vector3* GetGlobalScalePtr() { GetGlobalScale();  return &m_GlobalScale; }


			/// <summary>
			/// Set a new local rotation
			/// </summary>
			/// <param name="_rotation">: new local euler angles in degrees to set</param>
			/// <returns>The new local euler angles in degrees</returns>
			IRIS_ENGINE_API Vector3 SetEuler(const Vector3& _rotation);

			/// <summary>
			/// Set a new local rotation
			/// </summary>
			/// <param name="_rotation">: new local rotation in quaternion to set</param>
			/// <returns>The new local rotation in quaternion</returns>
			IRIS_ENGINE_API Quaternion SetRotation(const Quaternion& _rotation);

			/// <returns>The local euler angles in degrees
			///</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE Vector3 GetEuler() const { return m_EulerAngles; }
			IE_FORCE_INLINE Vector3* GetEulerPtr() { return &m_EulerAngles; }

			/// <returns>The local rotation in quaternion</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE Quaternion GetRotation() const { return m_Rotation; }
			IE_FORCE_INLINE Quaternion* GetRotationPtr() { return &m_Rotation; }

			/// <summary>
			/// Set the global rotation
			/// </summary>
			/// <param name="_rotation">: new global euler angles in degrees to set</param>
			/// <returns>The new local euler angles in degrees </returns>
			IRIS_ENGINE_API Vector3 SetGlobalEuler(const Vector3& _rotation);

			/// <summary>
			/// Set the global rotation
			/// </summary>
			/// <param name="_rotation">: new global rotation in quaternion to set</param>
			/// <returns>The new local rotation in quaternion</returns>
			IRIS_ENGINE_API Quaternion SetGlobalRotation(const Quaternion& _rotation);

			/// <returns>The global euler angles in degrees</returns>
			IRIS_ENGINE_API Vector3 GetGlobalEuler();
			IE_FORCE_INLINE Vector3* GetGlobalEulerPtr() { GetGlobalEuler();  return &m_GlobalEulerAngles; }

			/// <returns>The global rotation in quaternion</returns>
			IRIS_ENGINE_API Quaternion GetGlobalRotation();
			IE_FORCE_INLINE Quaternion* GetGlobalRotationPtr() { GetGlobalRotation(); return &m_GlobalRotation; }

			/// <returns>Local TRS</returns>
			IRIS_ENGINE_API Matrix4 GetLocalTRS();

			/// <returns>Global TRS</returns>
			IRIS_ENGINE_API Matrix4 GetGlobalTRS();

			/// <returns>The global right vector</returns>
			IRIS_ENGINE_API Vector3 GetRight();
			IE_FORCE_INLINE Vector3* GetRightPtr() { GetRight();  return &m_Right; }

			/// <returns>The global up vector</returns>
			IRIS_ENGINE_API Vector3 GetUp();
			IE_FORCE_INLINE Vector3* GetUpPtr() { GetUp();  return &m_Up; }

			/// <returns>the global forward vector</returns>
			IRIS_ENGINE_API Vector3 GetForward();
			IE_FORCE_INLINE Vector3* GetForwardPtr() { GetForward();  return &m_Forward; }

			/// <summary>
			/// Set the parent, recalculate the global of the object and his children
			/// </summary>
			/// <param name="_parent">: transform to set as parent, nullptr -> re-parent children to old parent</param>
			IRIS_ENGINE_API void SetParent(ITransform* _parent);

			/// <summary>
			/// Get parent
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API ITransform* GetParent();

			IRIS_ENGINE_API std::string PrintTransform();

			IRIS_ENGINE_API std::vector<ComponentHandle<ITransform>> GetChildren();

		private:
			/// <summary>
			/// Adds a new child and set this transform as parent
			/// </summary>
			/// <param name="_child">: transform to add</param>
			void AddChild(ITransform* _child);

			/// <summary>
			/// Removes the given child
			/// </summary>
			/// <param name="_child">: child to remove</param>
			/// <returns>false -> not found, true -> removed</returns>
			bool RemoveChild(ITransform* _child);

			/// <summary>
			/// Recalculate the local TRS
			/// </summary>
			void CalculateLocalTRS();

			/// <summary>
			/// Recalculate the global TRS 
			/// </summary>
			void CalculateGlobalTRS();

			/// <summary>
			/// Calls the global TRS recalculation function for all children
			/// </summary>
			void ChildrenRecalculateGlobalTRS();

			/// <summary>
			/// Recalculate trs when parent local trs change
			/// </summary>
			void RecalculateOnParentChange();


		private:

			/// <summary>
			/// The position of the object
			/// </summary>
			Vector3 m_Position = Vector3(0.f);
			Vector3 m_GlobalPosition = Vector3(0.f);

			/// <summary>
			/// The rotation in quaternion of the object
			/// </summary>
			Quaternion m_Rotation = Quaternion(1.f, 0.f, 0.f, 0.f);
			Quaternion m_GlobalRotation = Quaternion(1.f, 0.f, 0.f, 0.f);

			/// <summary>
			/// The rotation in degrees of the object
			/// </summary>
			Vector3 m_EulerAngles = Vector3(0.f);
			Vector3 m_GlobalEulerAngles = Vector3(0.f);

			/// <summary>
			/// The scale of the object
			/// </summary>
			Vector3 m_Scale = Vector3(1.f);
			Vector3 m_GlobalScale = Vector3(1.f);

			Vector3 m_Right = Vector3(1.f);
			Vector3 m_Up = Vector3(1.f);
			Vector3 m_Forward = Vector3(1.f);

			/// <summary>
			/// Local TRS matrix
			/// </summary>
			Matrix4 m_LocalMatrix = Matrix4::Identity;

			/// <summary>
			/// Global TRS matrix
			/// </summary>
			Matrix4 m_GlobalMatrix = Matrix4::Identity;

			/// <summary>
			/// Parent transform of the object
			/// </summary>
			ComponentHandle<ITransform>* m_Parent = nullptr;

			/// <summary>
			/// Transforms children of the object
			/// </summary>
			std::vector<ComponentHandle<ITransform>> m_Children;

			/// <summary>
			/// The need to recalculate local TRS
			/// </summary>
			bool m_RecalculateLocal = true;

			/// <summary>
			/// The need to recalculate global TRS
			/// </summary>
			bool m_RecalculateGlobal = true;
		};

	}
}