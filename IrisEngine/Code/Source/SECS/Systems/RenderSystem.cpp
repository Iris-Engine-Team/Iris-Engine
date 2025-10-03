#include "SECS/Systems/RenderSystem.h"

#include "SECS/Scene.h"
#include "SECS/Components/LightComponent.h"
#include "SECS/Components/ModelRenderer.h"
#include "SECS/Components/SkyboxObject.h"

#include "SECS/Systems/PhysicsSystem.h"

#include "Core/Graphics/RHI/IBuffer.h"
#include "Core/Graphics/RHI/IDescriptor.h"
#include "Core/Graphics/Renderer/IRenderer.h"

#include "Core/MultiThread/ThreadManager.h"

#include "Core/Graphics/RHI/IDevice.h"

namespace Iris
{
	namespace Engine
	{
		RenderSystem::RenderSystem(Scene* _scene)
		{
			m_Skybox = new SkyboxObject();
			SetScene(_scene);
		}

		void RenderSystem::SetScene(Scene* _scene)
		{
			if (!_scene)
				return;

			m_SceneRef = _scene;

			if (m_SceneRef->ContainTable<ModelRenderer>())
				m_ModelRenderers = m_SceneRef->GetComponentTable<ModelRenderer>();
			else
				m_ModelRenderers = m_SceneRef->AddTable<ModelRenderer>();

			if (m_SceneRef->ContainTable<LightComponent>())
				m_Lights = m_SceneRef->GetComponentTable<LightComponent>();
			else
				m_Lights = m_SceneRef->AddTable<LightComponent>();
		}

		void RenderSystem::Update(IRenderer* _renderer, RenderingContext* _renderingContext, ResourceContext* _resourceContext)
		{
			std::vector<LightComponent>& lightsRef = m_Lights->GetAllComponents();

			bool needWait = false;

			size_t lightCount = m_Lights->ComponentCount();
			for (size_t i = 0; i < lightCount; ++i)
			{
				if (!needWait && lightsRef[i].UpdateRequired())
				{
					_renderingContext->GetDevice()->WaitGraphicsQueueIdle();
					needWait = true;
				}

				lightsRef[i].UpdateComponent(*_renderer, *_renderingContext, *_resourceContext);
			}

			std::vector<ModelRenderer>& models = m_ModelRenderers->GetAllComponents();

			ComponentTable<ITransform>* tableTransform = m_SceneRef->GetComponentTable<ITransform>();
			std::vector<ITransform>& transforms = tableTransform->GetAllComponents();

			m_IndexCastShadow.clear();

			m_OpaqueLitObjects.clear();
			m_OpaqueUnlitObjects.clear();
			m_TransparentObjects.clear();

			size_t size = m_ModelRenderers->ComponentCount();
			for (size_t i = 0; i < size; ++i)
			{
				if (!models[i].IsEnable())
					continue;

				if (models[i].IsCastingShadow())
					m_IndexCastShadow.push_back(i);

				models[i].SetMatrix(transforms[tableTransform->ContainsEntity(models[i].GetIrisObject())].GetGlobalTRS());

				std::vector<ChildMesh> meshData = *models[i].GetMesh()->GetMeshData();
				size_t meshCount = meshData.size();

				const std::vector<Material*> materials = models[i].GetMaterials();

				for (size_t j = 0; j < meshCount; ++j)
				{
					size_t matIndex = meshData[j].MaterialIndex >= materials.size() ? 0 : meshData[j].MaterialIndex;
					Material* mat = materials[matIndex];

					DrawCommand cmd{};
					cmd.Pipeline = mat->GetMaterialPipeline();
					cmd.VertexBuffer = meshData[j].VertexBuffer;
					cmd.IndexBuffer = meshData[j].IndexBuffer;
					cmd.IndicesCount = meshData[j].IndicesCount;
					cmd.ModelRendererIndex = static_cast<uint32_t>(i);
					cmd.Material = mat;

					if (mat->IsOpaque() && mat->IsLit())
						m_OpaqueLitObjects.push_back(cmd);
					else if (mat->IsOpaque() && mat->IsUnlit())
						m_OpaqueUnlitObjects.push_back(cmd);
					else
						m_TransparentObjects.push_back(cmd);
				}
			}

			std::vector<LightComponent>& lights = m_Lights->GetAllComponents();

			m_IndexDirectionalLights.clear();
			m_IndexSpotLights.clear();
			m_IndexPointLights.clear();

			size = m_Lights->ComponentCount();
			for (size_t i = 0; i < size; ++i)
			{
				switch (lights[i].GetType())
				{
				case LightType::IE_DIRECTIONAL_LIGHT: default:
					m_IndexDirectionalLights.push_back(i);
					break;
				case LightType::IE_SPOT_LIGHT:
					m_IndexSpotLights.push_back(i);
					break;
				case LightType::IE_POINT_LIGHT:
					m_IndexPointLights.push_back(i);
					break;
				}
			}
		}

