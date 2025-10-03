# Create an executable based on the main.cpp
file(GLOB_RECURSE IRIS_ENGINE_UT_WINDOW_SOURCES "Code/Source/*.cpp")
include_directories("Code/Include/")
add_executable(IRIS_ENGINE_UT_WINDOW ${IRIS_ENGINE_UT_WINDOW_SOURCES})

target_compile_features(IRIS_ENGINE_UT_WINDOW PRIVATE cxx_std_20)

# Link GTest & GLM to the Library
target_link_libraries(IRIS_ENGINE_UT_WINDOW PRIVATE IRIS_ENGINE glfw GTest::gtest_main)

# Register the test with the name L_MATHS_CTEST & Make it run the L_MATHS_UT executable
add_test(NAME IRIS_ENGINE_CTEST COMMAND IRIS_ENGINE_UT_WINDOW)