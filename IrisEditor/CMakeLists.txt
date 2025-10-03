cmake_minimum_required (VERSION 3.29.5)

find_package(Vulkan 1.4.304 REQUIRED)

add_subdirectory(Externals)

file(GLOB_RECURSE IRIS_EDITOR_SOURCES "Code/Source/*.cpp")

add_executable(IRIS_EDITOR ${IRIS_EDITOR_SOURCES})

set_target_properties(IRIS_EDITOR PROPERTIES OUTPUT_NAME "IrisEditor")

target_include_directories(IRIS_EDITOR PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Code/Include")

target_link_libraries(IRIS_EDITOR PUBLIC IRIS_ENGINE imgui imguizmo)

target_compile_features(IRIS_EDITOR PRIVATE cxx_std_20)

set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Assets")

add_custom_target(CopyEditorAssets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_SOURCE_DIR} ${ASSETS_DEST_DIR}
    COMMENT "Copying assets from ${ASSETS_SOURCE_DIR} to ${ASSETS_DEST_DIR}"
)

add_dependencies(IRIS_EDITOR CopyEditorAssets)

if (WIN32)
    set_target_properties(IRIS_EDITOR PROPERTIES
        WIN32_EXECUTABLE $<$<CONFIG:Release>:ON>
    )
endif()

# Set Warning level 4 and warnings as errors flags
if (MSVC)
    target_compile_options(IRIS_EDITOR PRIVATE /W4 /WX)
else ()
    target_compile_options(IRIS_EDITOR PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif ()