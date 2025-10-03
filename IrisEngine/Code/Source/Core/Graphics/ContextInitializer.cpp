#include "Core/Graphics/ContextInitializer.h"

#include "Core/IWindow.h"
#include "Core/Graphics/VulkanRHI/VulkanRenderInterface.h"

namespace Iris
{
	namespace Engine
	{
		RHIResult ContextInitializer::Initialize(IWindow* _window)
		{
			switch (m_RHIAPI)
			{
			case RHIType::IE_RHI_TYPE_VULKAN: default:
				m_RenderInterface = new VulkanRenderInterface;
				break;
			case RHIType::IE_RHI_TYPE_OPENGL:
				break;
			}

			m_Device = m_RenderInterface->InstantiateDevice(_window);
			m_SwapChain = m_RenderInterface->InstantiateSwapChain(_window, m_Device);

			InitializeRenderingContext(_window);
			InitializeResourceContext(_window);

			return RHIResult();
		}

		RHIResult ContextInitializer::InitializeRenderingContext(IWindow* _window)
		{
			m_RenderingContext.Initialize(_window, m_Device, m_SwapChain, m_RenderInterface);
			return RHIResult();
		}

		RHIResult ContextInitializer::InitializeResourceContext(IWindow* _window)
		{
			m_ResourceContext.Initialize(_window, m_Device, m_SwapChain, m_RenderInterface);
			return RHIResult();
		}

		RHIResult ContextInitializer::Terminate()
		{
			m_RenderingContext.Terminate(m_RenderInterface);
			m_ResourceContext.Terminate(m_RenderInterface);

			m_RenderInterface->DestroyDevice(m_Device);

			delete m_RenderInterface;
			m_RenderInterface = nullptr;

			return RHIResult();
		}
	}
}