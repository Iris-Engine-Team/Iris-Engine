# Create an executable based on the main.cpp
file(GLOB_RECURSE IRIS_ENGINE_UT_SECS_SOURCES "Code/Source/*.cpp")
add_executable(IRIS_ENGINE_UT_SECS ${IRIS_ENGINE_UT_SECS_SOURCES})

target_include_directories(IRIS_ENGINE_UT_SECS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Code/Include")

target_compile_features(IRIS_ENGINE_UT_SECS PRIVATE cxx_std_20)

# Link GTest & GLM to the Library
target_link_libraries(IRIS_ENGINE_UT_SECS PRIVATE IRIS_ENGINE GTest::gtest_main)

# Register the test with the name L_MATHS_CTEST & Make it run the L_MATHS_UT executable
add_test(NAME IRIS_ENGINE_CTEST COMMAND IRIS_ENGINE_UT_SECS)