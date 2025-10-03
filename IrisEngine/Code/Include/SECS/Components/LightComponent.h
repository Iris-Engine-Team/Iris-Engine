#pragma once

#include "SECS/Components/IComponent.h"
#include "Vector/Vector4.h"
#include "Core/Graphics/Renderer/Shadows/ShadowMap.h"

namespace Iris
{
	namespace Engine
	{
		enum class LightType : int
		{
			IE_DIRECTIONAL_LIGHT = 0,
			IE_POINT_LIGHT = 1,
			IE_SPOT_LIGHT = 2
		};

		class IBuffer;

		class LightComponent : public IComponent
		{
		public:

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
			//							Component DEF							 //
			DEFINE_COMPONENT(LightComponent);
			static IE_FORCE_INLINE std::string GetName() { return "Light"; }
			static IE_FORCE_INLINE ComponentID GetID() { return ComponentID::IE_LIGHT; }
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

			LightComponent() = default;
			~LightComponent() override = default;

			/// <summary>
			/// Writes data in the given metadata
			/// </summary>
			/// <param name="_metadata"></param>
			IRIS_ENGINE_API void Serialize(IMetadata* _metadata) final override;

			/// <summary>
			/// Init the components with the given data
			/// </summary>
			IRIS_ENGINE_API void Deserialization(IMetadata* _metadata, InitEditorContext& _initializer) final override;

			/// <summary>
			/// Init function called from the editor
			/// </summary>
			IRIS_ENGINE_API void Init(InitEditorContext& _initializer) override;
			
			void Init(IRenderer& _renderer, ResourceContext& _resourceContext, RenderingContext& _renderingContext);

			IRIS_ENGINE_API void Copy(IComponent* _componentToCopy) final override;
			IRIS_ENGINE_API void Duplicate(IComponent* _componentToDuplicate, InitEditorContext* _initializer) final override;
			void Destroy() final override;

			IRIS_ENGINE_API void UpdateComponent(IRenderer& _renderer, RenderingContext& _renderingContext, ResourceContext& _resourceContext);

			/// <summary>
			/// Update light data for shadow map
			/// </summary>
			void UpdateShadowMapData(IRenderer& _renderer);

			IRIS_ENGINE_API IE_FORCE_INLINE Vector4 GetColor() { return m_Color; }
			IRIS_ENGINE_API IE_FORCE_INLINE Vector4* GetColorPtr() { return &m_Color; }
			IRIS_ENGINE_API IE_FORCE_INLINE void SetColor(Vector4 _newColor) { m_Color = _newColor; }

			IRIS_ENGINE_API IE_FORCE_INLINE float GetIntensity() const { return m_Intensity; }
			IRIS_ENGINE_API IE_FORCE_INLINE void SetIntensity(float _newIntensity) { m_Intensity = _newIntensity; }

			IRIS_ENGINE_API IE_FORCE_INLINE float GetNear() const { return m_Near; }
			IRIS_ENGINE_API IE_FORCE_INLINE void SetNear(float _newNear) { m_Near = _newNear; }

			IRIS_ENGINE_API IE_FORCE_INLINE float GetFar() const { return m_Far; }
			IRIS_ENGINE_API IE_FORCE_INLINE void SetFar(float _newFar) { m_Far = _newFar; }

			IRIS_ENGINE_API IE_FORCE_INLINE float GetRight() const { return m_Right; }
			IRIS_ENGINE_API IE_FORCE_INLINE void SetRight(float _newRight) { m_Right = _newRight; }

			IRIS_ENGINE_API IE_FORCE_INLINE float GetLeft() const { return m_Left; }
			IRIS_ENGINE_API IE_FORCE_INLINE void SetLeft(float _newLeft) { m_Left = _newLeft; }

			IRIS_ENGINE_API IE_FORCE_INLINE float GetTop() const { return m_Top; }
			IRIS_ENGINE_API IE_FORCE_INLINE void SetTop(float _newTop) { m_Top = _newTop; }

			IRIS_ENGINE_API IE_FORCE_INLINE float GetBottom() const { return m_Bottom; }
			IRIS_ENGINE_API IE_FORCE_INLINE void SetBottom(float _newBottom) { m_Bottom = _newBottom; }

			IRIS_ENGINE_API IE_FORCE_INLINE float GetCutOff() const { return m_CutOff; }
			IRIS_ENGINE_API IE_FORCE_INLINE void SetCutOff(float _newCutOff) { m_CutOff = _newCutOff; }

			IRIS_ENGINE_API IE_FORCE_INLINE float GetOuterCutOff()const { return m_OuterCutOff; }
			IRIS_ENGINE_API IE_FORCE_INLINE void SetOuterCutOff(float _newOuterCutOff) { m_OuterCutOff = _newOuterCutOff; }

			IRIS_ENGINE_API IE_FORCE_INLINE LightType GetType() const { return m_LightType; }
			IRIS_ENGINE_API void SetType(LightType _newType);

			/// <returns>The world position</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE Vector3 GetPosition() const { return m_Position; }

			IRIS_ENGINE_API IE_FORCE_INLINE ShadowMap* GetShadowMap() { return &m_LightShadowMap; }
			IRIS_ENGINE_API IE_FORCE_INLINE bool CalculateShadow() const { return m_CalculateShadow; }
			IRIS_ENGINE_API IE_FORCE_INLINE void SetCalculateShadow(bool _calculateShadow) { m_CalculateShadow = _calculateShadow; }

			/// <param name="_newPosition">: world posiiton</param>
			IRIS_ENGINE_API IE_FORCE_INLINE void SetPosition(Vector3 _newPosition) { m_Position = _newPosition; }
			/// <returns>The world forward</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE Vector3 GetDirection() const { return m_Direction; }

			/// <param name="_newDirection">: world forward vector</param>
			IRIS_ENGINE_API IE_FORCE_INLINE void SetDirection(Vector3 _newDirection) { m_Direction = _newDirection; }

			IRIS_ENGINE_API IE_FORCE_INLINE bool UpdateRequired() const { return m_UpdateRequired; }

		private:

			bool m_CalculateShadow = true;

			bool m_UpdateRequired = false;

			Vector3 m_Position = Vector3::s_Zero;
			Vector3 m_Direction = Vector3::s_Zero;

			LightType m_LightType = LightType::IE_POINT_LIGHT;

			Vector4 m_Color = Vector4(1.f);

			int m_ShadowIndex = -1;

			float m_Intensity = 5.f;

			float m_Near = 0.01f;
			float m_Far = 10.f;

			float m_CutOff = 12.5f;
			float m_OuterCutOff = 15.f;

			float m_Right = 10.f;
			float m_Left = -10.f;
			float m_Top = 10.f;
			float m_Bottom = -10.f;

			ShadowMap m_LightShadowMap{};

			RenderingContext* m_RenderingContextRef = nullptr;
			ResourceContext* m_ResourceContextRef = nullptr;
		};
	}
}