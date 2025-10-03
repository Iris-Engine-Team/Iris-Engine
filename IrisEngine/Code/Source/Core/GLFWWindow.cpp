#include "Core/GLFWWindow.h"
#include "Resources/Texture.h"
#include "Resources/IrisResource.h"

#include <vector>
#include <array>

namespace Iris
{
	namespace Engine
	{
		void GLFWWindow::GetWindowSize(int* _width, int* _height)
		{
			glfwGetFramebufferSize(m_WindowContext, _width, _height);

			p_WindowWidth = static_cast<unsigned int>(*_width);
			p_WindowHeight = static_cast<unsigned int>(*_height);
		}

		void GLFWWindow::SetApplicationIcon(const Texture* _iconTexture, std::filesystem::path _path)
		{
			static_cast<void>(_path);

			_iconTexture;
#ifdef _WIN32
			HWND hwnd = glfwGetWin32Window(m_WindowContext);
			//HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(name));
			//HICON hIcon = (HICON)LoadImage(NULL, _path.string().c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);

			HICON hIcon = LoadIconA(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_APP_ICON));

			if (hIcon)
			{
				SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
				SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
				//DestroyIcon(hIcon);
			}
			else
			{
				DEBUG_ERROR("Failed to load engine icon!")
			}
#endif
			//GLFWimage image;
			//image.pixels = _iconTexture->GetData();
			//image.width = _iconTexture->GetWidth();
			//image.height = _iconTexture->GetHeight();
			//glfwSetWindowIcon(m_WindowContext, 1, &image);
		}

		bool GLFWWindow::Initialize()
		{
			// Initializes GLFW with no API
			glfwInit();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
			glfwSwapInterval(1);

			// Creates the window context
			m_WindowContext = glfwCreateWindow(p_WindowWidth, p_WindowHeight, p_WindowName.c_str(), nullptr, nullptr);

			glfwSetWindowUserPointer(m_WindowContext, this);

			glfwSetFramebufferSizeCallback(m_WindowContext, reinterpret_cast<GLFWframebuffersizefun>(GLFWWindow::GLFWResizeCallBack));
			glfwSetKeyCallback(m_WindowContext, reinterpret_cast<GLFWkeyfun>(GLFWWindow::GLFWInputCallBack));
			glfwSetCursorPosCallback(m_WindowContext, reinterpret_cast<GLFWcursorposfun>(GLFWWindow::GLFWMousePositionCallBack));
			glfwSetMouseButtonCallback(m_WindowContext, reinterpret_cast<GLFWmousebuttonfun>(GLFWWindow::GLFWMouseInputCallBack));
			glfwSetDropCallback(m_WindowContext, reinterpret_cast<GLFWdropfun>(GLFWWindow::GLFWDropCallBack));

			return true;
		}

		bool GLFWWindow::Terminate()
		{
			// Destroys the window context and terminate it
			glfwDestroyWindow(m_WindowContext);
			glfwTerminate();

			m_WindowContext = nullptr;

			return false;
		}

		void GLFWWindow::Update()
		{
			// Updates GLFW events
			glfwPollEvents();
		}

		bool GLFWWindow::WindowShouldClose()
		{
			return glfwWindowShouldClose(m_WindowContext);
		}

		void GLFWWindow::SetWindowShouldClose(bool _shouldClose)
		{
			glfwSetWindowShouldClose(m_WindowContext, _shouldClose);
		}

		void GLFWWindow::MaximizeWindow()
		{
			glfwMaximizeWindow(m_WindowContext);
		}

		const char** GLFWWindow::GetRequiredExtensions(unsigned int* _extensionsCount)
		{
			return glfwGetRequiredInstanceExtensions(_extensionsCount);
		}

		void GLFWWindow::WaitEvents()
		{
			return glfwWaitEvents();
		}

		void GLFWWindow::GLFWResizeCallBack(GLFWwindow* _window, int _width, int _height)
		{
			// Casts the user's pointer to our GLFW Window wrapper class
			// Not into the opaque obj of glfw
			auto window = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(_window));

			window->SetWidth(_width);
			window->SetHeight(_height);

			if (window->p_WindowResizeCallBack)
				window->p_WindowResizeCallBack(window, static_cast<unsigned int>(_width), static_cast<unsigned int>(_height));
		}

		void GLFWWindow::GLFWInputCallBack(GLFWwindow* _window, int _key, int _scanCode, int _action, int _mods)
		{
			auto window = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(_window));

			if (window->p_WindowInputCallBack)
				window->p_WindowInputCallBack(
					window,
					static_cast<unsigned int>(_key),
					static_cast<unsigned int>(_scanCode),
					static_cast<unsigned int>(_action),
					static_cast<unsigned int>(_mods));
		}

		void GLFWWindow::GLFWMousePositionCallBack(GLFWwindow* _window, double _xPos, double _yPos)
		{
			auto window = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(_window));

			if (window->p_WindowMousePositionCallBack)
				window->p_WindowMousePositionCallBack(
					window,
					_xPos,
					_yPos);
		}

		void GLFWWindow::GLFWMouseInputCallBack(GLFWwindow* _window, int _button, int _action, int _mods)
		{
			auto window = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(_window));

			if (window->p_WindowMouseInputCallBack)
				window->p_WindowMouseInputCallBack(
					window,
					static_cast<unsigned int>(_button),
					static_cast<unsigned int>(_action),
					static_cast<unsigned int>(_mods));
		}

		void GLFWWindow::GLFWDropCallBack(GLFWwindow* _window, int _count, const char** _paths)
		{
			auto window = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(_window));

			std::vector<std::string> paths(_paths, _paths + _count);

			if (window->p_WindowDropCallBack)
				window->p_WindowDropCallBack(
					window,
					static_cast<unsigned int>(_count),
					paths);
		}
	}
}