#pragma once

#include "Core/Time.h"

#include "SECS/SceneManager.h"
#include "SECS/Scene.h"
#include "SECS/IrisObject.h"
#include "SECS/Components/IComponent.h"
#include "SECS/Components/CameraComponent.h"
#include "SECS/Components/Collider.h"
#include "SECS/Components/LightComponent.h"
#include "SECS/Components/ModelRenderer.h"
#include "SECS/Components/RigidBody.h"
#include "SECS/Components/Transform.h"
#include "SECS/Components/ScriptComponent.h"


extern "C"
{
	// Time
	IRIS_ENGINE_API double cGetDeltaTime(Iris::Engine::Time& _time) { return _time.GetDeltaTimeD(); }
	IRIS_ENGINE_API void cSetFixedDeltaTime(Iris::Engine::Time& _time, double _fixedDeltaTime) { _time.SetFixedDeltaTime(_fixedDeltaTime); }
	IRIS_ENGINE_API double cGetFixedDeltaTime(Iris::Engine::Time& _time) { return _time.GetFixedDeltaTimeD(); }

	// Scene manager
	IRIS_ENGINE_API void cSwitchScene(Iris::Engine::SceneManager& _sm, const char* _sceneName) { return _sm.SwitchScene(std::string{ _sceneName }); }

	// Scene
	IRIS_ENGINE_API const char* cGetSceneName(Iris::Engine::Scene& _scene) { return _scene.GetSceneNamePtr(); }
	IRIS_ENGINE_API Iris::Engine::IrisObject* cShortInstantiate(Iris::Engine::Scene& _scene, const float* _position, Iris::Engine::ITransform* _parent = nullptr, const char* _name = "") { return _scene.Instantiate(Vector3(_position[0], _position[1], _position[2]), _parent, _name); }
	IRIS_ENGINE_API Iris::Engine::IrisObject* cEulerInstantiate(Iris::Engine::Scene& _scene, const float* _position, const float* _rotation, const float* _scale, Iris::Engine::ITransform* _parent = nullptr, const char* _name = "")
	{
		return _scene.Instantiate(Vector3(_position[0], _position[1], _position[2]), Vector3(_rotation[0], _rotation[1], _rotation[2]), Vector3(_scale[0], _scale[1], _scale[2]), _parent, _name);
	}
	IRIS_ENGINE_API Iris::Engine::IrisObject* cQuaternionInstantiate(Iris::Engine::Scene& _scene, const float* _position, const float* _rotation, const float* _scale, Iris::Engine::ITransform* _parent = nullptr, const char* _name = "")
	{
		return _scene.Instantiate(Vector3(_position[0], _position[1], _position[2]), Quaternion(_rotation[0], _rotation[1], _rotation[2], _rotation[3]), Vector3(_scale[0], _scale[1], _scale[2]), _parent, _name);
	}
	IRIS_ENGINE_API Iris::Engine::IrisObject* cGetObjectByID(Iris::Engine::Scene& _scene, int _objectID) { return _scene.GetObjectByID(static_cast<uint32_t>(_objectID)); }
	IRIS_ENGINE_API Iris::Engine::IrisObject* cGetObjectByName(Iris::Engine::Scene& _scene, const char* _name) { return _scene.GetObjectByName(_name); }
	//IRIS_ENGINE_API ComponentHandle<CameraComponent>* cGetMainCamera(Scene& _scene) { _scene.GetMainCamera(); }

	// Iris Object
	IRIS_ENGINE_API void cSetActive(Iris::Engine::IrisObject& _obj, bool _activate) { _obj.SetActive(_activate); }
	IRIS_ENGINE_API void cDestroy(Iris::Engine::IrisObject& _obj) { _obj.Destroy(); }
	IRIS_ENGINE_API int cGetID(Iris::Engine::IrisObject& _obj) { return static_cast<int>(_obj.GetID()); }
	IRIS_ENGINE_API void cSetName(Iris::Engine::IrisObject& _obj, const char* _name) { _obj.SetName(std::string{ _name }); }
	IRIS_ENGINE_API const char* cGetName(Iris::Engine::IrisObject& _obj) { return _obj.GetNamePtr(); }
	IRIS_ENGINE_API Iris::Engine::ComponentHandle<Iris::Engine::CameraComponent>* cGetCameraComponent(Iris::Engine::IrisObject& _obj)
	{
		return _obj.GetHandle<Iris::Engine::ComponentHandle<Iris::Engine::CameraComponent>, Iris::Engine::CameraComponent>();
	}
	IRIS_ENGINE_API Iris::Engine::ComponentHandle<Iris::Engine::Collider>* cGetColliderComponent(Iris::Engine::IrisObject& _obj)
	{
		return _obj.GetHandle<Iris::Engine::ComponentHandle<Iris::Engine::Collider>, Iris::Engine::Collider>();
	}
	IRIS_ENGINE_API Iris::Engine::ComponentHandle<Iris::Engine::LightComponent>* cGetLightComponent(Iris::Engine::IrisObject& _obj)
	{
		return _obj.GetHandle<Iris::Engine::ComponentHandle<Iris::Engine::LightComponent>, Iris::Engine::LightComponent>();
	}
	IRIS_ENGINE_API Iris::Engine::ComponentHandle<Iris::Engine::ModelRenderer>* cGetModelComponent(Iris::Engine::IrisObject& _obj)
	{
		return _obj.GetHandle<Iris::Engine::ComponentHandle<Iris::Engine::ModelRenderer>, Iris::Engine::ModelRenderer>();
	}
	IRIS_ENGINE_API Iris::Engine::ComponentHandle<Iris::Engine::RigidBody>* cGetRigidBodyComponent(Iris::Engine::IrisObject& _obj)
	{
		return _obj.GetHandle<Iris::Engine::ComponentHandle<Iris::Engine::RigidBody>, Iris::Engine::RigidBody>();
	}
	IRIS_ENGINE_API Iris::Engine::ComponentHandle<Iris::Engine::ITransform>* cGetTransformComponent(Iris::Engine::IrisObject& _obj)
	{
		return _obj.GetHandle<Iris::Engine::ComponentHandle<Iris::Engine::ITransform>, Iris::Engine::ITransform>();
	}
	IRIS_ENGINE_API Iris::Engine::ComponentHandle<Iris::Engine::ScriptComponent>* cGetScriptComponent(Iris::Engine::IrisObject& _obj)
	{
		return _obj.GetHandle<Iris::Engine::ComponentHandle<Iris::Engine::ScriptComponent>, Iris::Engine::ScriptComponent>();
	}
	IRIS_ENGINE_API Iris::Engine::ComponentHandle<Iris::Engine::CameraComponent>* cAddCameraComponent(Iris::Engine::IrisObject& _obj)
	{
		_obj.AddComponent<Iris::Engine::CameraComponent>(); return cGetCameraComponent(_obj);
	}
	IRIS_ENGINE_API Iris::Engine::ComponentHandle<Iris::Engine::Collider>* cAddColliderComponent(Iris::Engine::IrisObject& _obj)
	{
		_obj.AddComponent<Iris::Engine::Collider>(); return cGetColliderComponent(_obj);
	}
	IRIS_ENGINE_API Iris::Engine::ComponentHandle<Iris::Engine::LightComponent>* cAddLightComponent(Iris::Engine::IrisObject& _obj)
	{
		_obj.AddComponent<Iris::Engine::LightComponent>(); return cGetLightComponent(_obj);
	}
	IRIS_ENGINE_API Iris::Engine::ComponentHandle<Iris::Engine::ModelRenderer>* cAddModelComponent(Iris::Engine::IrisObject& _obj)
	{
		_obj.AddComponent<Iris::Engine::ModelRenderer>(); return cGetModelComponent(_obj);
	}
	IRIS_ENGINE_API Iris::Engine::ComponentHandle<Iris::Engine::RigidBody>* cAddRigidBodyComponent(Iris::Engine::IrisObject& _obj)
	{
		_obj.AddComponent<Iris::Engine::RigidBody>(); return cGetRigidBodyComponent(_obj);
	}
	IRIS_ENGINE_API Iris::Engine::ComponentHandle<Iris::Engine::ScriptComponent>* cAddScriptComponent(Iris::Engine::IrisObject& _obj)
	{
		_obj.AddComponent<Iris::Engine::ScriptComponent>(); return cGetScriptComponent(_obj);
	}
	IRIS_ENGINE_API void cRemoveCameraComponent(Iris::Engine::IrisObject& _obj) { _obj.RemoveComponent<Iris::Engine::CameraComponent>(); }
	IRIS_ENGINE_API void cRemoveColliderComponent(Iris::Engine::IrisObject& _obj) { _obj.RemoveComponent<Iris::Engine::Collider>(); }
	IRIS_ENGINE_API void cRemoveLightComponent(Iris::Engine::IrisObject& _obj) { _obj.RemoveComponent<Iris::Engine::LightComponent>(); }
	IRIS_ENGINE_API void cRemoveModelComponent(Iris::Engine::IrisObject& _obj) { _obj.RemoveComponent<Iris::Engine::ModelRenderer>(); }
	IRIS_ENGINE_API void cRemoveRigidBodyComponent(Iris::Engine::IrisObject& _obj) { _obj.RemoveComponent<Iris::Engine::RigidBody>(); }
	IRIS_ENGINE_API void cRemoveScriptComponent(Iris::Engine::IrisObject& _obj) { _obj.RemoveComponent<Iris::Engine::ScriptComponent>(); }
}
