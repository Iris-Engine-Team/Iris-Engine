#include "SECS/Scene.h"
#include "SECS/IrisObject.h"

#include "SECS/Components/IComponent.h"
#include "SECS/Components/Transform.h"
#include "SECS/Components/LightComponent.h"
#include "SECS/Components/ModelRenderer.h"
#include "SECS/Components/SkyboxObject.h"

#include "SECS/Systems/RenderSystem.h"
#include "SECS/Systems/CameraSystem.h"
#include "Core/Physics/JoltImpl/JoltPhysicsSysteme.h"
#include "SECS/Systems/PhysicsSystem.h"
#include "SECS/Systems/ScriptSystem.h"


#include "SECS/LightData/LightData.h"
#include "Core/Graphics/Context/ResourceContext.h"
#include "Core/Graphics/RHI/IDescriptorLayout.h"
#include "Core/Graphics/Renderer/IRenderer.h"
#include "Core/Graphics/RHI/IDevice.h"

#include "Core/Application.h"
#include "Resources/MetadataManager.h"
#include "CSharpScripting/ScriptManager.h"

namespace Iris
{
	namespace Engine
	{
		uint64_t Scene::s_IdCounter = 0;

		Scene::Scene(std::string _name)
		{
			m_Root = new IrisObject(this);
			m_Root->SetName("Root");
			m_Name = _name;
		}

		Scene::~Scene()
		{
		}

		void Scene::Load(InitEditorContext& _initializer)
		{
			if (m_SceneMetadataRef->GetData().size() <= 2)
			{
				DEBUG_WARN("Not enought data to load the scene");
				return;
			}

			std::vector<std::string> pathIndexIncrementation = { "Scene Data", "Index" };
			s_IdCounter = m_SceneMetadataRef->GetData(pathIndexIncrementation);

			std::vector<std::string> path = { "Scene Data", "IrisObject" };
			MetaDataType objectsData = m_SceneMetadataRef->GetData(path);

			for (auto it = objectsData.begin(); it != objectsData.end(); ++it)
				m_AllIrisObjects.emplace_back(new IrisObject(this, m_SceneMetadataRef, it.key(), _initializer));

			CheckRemoveLists();
			CheckAddLists();

			GetComponentTable<ITransform>()->Deserialization(m_SceneMetadataRef, _initializer);

			std::vector<CameraComponent> cams = m_CameraSystemRef->GetAllCameras();
			size_t camCount = cams.size();
			for (size_t i = 0; i < camCount; ++i)
			{
				if (cams[i].IsMainGameCam())
				{
					m_CameraSystemRef->SetMainCamera(&cams[i]);
					break;
				}
			}

			path[1] = "Skybox";
			path.push_back("Texture");
			m_RenderSystemRef->GetSkybox()->SetTexture(m_SceneMetadataRef->GetData(path));

			path[2] = "Rotation";
			std::vector<float> euler = m_SceneMetadataRef->GetData(path);
			m_RenderSystemRef->GetSkybox()->SetRotation(Vector3(euler[0], euler[1], euler[2]));

			m_IsLoaded = true;
		}

		void Scene::ReLoad(InitEditorContext& _initializer)
		{
			int editorCamId = -1;
			ComponentHandle<CameraComponent>* camH = m_CameraSystemRef->GetEditorCamera();
			if (camH)
				editorCamId = static_cast<int>(camH->GetIrisObject()->GetID());

			m_RenderSystemRef->Clear();

			m_PhysicsSystemRef->Clear();

			m_ScriptSystemRef->Clear();
			m_ScriptSystemRef->SetInstanciateScript(false);

			m_CameraSystemRef->Clear(*m_ResourceContextRef, *_initializer.InitRenderingContext, true);

			size_t size = m_AllIrisObjects.size();
			for (size_t i = 0; i < size; ++i)
			{
				if (m_AllIrisObjects[i]->GetID() != editorCamId)
					m_AllIrisObjects[i]->Destroy();
			}

			CheckRemoveLists();

			m_PhysicsSystemRef->SetScene(this);
			m_RenderSystemRef->SetScene(this);
			m_ScriptSystemRef->SetScene(this);
			m_CameraSystemRef->SetScene(this);

			if (m_SceneMetadataRef->GetData().size() <= 1)
				return;

			std::vector<std::string> pathIndexIncrementation = { "Scene Data", "Index" };
			s_IdCounter = m_SceneMetadataRef->GetData(pathIndexIncrementation);

			std::vector<std::string> path = { "Scene Data", "IrisObject" };
			MetaDataType objectsData = m_SceneMetadataRef->GetData(path);

			for (auto it = objectsData.begin(); it != objectsData.end(); ++it)
			{
				if(it.key() != std::to_string(editorCamId))
					m_AllIrisObjects.emplace_back(new IrisObject(this, m_SceneMetadataRef, it.key(), _initializer));
			}

			CheckRemoveLists();
			CheckAddLists();
			GetComponentTable<ITransform>()->Deserialization(m_SceneMetadataRef, _initializer);

			std::vector<CameraComponent>&  cams = m_CameraSystemRef->GetAllCameras();
			size_t camCount = cams.size();
			for (size_t i = 0; i < camCount; ++i)
			{
				if (cams[i].IsMainGameCam())
				{
					m_CameraSystemRef->SetMainCamera(&cams[i]);
					break;
				}
			}

			path[1] = "Skybox";
			path.push_back("Texture");
			m_RenderSystemRef->GetSkybox()->SetTexture(m_SceneMetadataRef->GetData(path));

			path[2] = "Rotation";
			std::vector<float> euler = m_SceneMetadataRef->GetData(path);
			m_RenderSystemRef->GetSkybox()->SetRotation(Vector3(euler[0], euler[1], euler[2]));

			m_IsLoaded = true;
		}

