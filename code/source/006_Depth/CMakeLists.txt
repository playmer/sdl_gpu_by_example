cmake_minimum_required(VERSION 3.28.1)
project(006_Depth C)

if (PROJECT_IS_TOP_LEVEL)
    include(CMake/TopLevel.cmake)
endif()

add_executable(006_Depth)

target_compile_definitions(006_Depth PRIVATE TARGET_NAME="006_Depth")

target_sources(006_Depth
PRIVATE
    006_Depth.c
)

target_link_libraries(006_Depth PRIVATE SDL3::SDL3)

find_and_compile_shaders(${ShaderCrossExe} 006_Depth Shaders ${ShadersOutputDir})

get_target_property(targetSources 006_Depth SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})

test_example_as_cpp_if_toplevel(006_Depth)
