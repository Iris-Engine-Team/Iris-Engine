#include "ApplicationTest.h"

int main(int argc, char* argv[])
{
	ApplicationTest test;
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
	return 0;
}