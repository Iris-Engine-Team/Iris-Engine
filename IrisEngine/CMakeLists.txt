cmake_minimum_required (VERSION 3.24)

file(GLOB_RECURSE IRIS_ENGINE_SOURCES 
"Code/Source/*.cpp"
)


#find_package(DotNetFrameworkSdk)
find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)

add_subdirectory(Externals)

# For .dll
add_library(IRIS_ENGINE SHARED ${IRIS_ENGINE_SOURCES})

set_target_properties(IRIS_ENGINE PROPERTIES OUTPUT_NAME "IrisEngine")

target_include_directories(IRIS_ENGINE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Code/Include" ${Vulkan_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/Externals/" "${CMAKE_CURRENT_SOURCE_DIR}/Assets/")

target_link_libraries(IRIS_ENGINE PRIVATE ${Vulkan_LIBRARIES} Vulkan::shaderc_combined spirv-reflect-static assimp ${CMAKE_DOTNET_SDK})

target_link_libraries(IRIS_ENGINE PUBLIC IE_MATH Vulkan::Vulkan glfw Jolt)

target_compile_definitions(IRIS_ENGINE PRIVATE IRIS_ENGINE_EXPORTS)

target_compile_features(IRIS_ENGINE PRIVATE cxx_std_20)

set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Assets")

add_custom_target(CopyEngineAssets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_SOURCE_DIR} ${ASSETS_DEST_DIR}
    COMMENT "Copying assets from ${ASSETS_SOURCE_DIR} to ${ASSETS_DEST_DIR}"
)

add_dependencies(IRIS_ENGINE CopyEngineAssets)

# Set Warning level 4 and warnings as errors flags
if (MSVC)
    target_compile_options(IRIS_ENGINE PRIVATE /W4 /WX)
else ()
    target_compile_options(IRIS_ENGINE PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif ()


if(IRIS_ENGINE_UT)
    add_subdirectory(UnitTests)
endif()