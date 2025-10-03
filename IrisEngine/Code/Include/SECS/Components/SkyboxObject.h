#pragma once
#include "Core/Tools/Macros.h"
#include "Vector/Vector3.h"

#include "Resources/Texture.h"

namespace Iris
{
	namespace Engine
	{
		class Texture;
		class IDescriptor;

		class ContextInitializer;
		class IRenderer;

		class SkyboxObject
		{
		public:
			void Init(ContextInitializer* _contextInitializer, IRenderer* _render);

			IRIS_ENGINE_API void Destroy();
			IRIS_ENGINE_API void SetTexture(std::string _textureName);
			IRIS_ENGINE_API IE_FORCE_INLINE const Texture& GetTexture() const { return m_SkyboxTexture; }

			IRIS_ENGINE_API IE_FORCE_INLINE IDescriptor* GetSkyboxDescriptor() const { return m_Descriptor; }
			IRIS_ENGINE_API IE_FORCE_INLINE IDescriptor* GetIBLDescriptor() const { return m_IBLDescriptor; }

			IRIS_ENGINE_API IE_FORCE_INLINE Vector3 GetRotation() const { return m_Rotation; }
			IRIS_ENGINE_API IE_FORCE_INLINE void SetRotation(Vector3 _rotation) { m_Rotation = _rotation; }

		private:

			ContextInitializer* m_ContextInitializer = nullptr;
			IRenderer* m_Renderer = nullptr;

			Texture m_SkyboxTexture;
			IDescriptor* m_Descriptor = nullptr;


			Texture m_IBLIrradiance;
			Texture m_IBLPrefiltered;
			Texture m_IBLLookUp;

			IDescriptor* m_IBLDescriptor = nullptr;

			Vector3 m_Rotation;
		};
	}
}