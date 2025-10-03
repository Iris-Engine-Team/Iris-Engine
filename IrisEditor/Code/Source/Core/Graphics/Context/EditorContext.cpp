#include "Core/Graphics/Context/EditorContext.h"
#include "Core/Graphics/RHI/IRenderInterface.h"
#include "Core/Graphics/RHI/IDevice.h"
#include "Core/Graphics/RHI/ICommandAllocator.h"
#include "Core/Graphics/RHI/IDescriptorAllocator.h"

namespace Iris
{
	namespace Editor
	{
		Engine::RHIResult EditorContext::Initialize(Engine::IWindow* _window, Engine::IDevice* _device, Engine::ISwapChain* _swapChain, Engine::IRenderInterface* _renderInterface)
		{
			Engine::IContext::Initialize(_window, _device, _swapChain, _renderInterface);

			m_CommandAllocator = _renderInterface->InstantiateCommandAllocator(p_DeviceReference);

			m_DescriptorAllocator = _renderInterface->InstantiateDescriptorAllocator(p_DeviceReference);

			return Engine::RHIResult::IE_RHI_SUCCESS;
		}

		Engine::RHIResult EditorContext::Terminate(Engine::IRenderInterface* _renderInterface)
		{
			if (!_renderInterface)
			{
				// TODO: ERR
				return Engine::RHIResult::IE_RHI_FAIL_UNKNOWN;
			}

			p_DeviceReference->WaitIdle();

			_renderInterface->DestroyDescriptorAllocator(m_DescriptorAllocator, p_DeviceReference);

			_renderInterface->DestroyCommandAllocator(m_CommandAllocator, p_DeviceReference);

			return Engine::RHIResult::IE_RHI_SUCCESS;
		}
	}
}