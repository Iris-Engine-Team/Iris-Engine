
file(GLOB IMGUIZMO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(imguizmo STATIC ${IMGUIZMO_SOURCES})

target_include_directories(imguizmo PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(imguizmo PRIVATE imgui)