		void Scene::Save(MetaDataManager* _metadataManager)
		{
			m_SceneMetadataRef->Write("Type", MetadataResourceType::IE_SCENE_DATA);

			std::vector<std::string> path0 = { "Scene Data", "Index" };
			m_SceneMetadataRef->Write(path0, s_IdCounter);

			path0[1] = "Skybox";
			path0.push_back("Texture");
			m_SceneMetadataRef->Write(path0, m_RenderSystemRef->GetSkybox()->GetTexture().GetName());

			path0[2] = "Rotation";
			Vector3 rota = m_RenderSystemRef->GetSkybox()->GetRotation();
			std::vector<float> eulerSkybox = { rota[0], rota[1], rota[2] };
			m_SceneMetadataRef->Write(path0, eulerSkybox);

			std::vector<IMetadata*> irisobjectsMetadata;
			std::vector<std::string> irisObjID;

			std::vector<std::string> path = { "Scene Data", "IrisObject" ,""};

			//~~~~~~~Writes other objects's data~~~~~~~//

			std::mutex mutexMetadataPush;
			std::mutex mutexIdPush;

			size_t size = m_AllIrisObjects.size();
			m_ThreadManagerRef->SubmitLoop(0, size, [this, &_metadataManager, &irisobjectsMetadata, &irisObjID, &mutexMetadataPush, &mutexIdPush](const size_t i)
				{
					IMetadata* objectData = _metadataManager->CreateMetadata();
					m_AllIrisObjects[i]->Serialize(objectData);

					{
						std::lock_guard<std::mutex> lock(mutexMetadataPush);
						irisobjectsMetadata.push_back(objectData);
					}

					std::string id = std::to_string(m_AllIrisObjects[i]->GetID());
					{
						std::lock_guard<std::mutex> lock(mutexIdPush);
						irisObjID.push_back(id);
					}

					std::vector<std::string> path = { "Scene Data", "IrisObject" , id };
					m_SceneMetadataRef->WriteMeta(path, objectData);
				}
			);

			size = irisobjectsMetadata.size();
			for (size_t j = 0; j < size; ++j)
				delete irisobjectsMetadata[j];
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

			//~~~~~~~~~~Writes components data~~~~~~~~~//

			std::vector<std::future<void>> tasks;

			for (auto it = m_AllComponents.begin(); it != m_AllComponents.end(); ++it)
				tasks.push_back(m_ThreadManagerRef->SubmitTask([this, &_metadataManager, table = it]() { table->second->Serialize(_metadataManager, m_SceneMetadataRef);}));

			for (std::future<void>& f : tasks)
				f.wait();
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

			//~~Removes objects && components deleted~~//
			path = { "Scene Data", "IrisObject" };
			MetaDataType objectsData = m_SceneMetadataRef->GetData(path);

			for (auto it = objectsData.begin(); it != objectsData.end(); ++it)
			{
				bool founded = false;
				
				for (size_t i = 0; i < irisObjID.size(); ++i)
				{
					if (it.key() == irisObjID[i])
					{
						irisObjID.erase(irisObjID.begin() + i);
						founded = true;
						break;
					}
				}

				if (!founded)
				{
					std::vector<std::string> pathComponents = { "Scene Data", "IrisObject" , it.key(), "Components" };
					std::vector<std::string> toDeleteComp = m_SceneMetadataRef->GetData(pathComponents);

					size_t nbcomp = toDeleteComp.size();
					for (size_t compIndex = 0; compIndex < nbcomp; ++compIndex)
					{
						std::vector<std::string> pathRemoveComponent = { "Scene Data", "Components", toDeleteComp[compIndex], it.key()};
						m_SceneMetadataRef->RemoveAt(pathRemoveComponent);
					}

					std::vector<std::string> pathRemove = { "Scene Data", "IrisObject", it.key() };
					m_SceneMetadataRef->RemoveAt(pathRemove);
				}
			}
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

			std::vector<std::string> pathTransformRoot = { "Scene Data", "Components" , std::to_string(static_cast<int>(ITransform::GetID())), ITransform::GetName(), std::to_string(m_Root->GetID())};
			m_SceneMetadataRef->RemoveAt(pathTransformRoot);
			m_SceneMetadataRef->Update();
		}

