#include "SECS/Components/ModelRenderer.h"
#include "SECS/Components/Transform.h"

#include "Core/Tools/Log.h"
#include "SECS/IrisObject.h"

#include "Matrix/Matrix3.h"

#include "Core/Graphics/Renderer/IRenderer.h"
#include "Core/Graphics/Context/ResourceContext.h"
#include "Core/Graphics/RHI/IBuffer.h"
#include "Core/Graphics/RHI/IDescriptor.h"
#include "Core/Graphics/RHI/ISwapChain.h"
#include "Core/Graphics/RHI/IPipeline.h"
#include "Core/Graphics/RHI/IDevice.h"
#include "Resources/ResourceManager.h"

#include "Resources/Metadata.h"
#include "Resources/ResourceManager.h"

#include <unordered_map>

namespace Iris
{
	namespace Engine
	{
		ModelRenderer::~ModelRenderer()
		{
			m_ModelMesh = nullptr;
		}

		void ModelRenderer::Serialize(IMetadata* _metadata)
		{
			_metadata->Write("Enable", p_Enable);

			_metadata->Write("Cast Shadow", m_CastShadow);
			
			std::vector<std::string> matNames;
			size_t size = m_ModelMaterials.size();
			for (size_t i = 0; i < size;++i)
				matNames.push_back(m_ModelMaterials[i]->GetName());

			_metadata->Write("Materials", matNames);

			_metadata->Write("Mesh", m_ModelMesh->GetName());
		}

		void ModelRenderer::Deserialization(IMetadata* _metadata, InitEditorContext& _initializer)
		{
			m_ResourceContext = _initializer.InitResourceContext;
			m_Pipeline = _initializer.InitPipeline;

			std::vector<std::string> path = { "Scene Data", "Components", std::to_string(static_cast<int>(GetID())), GetName(), std::to_string(p_IrisObject->GetID()), "" };

			path[5] = "Cast Shadow";
			m_CastShadow = _metadata->GetData(path);

			path[5] = "Mesh";
			m_ModelMesh = ResourceManager::GetInstance()->Create<Mesh>(FIND_PATH(_metadata->GetData(path)));

			path[5] = "Materials";
			std::vector<std::string> matNames = _metadata->GetData(path);
			ResourceManager* rm = ResourceManager::GetInstance();
			size_t size = matNames.size();
			if(size == 0)
				m_ModelMaterials.push_back(rm->Get<Material>("Default Material"));

			for (size_t i = 0; i < size;++i)
				m_ModelMaterials.push_back(rm->Get<Material>(matNames[i]));


			path[5] = "Enable";
			p_Enable = _metadata->GetData(path);

			MeshDataUBO data{};
			m_TransformBuffers = m_ResourceContext->CreateBuffers(RHIBufferType::IE_RHI_UNIFORM_BUFFER, sizeof(data), &data);
			m_TransformDescriptors = m_ResourceContext->CreateBufferDescriptors(m_Pipeline->GetDescriptorLayout(0), m_TransformBuffers, sizeof(data), 0);
		}

		void ModelRenderer::SetMesh(Mesh*  _mesh)
		{
			m_ModelMesh = _mesh;

			if (m_ModelMaterials.size() == 0)
			{
				GetDefaultMaterials();
				return;
			}

			std::vector<Material*> mat = m_ModelMaterials;
			size_t sizeBefore = mat.size();

			GetDefaultMaterials();

			size_t newSize = m_ModelMaterials.size();
			for (size_t i = 0; i < newSize; ++i)
			{
				if (i == sizeBefore)
					break;

				m_ModelMaterials[i] = mat[i];
			}
		}

		void ModelRenderer::SetMaterial(Material* _material, size_t _index)
		{
			size_t index = _index < 0 || _index > m_ModelMaterials.size() - 1 ? 0 : _index;

			m_ModelMaterials[index] = _material;
		}

		void ModelRenderer::Init(ResourceContext* _resourceContext, Mesh* _mesh, IPipeline* _pipeline)
		{
			m_ResourceContext = _resourceContext;
			m_Pipeline = _pipeline;
			m_ModelMesh = _mesh;

			MeshDataUBO data{};
			data.Model = p_IrisObject->GetTransform()->GetGlobalTRS();
			data.NormalMatrix = p_IrisObject->GetTransform()->GetGlobalTRS();

			m_TransformBuffers = _resourceContext->CreateBuffers(RHIBufferType::IE_RHI_UNIFORM_BUFFER, sizeof(data), &data);
			m_TransformDescriptors = _resourceContext->CreateBufferDescriptors(m_Pipeline->GetDescriptorLayout(0), m_TransformBuffers, sizeof(data), 0);

			GetDefaultMaterials();
		}

