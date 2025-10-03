#pragma once

#include <imgui.h>
#include <vector>

namespace Iris
{
	namespace Engine 
	{
		class Texture;
		enum class RHIType : int;
		class IFramebuffer;
	}
	namespace Editor
	{
		class EditorRenderer;

		class ImGuiWrapper
		{
		public:
			static ImTextureID CreateTextureID(Engine::Texture* _texture);
			static ImTextureID CreateTextureID(const Engine::Texture* _texture);
			static std::vector<ImTextureID> CreateFramebufferIDs(EditorRenderer* _editorRenderer, std::vector<Engine::IFramebuffer*> _framebuffers);
			static void DeleteFramebufferIDs(std::vector<ImTextureID>& _framebufferIDs);
			static void SetRHIType(Engine::RHIType _RHIType);
		private:
			static Engine::RHIType m_RHIType;
		};
	}
}