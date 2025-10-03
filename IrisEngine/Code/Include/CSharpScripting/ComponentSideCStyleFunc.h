#pragma once

#include "SECS/IrisObject.h"
#include "SECS/Components/IComponent.h"
#include "SECS/Components/CameraComponent.h"
#include "SECS/Components/Collider.h"
#include "SECS/Components/LightComponent.h"
#include "SECS/Components/ModelRenderer.h"
#include "SECS/Components/RigidBody.h"
#include "SECS/Components/Transform.h"

#include "Resources/ResourceManager.h"
#include "Resources/Mesh.h"
// #include "Resources/Material.h"


extern "C"
{
	// IComponent
	IRIS_ENGINE_API void cSetEnable(Iris::Engine::IComponentHandle& _handle, bool _activate) { _handle.SetEnableComponent(_activate); }
	IRIS_ENGINE_API bool cIsEnable(Iris::Engine::IComponentHandle& _handle) { return _handle.IsEnableComponent(); }
	IRIS_ENGINE_API Iris::Engine::IrisObject* cGetObject(Iris::Engine::IComponentHandle* _handle) {
		if (!_handle)
		{
			std::cout << "NULL handle passed to cGetObject!" << std::endl;
			return nullptr;
		}
		return _handle->GetIrisObject(); }

	// Transform
	IRIS_ENGINE_API void cSetTransformPosition(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform, float* _position) { _transform.UseComponent()->SetPosition(Vector3{ _position[0], _position[1] , _position[2] }); }
	IRIS_ENGINE_API Vector3* cGetTransformPosition(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform) { return _transform.UseComponent()->GetPositionPtr(); }
	IRIS_ENGINE_API void cSetTransformGlobalPosition(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform, float* _position) { _transform.UseComponent()->SetGlobalPosition(Vector3{ _position[0], _position[1], _position[2] }); }
	IRIS_ENGINE_API Vector3* cGetTransformGlobalPosition(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform) { return _transform.UseComponent()->GetGlobalPositionPtr(); }
	IRIS_ENGINE_API void cSetScale(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform, float* _scale) { _transform.UseComponent()->SetScale(Vector3{ _scale[0], _scale[1], _scale[2] }); }
	IRIS_ENGINE_API Vector3* cGetScale(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform) { return _transform.UseComponent()->GetScalePtr(); }
	IRIS_ENGINE_API void cSetGlobalScale(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform, float* _scale) { _transform.UseComponent()->SetGlobalScale(Vector3{ _scale[0], _scale[1], _scale[2] }); }
	IRIS_ENGINE_API Vector3* cGetGlobalScale(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform) { return _transform.UseComponent()->GetGlobalScalePtr(); }
	IRIS_ENGINE_API void cSetEuler(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform, float* _rotation) { _transform.UseComponent()->SetEuler(Vector3{ _rotation[0], _rotation[1], _rotation[2] }); }
	IRIS_ENGINE_API Vector3* cGetEuler(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform) { return _transform.UseComponent()->GetEulerPtr(); }
	IRIS_ENGINE_API void cSetGlobalEuler(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform, float* _rotation) { _transform.UseComponent()->SetGlobalEuler(Vector3{ _rotation[0], _rotation[1], _rotation[2] }); }
	IRIS_ENGINE_API Vector3* cGetGlobalEuler(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform) { return _transform.UseComponent()->GetGlobalEulerPtr(); }
	IRIS_ENGINE_API void cSetTransformRotation(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform, float* _rotation) { _transform.UseComponent()->SetRotation(Quaternion{ _rotation[0], _rotation[1], _rotation[2], _rotation[3] }); }
	IRIS_ENGINE_API Quaternion* cGetTransformRotation(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform) { return _transform.UseComponent()->GetRotationPtr(); }
	IRIS_ENGINE_API void cSetGlobalRotation(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform, float* _rotation) { _transform.UseComponent()->SetGlobalRotation(Quaternion{ _rotation[0], _rotation[1], _rotation[2], _rotation[3] }); }
	IRIS_ENGINE_API Quaternion* cGetGlobalRotation(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform) { return _transform.UseComponent()->GetGlobalRotationPtr(); }
	IRIS_ENGINE_API Vector3* cGetTransformRight(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform) { return _transform.UseComponent()->GetRightPtr(); }
	IRIS_ENGINE_API Vector3* cGetTransformUp(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform) { return _transform.UseComponent()->GetUpPtr(); }
	IRIS_ENGINE_API Vector3* cGetTransformForward(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform) { return _transform.UseComponent()->GetForwardPtr(); }
	IRIS_ENGINE_API void cSetParent(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform, Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _parent) { _transform.UseComponent()->SetParent(_parent.UseComponent()); }
	IRIS_ENGINE_API Iris::Engine::ComponentHandle<Iris::Engine::ITransform>* cGetParent(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform) { return _transform.UseComponent()->GetParent()->GetIrisObject()->GetHandle<Iris::Engine::ComponentHandle<Iris::Engine::ITransform>, Iris::Engine::ITransform>(); }
	// IRIS_ENGINE_API std::vector<Iris::Engine::ComponentHandle<Iris::Engine::ITransform>>* cGetChildren(Iris::Engine::ComponentHandle<Iris::Engine::ITransform>& _transform) { return &(_transform.UseComponent()->GetChildren()); }
			
	// CameraComponent
	IRIS_ENGINE_API Vector3* cGetCameraForward(Iris::Engine::ComponentHandle<Iris::Engine::CameraComponent>& _camera) { return _camera.UseComponent()->GetForwardPtr(); }
	IRIS_ENGINE_API void cActivateOrthoMode(Iris::Engine::ComponentHandle<Iris::Engine::CameraComponent>& _camera, bool _activate) { _camera.UseComponent()->SetIsOrtho(_activate); }

	// LightComponent
	IRIS_ENGINE_API void cSetColor(Iris::Engine::ComponentHandle<Iris::Engine::LightComponent>& _light, float* _color) { _light.UseComponent()->SetColor(Vector4{ _color[0], _color[1], _color[2], 1.f }); }
	IRIS_ENGINE_API Vector4* cGetColor(Iris::Engine::ComponentHandle<Iris::Engine::LightComponent>& _light) { return _light.UseComponent()->GetColorPtr(); }
	IRIS_ENGINE_API void cSetIntensity(Iris::Engine::ComponentHandle<Iris::Engine::LightComponent>& _light, float _intensity) { _light.UseComponent()->SetIntensity(_intensity); }
	IRIS_ENGINE_API float cGetIntensity(Iris::Engine::ComponentHandle<Iris::Engine::LightComponent>& _light) { return _light.UseComponent()->GetIntensity(); }
	IRIS_ENGINE_API void cSetNear(Iris::Engine::ComponentHandle<Iris::Engine::LightComponent>& _light, float _near) { _light.UseComponent()->SetNear(_near); }
	IRIS_ENGINE_API float cGetNear(Iris::Engine::ComponentHandle<Iris::Engine::LightComponent>& _light) { return _light.UseComponent()->GetNear(); }
	IRIS_ENGINE_API void cSetFar(Iris::Engine::ComponentHandle<Iris::Engine::LightComponent>& _light, float _far) { _light.UseComponent()->SetFar(_far); }
	IRIS_ENGINE_API float cGetFar(Iris::Engine::ComponentHandle<Iris::Engine::LightComponent>& _light) { return _light.UseComponent()->GetFar(); }
	IRIS_ENGINE_API void cSetCutOff(Iris::Engine::ComponentHandle<Iris::Engine::LightComponent>& _light, float _cutOff) { _light.UseComponent()->SetCutOff(_cutOff); }
	IRIS_ENGINE_API float cGetCutOff(Iris::Engine::ComponentHandle<Iris::Engine::LightComponent>& _light) { return _light.UseComponent()->GetCutOff(); }
	IRIS_ENGINE_API void cSetOuterCutOff(Iris::Engine::ComponentHandle<Iris::Engine::LightComponent>& _light, float _outerCutOff) { _light.UseComponent()->SetOuterCutOff(_outerCutOff); }
	IRIS_ENGINE_API float cGetOuterCutOff(Iris::Engine::ComponentHandle<Iris::Engine::LightComponent>& _light) { return _light.UseComponent()->GetOuterCutOff(); }
	IRIS_ENGINE_API void cSetLightType(Iris::Engine::ComponentHandle<Iris::Engine::LightComponent>& _light, int _type) { _light.UseComponent()->SetType(static_cast<Iris::Engine::LightType>(_type)); }
	IRIS_ENGINE_API int cGetLightType(Iris::Engine::ComponentHandle<Iris::Engine::LightComponent>& _light) { return static_cast<int>(_light.UseComponent()->GetType()); }
			
	// RigidBody
	IRIS_ENGINE_API void cSetRigidBodyPosition(Iris::Engine::ComponentHandle<Iris::Engine::RigidBody>& _rigidBody, float* _position) { _rigidBody.UseComponent()->SetPosition(Vector3{ _position[0], _position[1], _position[2] }); }
	IRIS_ENGINE_API Vector3* cGetRigidBodyPosition(Iris::Engine::ComponentHandle<Iris::Engine::RigidBody>& _rigidBody) { return _rigidBody.UseComponent()->GetPositionPtr(); }
	IRIS_ENGINE_API void cSetPhysiquePosition(Iris::Engine::ComponentHandle<Iris::Engine::RigidBody>& _rigidBody) { _rigidBody.UseComponent()->SetPhysiquePosition(); }
	IRIS_ENGINE_API void cSetTargetPosition(Iris::Engine::ComponentHandle<Iris::Engine::RigidBody>& _rigidBody, float* _position) { _rigidBody.UseComponent()->SetTargetPosition(Vector3{ _position[0], _position[1], _position[2] }); }
	IRIS_ENGINE_API void cSetRigidBodyRotation(Iris::Engine::ComponentHandle<Iris::Engine::RigidBody>& _rigidBody, float* _rotation) { _rigidBody.UseComponent()->SetRotation(Quaternion{ _rotation[0], _rotation[1], _rotation[2], _rotation[3] }); }
	IRIS_ENGINE_API Quaternion* cGetRigidBodyRotation(Iris::Engine::ComponentHandle<Iris::Engine::RigidBody>& _rigidBody) { return _rigidBody.UseComponent()->GetRotationPtr(); }
	IRIS_ENGINE_API void cSetVelocity(Iris::Engine::ComponentHandle<Iris::Engine::RigidBody>& _rigidBody, float* _velocity) { _rigidBody.UseComponent()->SetVelocity(Vector3{ _velocity[0], _velocity[1], _velocity[2] }); }
	IRIS_ENGINE_API Vector3* cGetVelocity(Iris::Engine::ComponentHandle<Iris::Engine::RigidBody>& _rigidBody) { return _rigidBody.UseComponent()->GetVelocityPtr(); }
	IRIS_ENGINE_API void cAddForce(Iris::Engine::ComponentHandle<Iris::Engine::RigidBody>& _rigidBody, float* _force) { _rigidBody.UseComponent()->AddForce(Vector3{ _force[0], _force[1], _force[2] }); }
	IRIS_ENGINE_API void cSetPosInterpolation(Iris::Engine::ComponentHandle<Iris::Engine::RigidBody>& _rigidBody, bool _activate) { _rigidBody.UseComponent()->SetInterpolatePosition(_activate); }
	IRIS_ENGINE_API void cSetRotationConstraint(Iris::Engine::ComponentHandle<Iris::Engine::RigidBody>& _rigidBody, float* _constraints)
	{
		Iris::Engine::Constraint constraints{};
		constraints.SetRotationConstraint(_constraints[0], _constraints[1], _constraints[2]);
		_rigidBody.UseComponent()->SetRotationConstraint(constraints);
	}

	// Script


	// ModelRenderer
	IRIS_ENGINE_API void cSetMesh(Iris::Engine::ComponentHandle<Iris::Engine::ModelRenderer>& _modelRenderer, const char* _meshName) { _modelRenderer.UseComponent()->SetMesh(Iris::Engine::ResourceManager::GetInstance()->Get<Iris::Engine::Mesh>(std::string{_meshName})); }
	// IRIS_ENGINE_API void cSetMaterial(const ComponentHandle<ModelRenderer>& _modelRenderer, const char* _materialName) { ; }

	// Collider
	//IRIS_ENGINE_API void cSetShapeInfo(const ComponentHandle<Collider>& _collider, int _shapeType, float* _shapeInfo) { _collider.UseComponent()->SetShapeInfo; }
	//IRIS_ENGINE_API float* cGetShapeInfo(const ComponentHandle<Collider>& _collider);
	//IRIS_ENGINE_API int cGetCurrentShape(const ComponentHandle<Collider>& _collider) { return _collider.UseComponent()->GetCurrentShape(); }
	IRIS_ENGINE_API void cSetOffset(Iris::Engine::ComponentHandle<Iris::Engine::Collider>& _collider, float* _offset) { _collider.UseComponent()->SetOffSet(Vector3{_offset[0], _offset[1] , _offset[2]}); }
	IRIS_ENGINE_API Vector3* cGetOffset(Iris::Engine::ComponentHandle<Iris::Engine::Collider>& _collider) { return _collider.UseComponent()->GetOffSetPtr(); }
}