		void RenderSystem::UpdateLightData(IRenderer* _renderer)
		{
			SendLightData(_renderer);
		}

		void RenderSystem::Destroy()
		{
			m_SceneRef = nullptr;
			m_Lights = nullptr;
			m_ModelRenderers = nullptr;

			m_Skybox->Destroy();
			delete m_Skybox;
		}

		void RenderSystem::Clear()
		{
			m_SceneRef = nullptr;
			m_Lights = nullptr;
			m_ModelRenderers = nullptr;
		}

		void RenderSystem::RenderShadow(IRenderer* _renderer)
		{
			std::vector<ModelRenderer>& models = m_ModelRenderers->GetAllComponents();

			size_t sizeCastShadow = m_IndexCastShadow.size();
			for (size_t i = 0; i < sizeCastShadow; ++i)
				models[m_IndexCastShadow[i]].RenderMesh(_renderer);
		}

		void RenderSystem::RenderOpaque(IRenderer* _renderer, const CameraComponent& _cameraComponent, SkyboxObject* _skybox)
		{
			std::vector<ModelRenderer>& models = m_ModelRenderers->GetAllComponents();

			size_t sizeOpaqueLit = m_OpaqueLitObjects.size();
			for (size_t i = 0; i < sizeOpaqueLit; ++i)
			{
				size_t mrIndex = m_OpaqueLitObjects[i].ModelRendererIndex;
				models[mrIndex].RenderDrawCommand(_renderer, m_OpaqueLitObjects[i], _cameraComponent, _skybox);
			}
		}

		void RenderSystem::RenderTransparent(IRenderer* _renderer, const CameraComponent& _cameraComponent, SkyboxObject* _skybox)
		{
			std::vector<ModelRenderer>& models = m_ModelRenderers->GetAllComponents();

			size_t sizeOpaqueUnlit = m_OpaqueUnlitObjects.size();
			for (size_t i = 0; i < sizeOpaqueUnlit; ++i)
			{
				size_t mrIndex = m_OpaqueUnlitObjects[i].ModelRendererIndex;
				models[mrIndex].RenderDrawCommand(_renderer, m_OpaqueUnlitObjects[i], _cameraComponent, _skybox);
			}

			size_t sizeTransparent = m_TransparentObjects.size();
			for (size_t i = 0; i < sizeTransparent; ++i)
			{
				size_t mrIndex = m_TransparentObjects[i].ModelRendererIndex;
				models[mrIndex].RenderDrawCommand(_renderer, m_TransparentObjects[i], _cameraComponent, _skybox);
			}
		}

		void RenderSystem::RenderColliders(IRenderer* _renderer)
		{
			if (!m_SceneRef->GetSelectedObject())
				return;

			Collider* col = m_SceneRef->GetSelectedObject()->GetComponent<Collider>();

			if (col && col->IsEnable())
			{
				std::vector<Mesh*> meshes = col->GetMesh();
				std::vector<Matrix4> trs = col->GetPhysicTRS();

				size_t size = meshes.size();
				for(size_t i = 0; i<size ; ++i)
					_renderer->DrawCollider(trs[i], Vector4(1.f, 0.5f, 0.f, 1.f), meshes[i]);
			}
		}

		std::vector<LightComponent> RenderSystem::GetDirectionalLights()
		{
			std::vector<LightComponent>& lc = m_Lights->GetAllComponents();
			size_t sizeDirLights = m_IndexDirectionalLights.size();
			std::vector<LightComponent> returnArray(sizeDirLights);

			for (size_t i = 0; i < sizeDirLights; ++i)
			{
				returnArray[i] = (lc[m_IndexDirectionalLights[i]]);
			}

			return returnArray;
		}

		std::vector<LightComponent> RenderSystem::GetSpotLights()
		{
			std::vector<LightComponent>& lc = m_Lights->GetAllComponents();
			size_t sizeSpotLights = m_IndexSpotLights.size();
			std::vector<LightComponent> returnArray(sizeSpotLights);

			for (size_t i = 0; i < sizeSpotLights; ++i)
			{
				returnArray[i] = (lc[m_IndexSpotLights[i]]);
			}

			return returnArray;
		}

		std::vector<LightComponent> RenderSystem::GetPointLights()
		{
			std::vector<LightComponent>& lc = m_Lights->GetAllComponents();
			size_t sizePointLights = m_IndexPointLights.size();
			std::vector<LightComponent> returnArray(sizePointLights);

			for (size_t i = 0; i < sizePointLights; ++i)
			{
				returnArray[i] = (lc[m_IndexPointLights[i]]);
			}

			return returnArray;
		}

