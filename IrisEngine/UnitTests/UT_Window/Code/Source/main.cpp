#include "WindowTest.h"

int main(int argc, char* argv[])
{
	WindowTest test;
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
	return 0;
}