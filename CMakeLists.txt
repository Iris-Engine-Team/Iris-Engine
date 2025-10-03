cmake_minimum_required (VERSION 3.29.5)

project (IrisProject VERSION 0.0.1 DESCRIPTION "Iris Project")

set(IRIS_COMPILE_GAME false)

if(NOT IRIS_COMPILE_GAME)

set(MY_BIN_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/Build/${CMAKE_CXX_COMPILER_ID}_$<CONFIG>")

message("[${CMAKE_PROJECT_NAME}]: Output directory: ${MY_BIN_OUTPUT_DIR}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${MY_BIN_OUTPUT_DIR}/Libs")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${MY_BIN_OUTPUT_DIR}/Libs")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${MY_BIN_OUTPUT_DIR}/Bin")
set(ASSETS_DEST_DIR "${MY_BIN_OUTPUT_DIR}/Assets/Content")

set(CS_CONFIG_PATH "${CMAKE_SOURCE_DIR}/CSharpBridge/CSharp.runtimeconfig.json")
set(CS_HOST_PATH "${CMAKE_SOURCE_DIR}/CSharpBridge/nethost.dll")

add_subdirectory("${CMAKE_SOURCE_DIR}/IrisEngine")

add_subdirectory("${CMAKE_SOURCE_DIR}/IrisEditor")
else()

set(MY_BIN_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/GameBuild/${CMAKE_CXX_COMPILER_ID}_$<CONFIG>")

message("[${CMAKE_PROJECT_NAME}]: Output directory: ${MY_BIN_OUTPUT_DIR}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${MY_BIN_OUTPUT_DIR}/Libs")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${MY_BIN_OUTPUT_DIR}/Libs")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${MY_BIN_OUTPUT_DIR}/Bin")
set(ASSETS_DEST_DIR "${MY_BIN_OUTPUT_DIR}/Assets/Content")

add_subdirectory("${CMAKE_SOURCE_DIR}/IrisEngine")

add_subdirectory("${CMAKE_SOURCE_DIR}/IrisGame")
endif()