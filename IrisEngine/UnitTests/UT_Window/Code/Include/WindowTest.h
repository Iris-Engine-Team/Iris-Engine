#include <gtest/gtest.h>
#include "Core/GLFWWindow.h"

class WindowTest : public ::testing::Test
{
public:
	static int Argc;
	static char** Args;

public:
	WindowTest() {}
	virtual ~WindowTest() override {}

	static void SetUpTestCase() {}
	static void TearDownTestCase() {}

	virtual void SetUp() override {}
	virtual void TearDown() override {}

	virtual void TestBody() override {}
};

TEST_F(WindowTest, WindowInitialization)
{
	Iris::Engine::IWindow* window;
	window = new Iris::Engine::GLFWWindow();

	window->Initialize();

	EXPECT_TRUE(window) << "Window test: IWindow is null !";

	EXPECT_TRUE(window->CastToGLFW()) << "Window test: GLFW Window is null !";

	EXPECT_TRUE(window->CastToGLFW()->GetRaw()) << "Window test: GLFW Window context is null !";

	window->Terminate();

	delete window;
	window = nullptr;
}

TEST_F(WindowTest, WindowTermination)
{
	Iris::Engine::IWindow* window;
	window = new Iris::Engine::GLFWWindow();

	window->Initialize();
	window->Terminate();

	EXPECT_TRUE(!window->CastToGLFW()->GetRaw()) << "Window test: GLFW Window context is not null !";

	delete window;
	window = nullptr;

	EXPECT_TRUE(!window) << "Window test: IWindow is not null !";
}

TEST_F(WindowTest, WindowRun)
{
	Iris::Engine::IWindow* window;
	window = new Iris::Engine::GLFWWindow();

	window->Initialize();

	while (!window->WindowShouldClose())
	{
		window->Update();
	}

	EXPECT_TRUE(window->WindowShouldClose()) << "Window test: IWindow closed for unknown reason !";

	window->Terminate();

	delete window;
	window = nullptr;
}