#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Graphics/ContextInitializer.h"

#include "Resources/Mesh.h"
#include "Resources/Texture.h"
#include "Resources/Material.h"
#include "Matrix/Matrix4.h"

#include "Core/Time.h"
#include <unordered_map>

namespace Iris
{
	namespace Engine
	{
		class SceneManager;
		class MetaDataManager;
		class ThreadManager;

		class IWindow;
		class IRenderer;
		class InputManager;

		template<class T>
		class ComponentHandle;
		class CameraComponent;
		class IBuffer;
		class IDescriptor;

		class CameraComponent;
		class PlayerInput;

		class Application
		{
		public:
			IRIS_ENGINE_API Application();
			IRIS_ENGINE_API virtual ~Application();

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API virtual bool Initialize();
			
			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API virtual bool Terminate();
			
			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API virtual void Run();
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_window"></param>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			/// <returns></returns>
			IRIS_ENGINE_API virtual void ResizeWindow(IWindow* _window, unsigned int _width, unsigned int _height);
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_window"></param>
			/// <param name="_key"></param>
			/// <param name="_scanCode"></param>w
			/// <param name="_action"></param>
			/// <param name="_mods"></param>
			/// <returns></returns>
			IRIS_ENGINE_API virtual void InputCallback(IWindow* _window, unsigned int _key, unsigned int _scanCode, unsigned int _action, unsigned int _mods);
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_window"></paramc>
			/// <param name="_xPos"></param>
			/// <param name="_yPos"></param>
			/// <returns></returns>
			IRIS_ENGINE_API virtual void MousePositionCallback(IWindow* _window, double _xPos, double _yPos);
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_window"></param>
			/// <param name="_button"></param>
			/// <param name="_action"></param>
			/// <param name="_mods"></param>
			/// <returns></returns>
			IRIS_ENGINE_API virtual void MouseInputCallback(IWindow* _window, unsigned int _button, unsigned int _action, unsigned int _mods);

			IRIS_ENGINE_API IE_FORCE_INLINE Time* GetTimePtr() { return &p_Time; }
		protected:

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API virtual void Update(double _deltaTime);

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API virtual void Render();

		protected:

			IWindow* p_Window = nullptr;
			ContextInitializer p_ContextInitializer {};
			IRenderer* p_Renderer = nullptr;
			InputManager* p_InputManager = nullptr;

			bool p_CloseWindow = false;

			bool p_PlayMode = true;

			Time p_Time;

			SceneManager* p_SceneManager = nullptr;
			
			bool p_Editor = false;

			PlayerInput* p_PlayerInput = nullptr;

			ThreadManager* p_ThreadManager = nullptr;
		private:
			MetaDataManager* m_MetadataManager = nullptr;
		};
	}
}