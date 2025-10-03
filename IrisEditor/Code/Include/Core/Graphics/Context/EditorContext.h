#pragma once

#include "Core/Graphics/Context/IContext.h"

#include <vector>

namespace Iris
{
	namespace Engine
	{
		class IDescriptorAllocator;
		class ICommandAllocator;
		class IRenderPass;
		class IFramebuffer;
	}

	namespace Editor
	{
		class EditorContext : public Engine::IContext
		{
		public:
			~EditorContext() final = default;
			IE_FORCE_INLINE Engine::IRenderPass* GetRenderPass() { return m_FinalRenderPass; }
			IE_FORCE_INLINE std::vector<Engine::IFramebuffer*> GetFramebuffers() { return m_FinalFramebuffers; }
			IE_FORCE_INLINE Engine::ICommandAllocator* GetCommandAllocator() { return m_CommandAllocator; }
			IE_FORCE_INLINE Engine::IDescriptorAllocator* GetDescriptorAllocator() { return m_DescriptorAllocator; }

			Engine::RHIResult Initialize(Engine::IWindow* _window, Engine::IDevice* _device, Engine::ISwapChain* _swapChain, Engine::IRenderInterface* _renderInterface) override final;
			Engine::RHIResult Terminate(Engine::IRenderInterface* _renderInterface) final;

		private:

			Engine::IRenderPass* m_FinalRenderPass = nullptr;
			std::vector<Engine::IFramebuffer*> m_FinalFramebuffers;

			Engine::IDescriptorAllocator* m_DescriptorAllocator = nullptr;

			Engine::ICommandAllocator* m_CommandAllocator = nullptr;
		};
	}
}