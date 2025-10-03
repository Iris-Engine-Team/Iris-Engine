#include "SECS/Components/LightComponent.h"
#include "Core/Graphics/Context/ResourceContext.h"
#include "Core/Graphics/Renderer/IRenderer.h"
#include "Core/Graphics/RHI/IPipeline.h"
#include "Core/Tools/Log.h"

#include "Resources/Metadata.h"

namespace Iris
{
	namespace Engine
	{
		void LightComponent::Serialize(IMetadata* _metadata)
		{
			_metadata->Write("Enable", p_Enable);

			_metadata->Write("Type", m_LightType);
			
			std::vector<float> vec4 = { m_Color[0], m_Color[1], m_Color[2], m_Color[3] };
			_metadata->Write("Color", vec4);

			_metadata->Write("Intensity", m_Intensity);

			_metadata->Write("Near", m_Near);
			_metadata->Write("Far", m_Far);
			_metadata->Write("CutOff", m_CutOff);
			_metadata->Write("Outer CutOff", m_OuterCutOff);

			std::vector<float> frustrumDirLight = { m_Top, m_Bottom, m_Left, m_Right };
			_metadata->Write("Frustrum", frustrumDirLight);
		}

		void LightComponent::Deserialization(IMetadata* _metadata, InitEditorContext& _initializer)
		{
			std::vector<std::string> path = { "Scene Data", "Components", std::to_string(static_cast<uint16_t>(GetID())), GetName(), std::to_string(p_IrisObject->GetID()), "" };
			
			path[5] = "Type";
			m_LightType = _metadata->GetData(path);

			path[5] = "Color";
			std::vector<float> vec4 = _metadata->GetData(path);
			m_Color = Vector4(vec4[0], vec4[1], vec4[2], vec4[3]);

			path[5] = "Intensity";
			m_Intensity = _metadata->GetData(path);

			path[5] = "Near";
			m_Near = _metadata->GetData(path);

			path[5] = "Far";
			m_Far = _metadata->GetData(path);

			path[5] = "CutOff";
			m_CutOff = _metadata->GetData(path);

			path[5] = "Outer CutOff";
			m_OuterCutOff = _metadata->GetData(path);

			path[5] = "Frustrum";
			std::vector<float> frustrumDirLight = _metadata->GetData(path); 
			m_Top = frustrumDirLight[0];
			m_Bottom = frustrumDirLight[1];
			m_Left = frustrumDirLight[2];
			m_Right = frustrumDirLight[3];

			path[5] = "Enable";
			p_Enable = _metadata->GetData(path);

			m_ResourceContextRef = _initializer.InitResourceContext;
			m_RenderingContextRef = _initializer.InitRenderingContext;

			if (m_CalculateShadow)
			{
				switch (m_LightType)
				{
				case Iris::Engine::LightType::IE_DIRECTIONAL_LIGHT: default:
					m_LightShadowMap.CreateDirectionalShadowMap(
						_initializer.InitRenderer->GetLightPipeline(),
						_initializer.InitRenderer->GetDirectionalShadowPipeline(),
						_initializer.InitRenderer->GetShadowRenderPass(), 
						_initializer.InitRenderingContext, 
						_initializer.InitResourceContext, _initializer.InitRenderer->GetShadowLayoutIndex());
					break;
				case Iris::Engine::LightType::IE_POINT_LIGHT:
					m_LightShadowMap.CreatePointShadowMap(
						_initializer.InitRenderer->GetLightPipeline(),
						_initializer.InitRenderer->GetPointShadowPipeline(),
						_initializer.InitRenderer->GetShadowRenderPass(),
						_initializer.InitRenderingContext,
						_initializer.InitResourceContext, _initializer.InitRenderer->GetShadowLayoutIndex());
					break;
				case Iris::Engine::LightType::IE_SPOT_LIGHT:
					m_LightShadowMap.CreateSpotShadowMap(
						_initializer.InitRenderer->GetLightPipeline(),
						_initializer.InitRenderer->GetDirectionalShadowPipeline(),
						_initializer.InitRenderer->GetShadowRenderPass(),
						_initializer.InitRenderingContext,
						_initializer.InitResourceContext, _initializer.InitRenderer->GetShadowLayoutIndex());
					break;
				}
			}
		}

		void LightComponent::Init(InitEditorContext& _initializer)
		{
			Init(*_initializer.InitRenderer, *_initializer.InitResourceContext, *_initializer.InitRenderingContext);
		}

		void LightComponent::Init(IRenderer& _renderer, ResourceContext& _resourceContext, RenderingContext& _renderingContext)
		{
			m_ResourceContextRef = &_resourceContext;
			m_RenderingContextRef = &_renderingContext;

			if (m_CalculateShadow)
			{
				switch (m_LightType)
				{
				case Iris::Engine::LightType::IE_DIRECTIONAL_LIGHT: default:
					m_LightShadowMap.CreateDirectionalShadowMap(
						_renderer.GetLightPipeline(),
						_renderer.GetDirectionalShadowPipeline(),
						_renderer.GetShadowRenderPass(),
						&_renderingContext,
						&_resourceContext, _renderer.GetShadowLayoutIndex());
					break;
				case Iris::Engine::LightType::IE_POINT_LIGHT:
					m_LightShadowMap.CreatePointShadowMap(
						_renderer.GetLightPipeline(),
						_renderer.GetPointShadowPipeline(),
						_renderer.GetShadowRenderPass(),
						&_renderingContext,
						&_resourceContext, _renderer.GetShadowLayoutIndex());
					break;
				case Iris::Engine::LightType::IE_SPOT_LIGHT:
					m_LightShadowMap.CreateSpotShadowMap(
						_renderer.GetLightPipeline(),
						_renderer.GetDirectionalShadowPipeline(),
						_renderer.GetShadowRenderPass(),
						&_renderingContext,
						&_resourceContext, _renderer.GetShadowLayoutIndex());
					break;
				}
			}
		}

