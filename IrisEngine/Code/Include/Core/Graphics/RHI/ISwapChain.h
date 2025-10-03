#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"
#include "Core/Graphics/RHI/CommonTypes.h"

#define MAX_FRAMES_IN_FLIGHT 2

namespace Iris
{
	namespace Engine
	{
		class VulkanSwapChain;
		class IDevice;
		class IWindow;
		class IRenderPass;
		class ICommandBuffer;
		class IFramebuffer;

		class ISwapChain
		{
		public:
			/// <summary>
			/// Default ISwapChain's destructor
			/// </summary>
			virtual ~ISwapChain() = default;

			/// <summary>
			/// Casts an ISwapChain into a Vulkan swapChain
			/// </summary>
			/// <returns>The Vulkan swapChain or nullptr</returns>
			virtual VulkanSwapChain* CastToVulkan() { return nullptr; }

			/// <summary>
			/// Gets the current frame in flight
			/// </summary>
			/// <returns>The index of the current frame in flight</returns>
			IE_FORCE_INLINE unsigned int GetCurrentFrame() const { return p_CurrentFrame; }

			/// <summary>
			/// Gets the current swapChain image
			/// </summary>
			/// <returns>The index of the current swapChain image</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE unsigned int GetCurrentSwapChainImage() const { return p_CurrentSwapChainImage; }

			/// <summary>
			/// Gets the information if the frame has been resized
			/// </summary>
			/// <returns>A bool, true: if the frame has been resized, false if not</returns>
			IE_FORCE_INLINE bool GetFrameResized() const { return p_FrameResized; }

			/// <summary>
			/// Gets the current frame width
			/// </summary>
			/// <returns>An int corresponding to the size of the frame width</returns>
			virtual unsigned int GetFrameWidth() const = 0;

			/// <summary>
			/// Gets the current frame height
			/// </summary>
			/// <returns>An int corresponding to the size of the frame height</returns>
			virtual unsigned int GetFrameHeight() const = 0;

			/// <summary>
			/// Gets a specified framebuffer of the swapChain
			/// </summary>
			/// <param name="_currentFrame">: Index of the request frame</param>
			/// <returns>The framebuffer corresponding to the requested index</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE IFramebuffer* GetFramebuffer(unsigned int _frameIndex) { return _frameIndex < p_SwapChainFramebuffers.size() ? p_SwapChainFramebuffers[_frameIndex] : nullptr; }

			/// <summary>
			/// Gets the current framebuffer of the swapChain
			/// </summary>
			/// <returns>The framebuffer corresponding to the current frame</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE IFramebuffer* GetCurrentFramebuffer() { return p_SwapChainFramebuffers[p_CurrentFrame]; }

			IRIS_ENGINE_API IE_FORCE_INLINE std::vector<IFramebuffer*>& GetFramebuffers() { return p_SwapChainFramebuffers; }

			/// <summary>
			/// Sets the state new resized state of the frame
			/// </summary>
			/// <param name="_frameResized">: New state of the frame</param>
			IE_FORCE_INLINE void SetFrameResized(bool _frameResized) { p_FrameResized = _frameResized; }

			/// <summary>
			/// Initializes the SwapChain
			/// </summary>
			/// <param name="_window">: Window of the application</param>
			/// <param name="_device">: Device of the application</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			virtual RHIResult Initialize(IWindow* _window, IDevice* _device) = 0;

			/// <summary>
			/// Terminates the SwapChain
			/// </summary>
			/// <param name="_Device">: Device of the application</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			virtual RHIResult Terminate(IDevice* _device) = 0;

			/// <summary>
			/// Recreates the SwapChain
			/// </summary>
			/// <param name="_window">: Window of the application</param>
			/// <param name="_device">: Device of the application</param>
			/// <param name="_renderPass">: RenderPass of the final result</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			virtual RHIResult RecreateSwapChain(IWindow* _window, IDevice* _device, IRenderPass* _renderPass) = 0;

			/// <summary>
			/// Makes the firsts operations of the frame
			/// </summary>
			/// <param name="_window">: Window of the application</param>
			/// <param name="_device">: Device of the application</param>
			/// <param name="_renderPass">: RenderPass of the final result</param>
			/// <returns>The index of the new swap chain image</returns>
			virtual unsigned int StartRenderingFrame(IWindow* _window, IDevice* _device, IRenderPass* _renderPass) = 0;

			/// <summary>
			/// Makes the lasts operations of the frame
			/// </summary>
			/// <param name="_window">: Window of the application</param>
			/// <param name="_device">: Device of the application</param>
			/// <param name="_commandBuffer">: Command buffer for operations</param>
			/// <param name="_renderPass">: RenderPass of the final result</param>
			/// <param name="_imageIndex">: Index of the current swap chain image</param>
			virtual void EndRenderingFrame(IWindow* _window, IDevice* _device, ICommandBuffer* _commandBuffer, IRenderPass* _renderPass, unsigned int _imageIndex) = 0;

		protected:
			std::vector<IFramebuffer*> p_SwapChainFramebuffers;
			unsigned int p_CurrentFrame = 0;
			unsigned int p_CurrentSwapChainImage = 0;
			bool p_FrameResized = false;
		};
	}
}