#pragma once

#include <vector>
#include "Core/Tools/Macros.h"

namespace Iris
{
	namespace Engine
	{
		class IRenderPass;
		class IFramebuffer;
		class ISampler;
		class RenderingContext;
		class IRenderInterface;
	}

	namespace Editor
	{
		class EditorRenderer
		{
		public:
			IE_FORCE_INLINE Engine::IRenderPass* GetRenderPass() { return m_EditorRenderPass; }
			IE_FORCE_INLINE std::vector<Engine::IFramebuffer*> GetFramebuffers() { return m_EditorFramebuffers; }
			IE_FORCE_INLINE Engine::ISampler* GetFramebufferSampler() { return m_FramebufferSampler; }

			void Initialize(Engine::RenderingContext* _renderingContext);
			void Terminate();

		private:
			Engine::RenderingContext* m_RenderingContextReference = nullptr;

			Engine::IRenderPass* m_EditorRenderPass = nullptr;
			std::vector<Engine::IFramebuffer*> m_EditorFramebuffers{};
			
			Engine::ISampler* m_FramebufferSampler = nullptr;
		};
	}
}