		void ModelRenderer::Init(InitEditorContext& _initializer)
		{
			Init(_initializer.InitResourceContext, _initializer.InitMesh, _initializer.InitPipeline);
		}

		void ModelRenderer::Copy(IComponent* _componentToCopy)
		{
			ModelRenderer* mR = dynamic_cast<ModelRenderer*>(_componentToCopy);
			if (mR)
			{
				IComponent::Copy(_componentToCopy);

				m_ResourceContext = mR->m_ResourceContext;
				m_ModelMesh = mR->m_ModelMesh;
				m_Pipeline = mR->m_Pipeline;
				m_ModelMaterials = mR->m_ModelMaterials;
				m_TransformBuffers = mR->m_TransformBuffers;
				m_TransformDescriptors = mR->m_TransformDescriptors;
				m_CastShadow = mR->m_CastShadow;
				m_ModelMaterials = mR->m_ModelMaterials;
			}
			else
			{
				DEBUG_LOG("cant copy this component ");
			}
		}

		void ModelRenderer::Duplicate(IComponent* _componentToDuplicate, InitEditorContext* _initializer)
		{
			ModelRenderer* mR = dynamic_cast<ModelRenderer*>(_componentToDuplicate);
			if (mR)
			{
				IComponent::Duplicate(_componentToDuplicate, _initializer);

				Init(_initializer->InitResourceContext, mR->m_ModelMesh, mR->m_Pipeline);

				m_CastShadow = mR->m_CastShadow;

				m_ModelMaterials = mR->m_ModelMaterials;
			}
		}

		void ModelRenderer::Destroy()
		{
			p_IrisObject = nullptr;

			if (m_TransformBuffers.size() <= 0)
				return;

			m_ResourceContext->GetDevice()->WaitIdle();

			m_ResourceContext->DestroyBuffers(m_TransformBuffers);
			m_ResourceContext->DeleteDescriptors(m_TransformDescriptors);
		}

		void ModelRenderer::RenderDrawCommand(IRenderer* _renderer, const DrawCommand& _command, const CameraComponent& _camera, SkyboxObject* _skybox)
		{
			MeshDataUBO data{};
			data.Model = m_Model;
			data.NormalMatrix = m_Model.Inverse().Transpose();

			_renderer->UpdateBuffer(m_TransformBuffers, &data, sizeof(data));

			_renderer->StartDrawCommand(_command.Pipeline, _camera, _skybox, _command.Material->IsOpaque());

			_renderer->BindDescriptor(_renderer->GetCurrentPipeline(), m_TransformDescriptors, 1);

			_renderer->DrawModel(_command.VertexBuffer, _command.IndexBuffer, _command.IndicesCount, _command.Material);
		}

		void ModelRenderer::RenderMesh(IRenderer* _renderer)
		{
			MeshDataUBO data{};
			data.Model = m_Model;
			data.NormalMatrix = m_Model.Inverse().Transpose();

			_renderer->BindBlockVariable(_renderer->GetCurrentPipeline(), sizeof(data), &data, RHIShaderType::IE_RHI_SHADER_TYPE_VERTEX);

			_renderer->DrawMesh(_renderer->GetCurrentCommandBuffer(), m_ModelMesh);
		}

		void ModelRenderer::RenderModel(IRenderer* _renderer)
		{
			MeshDataUBO data{};
			data.Model = m_Model;
			data.NormalMatrix = m_Model.Inverse().Transpose();

			_renderer->UpdateBuffer(m_TransformBuffers, &data, sizeof(data));

			_renderer->DrawModel(m_ModelMesh, m_ModelMaterials);
		}

		void ModelRenderer::GetDefaultMaterials()
		{
			m_ModelMaterials.clear();

			std::unordered_map<uint32_t, Material*> uniqueMat{};
			std::vector<ChildMesh> mData = *m_ModelMesh->GetMeshData();

			ResourceManager* mg = ResourceManager::GetInstance();

			size_t meshCount = mData.size();
			for (size_t i = 0; i < meshCount; ++i)
			{
				if (uniqueMat.find(mData[i].MaterialIndex) == uniqueMat.end())
				{
					Material* mat = mg->Get<Material>(mData[i].MaterialID);

					if (!mat)
					{
						mat = mg->Get<Material>("Default Material");
					}

					uniqueMat.try_emplace(mData[i].MaterialIndex, mat);
					m_ModelMaterials.push_back(mat);
				}
			}
		}
	}
}