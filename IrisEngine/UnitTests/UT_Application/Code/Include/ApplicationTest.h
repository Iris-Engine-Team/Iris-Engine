#include <gtest/gtest.h>
#include "Core/Application.h"

class ApplicationTest : public ::testing::Test
{
public:
	static int Argc;
	static char** Args;

public:
	ApplicationTest() {}
	virtual ~ApplicationTest() override {}

	static void SetUpTestCase() {}
	static void TearDownTestCase() {}

	virtual void SetUp() override {}
	virtual void TearDown() override {}

	virtual void TestBody() override {}
};

TEST_F(ApplicationTest, ApplicationRun)
{
	Iris::Engine::Application app;
	app.Initialize();

	app.Run();

	app.Terminate();
}