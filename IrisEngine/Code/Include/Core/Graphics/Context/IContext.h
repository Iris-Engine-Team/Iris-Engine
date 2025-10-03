#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Graphics/RHI/CommonTypes.h"

namespace Iris
{
	namespace Engine
	{
		class IWindow;
		class IRenderInterface;
		class IDevice;
		class ISwapChain;

		class IContext
		{
		public:
			/// <summary>
			/// Default IContext's destructor
			/// </summary>
			IRIS_ENGINE_API virtual ~IContext() = default;

			/// <summary>
			/// Device getter
			/// </summary>
			/// <returns>: A pointer to the device</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE IDevice* GetDevice() { return p_DeviceReference; }

			/// <summary>
			/// Render Interface getter
			/// </summary>
			/// <returns>: A pointer to the render interface</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE IRenderInterface* GetRenderInterface() { return p_RenderInterfaceReference; }

			/// <summary>
			/// Initializes the global context variables
			/// </summary>
			/// <param name="_window">: Window of the application</param>
			/// <param name="_device">: Device of the application</param>
			/// <param name="_swapChain">: SwapChain of the application</param>
			/// <param name="_renderInterface">: Render interface of the application</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API virtual RHIResult Initialize(IWindow* _window, IDevice* _device, ISwapChain* _swapChain, IRenderInterface* _renderInterface) { _window;  p_DeviceReference = _device; p_SwapChainReference = _swapChain; p_RenderInterfaceReference = _renderInterface; return RHIResult::IE_RHI_SUCCESS; }

			/// <summary>
			/// Terminates the global context variables
			/// </summary>
			/// <param name="_renderInterface">: Render interface of the application</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API virtual RHIResult Terminate(IRenderInterface* _renderInterface) = 0;

		protected:
			IDevice* p_DeviceReference = nullptr;
			ISwapChain* p_SwapChainReference = nullptr;
			IRenderInterface* p_RenderInterfaceReference = nullptr;
		};
	}
}