		void LightComponent::Copy(IComponent* _componentToCopy)
		{
			LightComponent* l = dynamic_cast<LightComponent*>(_componentToCopy);
			if (l)
			{
				IComponent::Copy(_componentToCopy);
				
				m_ResourceContextRef = l->m_ResourceContextRef;
				m_RenderingContextRef = l->m_RenderingContextRef;

				m_Position = l->m_Position;
				m_Direction = l->m_Direction;

				m_LightType = l->m_LightType;

				m_Color = l->m_Color;

				m_Intensity = l->m_Intensity;

				m_Near = l->m_Near;
				m_Far = l->m_Far;

				m_CutOff = l->m_CutOff;
				m_OuterCutOff = l->m_OuterCutOff;

				m_Top = l->m_Top;
				m_Bottom = l->m_Bottom;
				m_Left = l->m_Left;
				m_Right = l->m_Right;

				m_LightShadowMap = l->m_LightShadowMap;
				m_CalculateShadow = l->m_CalculateShadow;
				m_ShadowIndex = l->m_ShadowIndex;
				m_LightShadowMap = l->m_LightShadowMap;
			}
			else
			{
				DEBUG_LOG("cant copy this component ");
			}
		}

		void LightComponent::SetType(LightType _newType)
		{
			m_LightType = _newType;
			m_UpdateRequired = true;
		}

		void LightComponent::UpdateComponent(IRenderer& _renderer, RenderingContext& _renderingContext, ResourceContext& _resourceContext)
		{
			if (!m_UpdateRequired)
				return;

			m_UpdateRequired = false;

			m_LightShadowMap.DestroyShadowMap(&_renderingContext, &_resourceContext);

			if (m_CalculateShadow)
			{
				switch (m_LightType)
				{
				case Iris::Engine::LightType::IE_DIRECTIONAL_LIGHT: default:
					m_LightShadowMap.CreateDirectionalShadowMap(
						_renderer.GetLightPipeline(),
						_renderer.GetDirectionalShadowPipeline(),
						_renderer.GetShadowRenderPass(),
						&_renderingContext,
						&_resourceContext, _renderer.GetShadowLayoutIndex());
					break;
				case Iris::Engine::LightType::IE_POINT_LIGHT:
					m_LightShadowMap.CreatePointShadowMap(
						_renderer.GetLightPipeline(),
						_renderer.GetPointShadowPipeline(),
						_renderer.GetShadowRenderPass(),
						&_renderingContext,
						&_resourceContext, _renderer.GetShadowLayoutIndex());
					break;
				case Iris::Engine::LightType::IE_SPOT_LIGHT:
					m_LightShadowMap.CreateSpotShadowMap(
						_renderer.GetLightPipeline(),
						_renderer.GetDirectionalShadowPipeline(),
						_renderer.GetShadowRenderPass(),
						&_renderingContext,
						&_resourceContext, _renderer.GetShadowLayoutIndex());
					break;
				}
			}
		}

		void LightComponent::Duplicate(IComponent* _componentToDuplicate, InitEditorContext* _initializer)
		{
			LightComponent* l = dynamic_cast<LightComponent*>(_componentToDuplicate);
			if (l)
			{
				IComponent::Duplicate(_componentToDuplicate, _initializer);
				Init(*_initializer->InitRenderer, *_initializer->InitResourceContext, *_initializer->InitRenderingContext);

				m_Position = l->m_Position;
				m_Direction = l->m_Direction;

				m_LightType = l->m_LightType;

				m_Color = l->m_Color;

				m_Intensity = l->m_Intensity;

				m_Near = l->m_Near;
				m_Far = l->m_Far;

				m_Top = l->m_Top;
				m_Bottom = l->m_Bottom;
				m_Left = l->m_Left;
				m_Right = l->m_Right;


				m_CutOff = l->m_CutOff;
				m_OuterCutOff = l->m_OuterCutOff;
				m_LightShadowMap = l->m_LightShadowMap;
				m_CalculateShadow = l->m_CalculateShadow;
				m_ShadowIndex = l->m_ShadowIndex;
				m_LightShadowMap = l->m_LightShadowMap;
			}
		}

		void LightComponent::UpdateShadowMapData(IRenderer& _renderer)
		{
			switch (m_LightType)
			{
			case Iris::Engine::LightType::IE_DIRECTIONAL_LIGHT: default:
				m_LightShadowMap.UpdateDirectionalData(&_renderer, m_Position, m_Direction, m_Near, m_Far, m_Top, m_Bottom, m_Right, m_Left);
				break;
			case Iris::Engine::LightType::IE_POINT_LIGHT:
				m_LightShadowMap.UpdatePointData(&_renderer, m_Position, m_Near, m_Far);
				break;
			case Iris::Engine::LightType::IE_SPOT_LIGHT:
				m_LightShadowMap.UpdateSpotData(&_renderer, m_Position, m_Direction, m_Near, m_Far);
				break;
			}
		}

		void LightComponent::Destroy()
		{
			if (m_CalculateShadow)
			{
				m_LightShadowMap.DestroyShadowMap(m_RenderingContextRef, m_ResourceContextRef);
			}
		}
	}
}