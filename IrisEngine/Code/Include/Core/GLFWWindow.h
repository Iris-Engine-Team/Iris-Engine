#pragma once

#include "IWindow.h"

#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Iris
{
	namespace Engine
	{	
		class GLFWWindow : public IWindow
		{
		public:
			IRIS_ENGINE_API virtual ~GLFWWindow() override = default;

			///////////////////////////////////////////////////////////////////////
			///																	///
			/// 		Getters 												///
			///																	///
			///////////////////////////////////////////////////////////////////////

			/// <summary>
			/// Gets the raw type of the window
			/// </summary>
			/// <returns>The raw type of the window context</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE GLFWwindow* GetRaw() { return m_WindowContext; }

			/// <summary>
			/// Casts the current window to a GLFW window
			/// </summary>
			/// <returns>A pointer on the GLFWWindow class or nullptr if the cast failed</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE GLFWWindow* CastToGLFW() override { return this; }

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			IRIS_ENGINE_API void GetWindowSize(int* _width, int* _height) override;

			IRIS_ENGINE_API IE_FORCE_INLINE Vector2 GetCursorPosition() override { double x, y; glfwGetCursorPos(m_WindowContext, &x, &y); return Vector2(static_cast<float>(x), static_cast<float>(y)); }

			IRIS_ENGINE_API void SetApplicationIcon(const Texture* _iconTexture, std::filesystem::path _path) override;

			IRIS_ENGINE_API	IE_FORCE_INLINE void SetCursorPosition(Vector2 _newPosition) override { glfwSetCursorPos(m_WindowContext, static_cast<double>(_newPosition[0]), static_cast<double>(_newPosition[1])); }

			IRIS_ENGINE_API IE_FORCE_INLINE void SetShowCursor(bool _showCursor) override 
			{ 
				if (_showCursor) 
					glfwSetInputMode(m_WindowContext, GLFW_CURSOR, GLFW_CURSOR_NORMAL); 
				else 
					glfwSetInputMode(m_WindowContext, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}

			///////////////////////////////////////////////////////////////////////
			///																	///
			/// 		Methods 												///
			///																	///
			///////////////////////////////////////////////////////////////////////

			/// <summary>
			/// Initializes a window
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API bool Initialize() override;

			/// <summary>
			/// Terminates a window
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API bool Terminate() override;

			/// <summary>
			/// Updates the window
			/// </summary>
			IRIS_ENGINE_API void Update() override;

			/// <summary>
			/// Checks if the window should be closed
			/// </summary>
			/// <returns>A bool true the window should close, false if not</returns>
			IRIS_ENGINE_API bool WindowShouldClose() override;

			IRIS_ENGINE_API void SetWindowShouldClose(bool _shouldClose) override;

			/// <summary>
			/// Maximize the window
			/// </summary>
			IRIS_ENGINE_API void MaximizeWindow() override;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API const char** GetRequiredExtensions(unsigned int* _extensionsCount) override;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API void WaitEvents() override;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API IE_FORCE_INLINE double GetTime() override final { return glfwGetTime(); }

		private:
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_window"></param>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			static void GLFWResizeCallBack(GLFWwindow* _window, int _width, int _height);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_window"></param>
			/// <param name="_key"></param>
			/// <param name="_scanCode"></param>
			/// <param name="_action"></param>
			/// <param name="_mods"></param>
			static void GLFWInputCallBack(GLFWwindow* _window, int _key, int _scanCode, int _action, int _mods);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_window"></param>
			/// <param name="_key"></param>
			/// <param name="_scanCode"></param>
			/// <param name="_action"></param>
			/// <param name="_mods"></param>
			static void GLFWMousePositionCallBack(GLFWwindow* _window, double _xPos, double _yPos);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_window"></param>
			/// <param name="_button"></param>
			/// <param name="_action"></param>
			/// <param name="_mods"></param>
			static void GLFWMouseInputCallBack(GLFWwindow* _window, int _button, int _action, int _mods);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_window"></param>
			/// <param name="count"></param>
			/// <param name="paths"></param>
			static void GLFWDropCallBack(GLFWwindow* _window, int count, const char** paths);

		private:
			GLFWwindow* m_WindowContext = nullptr;
		};
	}
}