		void Scene::Init(IMetadata* _sceneMetadata, const ThreadManager* _threadManager, ResourceContext* _resourceContext
			, CameraSystem* _cameraSystem, RenderSystem* _renderSystem, IPhysicsSystem* _physicsSystem, ScriptSystem* _scriptSystem)
		{
			m_SceneMetadataRef = _sceneMetadata;
			m_ThreadManagerRef = _threadManager;
			m_ResourceContextRef = _resourceContext;

			m_CameraSystemRef = _cameraSystem;
			m_RenderSystemRef = _renderSystem;
			m_PhysicsSystemRef = _physicsSystem;

			m_ScriptSystemRef = _scriptSystem;
		}

		IrisObject* Scene::BaseInstantiate(std::string _name)
		{
			IrisObject* newObj = new IrisObject(this);

			if (_name == "")
				_name = newObj->GetName() + " (" + std::to_string(newObj->GetID()) + ")";
			newObj->SetName(_name);

			m_AllIrisObjects.emplace_back(newObj);
			return newObj;
		}

		IrisObject* Scene::Instantiate(const Vector3& _position, ITransform* _parent, std::string _name)
		{
			IrisObject* newObj = BaseInstantiate(_name);
			ITransform* t = GetComponent<ITransform>(newObj);
			if (!_parent)
				_parent = m_Root->GetTransform();

			t->Init(_position, _parent);

			return newObj;
		}

		IrisObject* Scene::Instantiate(const Vector3& _position, const Quaternion& _rotation, const Vector3& _scale, ITransform* _parent, std::string _name)
		{
			IrisObject* newObj = BaseInstantiate(_name);

			ITransform* t = GetComponent<ITransform>(newObj);
			if (!_parent)
				_parent = m_Root->GetTransform();

			t->Init(_position, _rotation, _scale, _parent);

			return newObj;
		}

		IrisObject* Scene::Instantiate(const Vector3& _position, const Vector3& _rotation, const Vector3& _scale, ITransform* _parent, std::string _name)
		{
			IrisObject* newObj = BaseInstantiate(_name);

			ITransform* t = GetComponent<ITransform>(newObj);

			if (!_parent)
				_parent = m_Root->GetTransform();

			t->Init(_position, _rotation, _scale, _parent);

			return newObj;
		}

		void Scene::DestroyIrisObject(IrisObject* _irisObject)
		{
			if (!_irisObject || _irisObject->IsDestroyed())
				return;

			if (GetComponent<ITransform>(_irisObject))
				_irisObject->Destroy();
		}

		IrisObject* Scene::GetObjectByID(uint64_t _objectID)
		{
			size_t size = m_AllIrisObjects.size();
			for (size_t i = 0; i < size; ++i)
			{
				if (m_AllIrisObjects[i]->GetID() == _objectID)
					return m_AllIrisObjects[i];
			}

			return nullptr;
		}

		IrisObject* Scene::GetObjectByName(std::string _name)
		{
			size_t size = m_AllIrisObjects.size();
			for (size_t i = 0; i < size; ++i)
			{
				if (m_AllIrisObjects[i]->GetName() == _name)
					return m_AllIrisObjects[i];
			}

			return nullptr;
		}

		void Scene::Update(double _deltaTime)
		{
			_deltaTime;
			
			CheckRemoveLists();
			CheckAddLists();
		
			m_PhysicsSystemRef->UpdateModifiedShape();

			m_CameraSystemRef->Update();
		}

		void Scene::FixedUpdate(double _fixedDeltaTime)
		{
			m_ScriptSystemRef->FixedUpdate();

			m_PhysicsSystemRef->Update(_fixedDeltaTime);
		}

		void Scene::Destroy()
		{
			for (auto it = m_AllComponents.begin(); it != m_AllComponents.end(); ++it)
			{
				it->second->Destroy();
				delete it->second;
			}
			
			delete m_Root;
			m_Root = nullptr;

			for (IrisObject* o : m_AllIrisObjects)
				delete o;
			m_AllIrisObjects.clear();
		}