		void RenderSystem::SendLightData(IRenderer* _renderer)
		{
			std::vector<LightComponent>& lst = m_Lights->GetAllComponents();
			SceneLightData lightData{};

			int dirLightIndex = 0;
			int spotLightIndex = 0;
			int pointLightIndex = 0;

			size_t size = m_Lights->ComponentCount();
			for (size_t i = 0; i < size; ++i)
			{
				if (!lst[i].IsEnable())
					continue;

				switch (lst[i].GetType())
				{
				case LightType::IE_DIRECTIONAL_LIGHT:
					lightData.DirectionalLight[dirLightIndex] = GetDirectionalLightData(&lst[i]);
					++dirLightIndex;
					break;
				case LightType::IE_POINT_LIGHT:
					lightData.PointlLight[pointLightIndex] = GetPointLightData(&lst[i]);
					++pointLightIndex;
					break;
				case LightType::IE_SPOT_LIGHT:
					lightData.SpotLight[spotLightIndex] = GetSpotLightData(&lst[i]);
					++spotLightIndex;
					break;
				}
			}

			lightData.DirLightCount = dirLightIndex;
			lightData.SpotLightCount = spotLightIndex;
			lightData.PointLightCount = pointLightIndex;

			_renderer->UpdateLightBuffer(&lightData, sizeof(lightData));
		}

		DirectionalLightData RenderSystem::GetDirectionalLightData(LightComponent* _lightComponent)
		{
			DirectionalLightData dlightData{};

			Vector3 forward = _lightComponent->GetIrisObject()->GetTransform()->GetForward();
			_lightComponent->SetDirection(forward);
			_lightComponent->SetPosition(_lightComponent->GetIrisObject()->GetTransform()->GetPosition());

			dlightData.Direction = Vector4(forward[0], forward[1], forward[2], 0.f);
			dlightData.Color = _lightComponent->GetColor();
			dlightData.Insensity = _lightComponent->GetIntensity();
			dlightData.LightNear = _lightComponent->GetNear();
			dlightData.LightFar = _lightComponent->GetFar();

			Vector3 position = _lightComponent->GetDirection() * -1.f * 50.f;

			dlightData.LightSpaceMatrix = Matrix4::ProjectionOrthographicMatrix(
				dlightData.LightNear,
				dlightData.LightFar,
				_lightComponent->GetTop(),
				_lightComponent->GetBottom(),
				_lightComponent->GetRight(),
				_lightComponent->GetLeft()) * Matrix4::LookAtMatrix(position, _lightComponent->GetDirection(), Vector3::s_Up);

			dlightData.CalculateShadow = _lightComponent->CalculateShadow();

			return dlightData;
		}

		PointLightData RenderSystem::GetPointLightData(LightComponent* _lightComponent)
		{
			PointLightData plightData{};

			Vector3 position = _lightComponent->GetIrisObject()->GetTransform()->GetGlobalPosition();
			_lightComponent->SetPosition(position);

			plightData.Position = Vector4(position[0], position[1], position[2], 1.f);
			plightData.Color = _lightComponent->GetColor();
			plightData.Insensity = _lightComponent->GetIntensity();
			plightData.LightNear = _lightComponent->GetNear();
			plightData.LightFar = _lightComponent->GetFar();
			plightData.CalculateShadow = _lightComponent->CalculateShadow();

			return plightData;
		}

		SpotLightData RenderSystem::GetSpotLightData(LightComponent* _lightComponent)
		{
			SpotLightData splightData{};

			Vector3 position = _lightComponent->GetIrisObject()->GetTransform()->GetGlobalPosition();
			Vector3 forward = _lightComponent->GetIrisObject()->GetTransform()->GetForward();
			_lightComponent->SetPosition(position);
			_lightComponent->SetDirection(forward);

			splightData.Position = Vector4(position[0], position[1], position[2], 1.f);
			splightData.Direction = Vector4(forward[0], forward[1], forward[2], 0.f);;
			splightData.Color = _lightComponent->GetColor();
			splightData.Insensity = _lightComponent->GetIntensity();
			splightData.LightNear = _lightComponent->GetNear();
			splightData.LightFar = _lightComponent->GetFar();
			splightData.CutOff = _lightComponent->GetCutOff() * Deg2Rad;
			splightData.OuterCutOff = _lightComponent->GetOuterCutOff() * Deg2Rad;
			splightData.LightSpaceMatrix = Matrix4::ProjectionPerspectiveMatrix(splightData.LightNear, splightData.LightFar, 1.f, 90.f * Deg2Rad) * Matrix4::LookAtMatrix(position, position + _lightComponent->GetDirection(), Vector3::s_Up);
			splightData.CalculateShadow = _lightComponent->CalculateShadow();

			return splightData;
		}
	}
}