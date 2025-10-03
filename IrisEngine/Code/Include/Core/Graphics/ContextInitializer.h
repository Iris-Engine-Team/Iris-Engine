#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Graphics/RHI/CommonTypes.h"

#include "Core/Graphics/Context/RenderingContext.h"
#include "Core/Graphics/Context/ResourceContext.h"

namespace Iris
{
	namespace Engine
	{
		class IWindow;
		class IRenderInterface;
		class IDevice;
		class ISwapChain;

		class ContextInitializer
		{
		public:
			IRIS_ENGINE_API IE_FORCE_INLINE RenderingContext* GetRawRenderingContext() { return &m_RenderingContext; }
			IRIS_ENGINE_API IE_FORCE_INLINE ResourceContext* GetRawResourceContext() { return &m_ResourceContext; }
			IRIS_ENGINE_API IE_FORCE_INLINE RHIType GetRHIType() { return m_RHIAPI; }

			IRIS_ENGINE_API RHIResult Initialize(IWindow* _window);
			IRIS_ENGINE_API RHIResult Terminate();

		private:
			RHIResult InitializeRenderingContext(IWindow* _window);
			RHIResult InitializeResourceContext(IWindow* _window);

		private:
			RHIType m_RHIAPI = RHIType::IE_RHI_TYPE_VULKAN;

			IRenderInterface* m_RenderInterface = nullptr;

			IDevice* m_Device = nullptr;
			ISwapChain* m_SwapChain = nullptr;

			RenderingContext m_RenderingContext{};
			ResourceContext m_ResourceContext{};
		};
	}
}