		IComponentTable* Scene::AddTable(ComponentID _componentId)
		{
			IComponentTable* table = nullptr;

			if (_componentId == Collider::GetID())
				table = AddTable<Collider>();
			
			else if (_componentId == ModelRenderer::GetID())
				table = AddTable<ModelRenderer>();

			else if (_componentId == LightComponent::GetID())
				table = AddTable<LightComponent>();

			else if (_componentId == RigidBody::GetID())
				table = AddTable<RigidBody>();

			else if (_componentId == CameraComponent::GetID())
				table = AddTable<CameraComponent>();

			else if (_componentId == ScriptComponent::GetID())
				table = AddTable<ScriptComponent>();

			return table;
		}

		IComponent* Scene::GetComponent(IrisObject* _irisObjectID, ComponentID _componentId)
		{
			IComponent* comp = nullptr;

			if (_componentId == Collider::GetID())
				comp = GetComponent<Collider>(_irisObjectID);

			else if (_componentId == ModelRenderer::GetID())
				comp = GetComponent<ModelRenderer>(_irisObjectID);

			else if (_componentId == LightComponent::GetID())
				comp = GetComponent<LightComponent>(_irisObjectID);

			else if (_componentId == RigidBody::GetID())
				comp = GetComponent<RigidBody>(_irisObjectID);

			else if (_componentId == CameraComponent::GetID())
				comp = GetComponent<CameraComponent>(_irisObjectID);

			else if (_componentId == ScriptComponent::GetID())
				comp = GetComponent<ScriptComponent>(_irisObjectID);

			return comp;
		}

		IComponent* Scene::AddComponent(IrisObject* _irisObjectID, ComponentID _componentId)
		{
			IComponent* comp = nullptr;

			if (_componentId == Collider::GetID())
				comp = AddComponent<Collider>(_irisObjectID);

			else if (_componentId == ModelRenderer::GetID())
				comp = AddComponent<ModelRenderer>(_irisObjectID);

			else if (_componentId == LightComponent::GetID())
				comp = AddComponent<LightComponent>(_irisObjectID);

			else if (_componentId == RigidBody::GetID())
				comp = AddComponent<RigidBody>(_irisObjectID);

			else if (_componentId == CameraComponent::GetID())
				comp = AddComponent<CameraComponent>(_irisObjectID);

			else if (_componentId == ScriptComponent::GetID())
				comp = AddComponent<ScriptComponent>(_irisObjectID);

			return comp;
		}

		IrisObject* Scene::GetRoot() const
		{
			return m_Root;
		}

		void Scene::CheckRemoveLists()
		{
			size_t size = m_ComponentsToDestroy.size();

			if (size > 0)
				m_ResourceContextRef->GetDevice()->WaitIdle();

			for (size_t i = 0; i < size; ++i)
			{
				auto table = m_AllComponents.find(m_ComponentsToDestroy[i]->GetType());
				table->second->RemoveComponent(m_ComponentsToDestroy[i]->GetIrisObject());
				delete m_ComponentsToDestroy[i];
			}
			m_ComponentsToDestroy.clear();


			size_t sizeAllObject = m_AllIrisObjects.size();
			for (size_t i = 0; i < sizeAllObject;++i)
			{
				size_t sizeObjectToDestroy = m_IrisObjectToDestroy.size();
				if (sizeObjectToDestroy == 0)
					break;


				for (size_t j = 0; j < sizeObjectToDestroy; ++j)
				{
					if (m_AllIrisObjects[i]->GetID() == m_IrisObjectToDestroy[j]->GetID())
					{
						delete m_AllIrisObjects[i];
						m_AllIrisObjects.erase(m_AllIrisObjects.begin() + i);
						--i;
						--sizeAllObject;
						m_IrisObjectToDestroy.erase(m_IrisObjectToDestroy.begin() + j);
						break;
					}
				}
			}
			m_IrisObjectToDestroy.clear();
		}

		void Scene::CheckAddLists()
		{
			size_t size = m_ComponentsToAdd.size();
			for (size_t i = 0; i < size;++i)
			{
				auto table = m_AllComponents.find(m_ComponentsToAdd[i]->GetTypeID());
				table->second->AddComponent(m_ComponentsToAdd[i]->GetIrisObject())->Copy(m_ComponentsToAdd[i]);
			}

			for (size_t i = 0; i < size;++i)
				delete m_ComponentsToAdd[i];

			m_ComponentsToAdd.clear();
		}
	}
}
