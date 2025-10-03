#pragma once

#include <string>

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"

#include "Vector/Vector2.h"

#include <functional>

namespace Iris
{
	namespace Engine
	{
		class GLFWWindow;
		class Texture;

		class IWindow
		{
		public:
			IRIS_ENGINE_API	virtual ~IWindow() = default;

			///////////////////////////////////////////////////////////////////////
			///																	///
			/// 		Getters 												///
			///																	///
			///////////////////////////////////////////////////////////////////////

			/// <summary>
			/// Gets the width of the window
			/// </summary>
			/// <returns>The width of the window</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE unsigned int GetWidth() const { return p_WindowWidth; }

			/// <summary>
			/// Gets the height of the window
			/// </summary>
			/// <returns>The height of the window</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE unsigned int GetHeight() const { return p_WindowHeight; }

			/// <summary>
			/// Gets the name of the window
			/// </summary>
			/// <returns>The name of the window</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE std::string GetName() const { return p_WindowName; }

			/// <summary>
			/// Casts the current window to a GLFW window
			/// </summary>
			/// <returns>A pointer on the GLFWWindow class or nullptr if the cast failed</returns>
			IE_FORCE_INLINE virtual GLFWWindow* CastToGLFW() { return nullptr; }

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			virtual void GetWindowSize(int* _width, int* _height) = 0;

			virtual Vector2 GetCursorPosition() = 0;

			///////////////////////////////////////////////////////////////////////
			///																	///
			/// 		Setters 												///
			///																	///
			///////////////////////////////////////////////////////////////////////

			/// <summary>
			/// Sets the width of the window
			/// </summary>
			/// <param name="_width">: new width of the window</param>
			IRIS_ENGINE_API IE_FORCE_INLINE void SetWidth(unsigned int _width) { p_WindowWidth = _width; }

			/// <summary>
			/// Sets the height of the window
			/// </summary>
			/// <param name="_height">: new height of the window</param>
			IRIS_ENGINE_API IE_FORCE_INLINE void SetHeight(unsigned int _height) { p_WindowHeight = _height; }

			/// <summary>
			/// Sets the name of the window
			/// </summary>
			/// <param name="_windowName">: new name of the window</param>
			IRIS_ENGINE_API IE_FORCE_INLINE void SetName(const std::string& _windowName) { p_WindowName = _windowName; }

			virtual void SetCursorPosition(Vector2 _newPosition) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_CallBackFunction"></param>
			/// <returns></returns>
			IRIS_ENGINE_API IE_FORCE_INLINE void SetResizeCallBackFunction(std::function<void(IWindow*, unsigned int, unsigned int)>&& _CallBackFunction) { p_WindowResizeCallBack = _CallBackFunction; }
			
			IRIS_ENGINE_API IE_FORCE_INLINE void SetInputCallBackFunction(std::function<void(IWindow*, unsigned int, unsigned int, unsigned int, unsigned int)>&& _CallBackFunction) { p_WindowInputCallBack = _CallBackFunction; }
			
			IRIS_ENGINE_API IE_FORCE_INLINE void SetMousePositionCallBackFunction(std::function<void(IWindow*, double, double)>&& _CallBackFunction) { p_WindowMousePositionCallBack = _CallBackFunction; }
			
			IRIS_ENGINE_API IE_FORCE_INLINE void SetMouseInputCallBackFunction(std::function<void(IWindow*, unsigned int, unsigned int, unsigned int)>&& _CallBackFunction) { p_WindowMouseInputCallBack = _CallBackFunction; }

			IRIS_ENGINE_API IE_FORCE_INLINE void SetDropCallBackFunction(std::function<void(IWindow*, unsigned int, const std::vector<std::string>&)>&& _CallBackFunction) { p_WindowDropCallBack = _CallBackFunction; }

			virtual void SetApplicationIcon(const Texture* _iconTexture, std::filesystem::path _path) = 0;

			virtual void SetShowCursor(bool _showCursor) = 0;

			///////////////////////////////////////////////////////////////////////
			///																	///
			/// 		Methods 												///
			///																	///
			///////////////////////////////////////////////////////////////////////

			/// <summary>
			/// Initializes a window
			/// </summary>
			/// <returns></returns>
			virtual bool Initialize() = 0;

			/// <summary>
			/// Terminates a window
			/// </summary>
			/// <returns></returns>
			virtual bool Terminate() = 0;

			/// <summary>
			/// Updates the window
			/// </summary>
			virtual void Update() = 0;

			/// <summary>
			/// Checks if the window should be closed
			/// </summary>
			/// <returns>A bool true the window should close, false if not</returns>
			virtual bool WindowShouldClose() = 0;

			virtual void SetWindowShouldClose(bool _shouldClose) = 0;

			/// <summary>
			/// Maximize the window
			/// </summary>
			virtual void MaximizeWindow() = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			virtual const char** GetRequiredExtensions(unsigned int* _extensionsCount) = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			virtual void WaitEvents() = 0;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			virtual double GetTime() = 0;

		protected:
			unsigned int p_WindowWidth = 1280;
			unsigned int p_WindowHeight = 720;
			std::string p_WindowName = "Iris Engine";

			std::function<void(IWindow*, unsigned int, unsigned int)> p_WindowResizeCallBack = nullptr;
			std::function<void(IWindow*, unsigned int, unsigned int, unsigned int, unsigned int)> p_WindowInputCallBack = nullptr;
			std::function<void(IWindow*, double, double)> p_WindowMousePositionCallBack = nullptr;
			std::function<void(IWindow*, unsigned int, unsigned int, unsigned int)> p_WindowMouseInputCallBack = nullptr;
			std::function<void(IWindow*, unsigned int, const std::vector<std::string>&)> p_WindowDropCallBack = nullptr;
		};
	}
}