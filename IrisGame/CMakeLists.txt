cmake_minimum_required (VERSION 3.29.5)

find_package(Vulkan 1.4.304 REQUIRED)

file(GLOB_RECURSE IRIS_GAME_SOURCES "main.cpp")

add_executable(IRIS_GAME ${IRIS_GAME_SOURCES})

set_target_properties(IRIS_GAME PROPERTIES OUTPUT_NAME "IrisGame")

target_link_libraries(IRIS_GAME PUBLIC IRIS_ENGINE)

target_compile_features(IRIS_GAME PRIVATE cxx_std_20)

set(ASSETS_SOURCE_DIR1 "${CMAKE_CURRENT_SOURCE_DIR}/GameFiles")

add_custom_target(CopyGameAssets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_SOURCE_DIR1} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

add_dependencies(IRIS_GAME CopyGameAssets)

if (WIN32)
    set_target_properties(IRIS_GAME PROPERTIES
        WIN32_EXECUTABLE ON
    )
endif()

# Set Warning level 4 and warnings as errors flags
if (MSVC)
    target_compile_options(IRIS_GAME PRIVATE /W4 /WX)
else ()
    target_compile_options(IRIS_GAME PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif ()