cmake_minimum_required(VERSION 3.28.1)
project(008_Depth C)

if (PROJECT_IS_TOP_LEVEL)
    include(CMake/TopLevel.cmake)
endif()

add_executable(008_Depth)

target_compile_definitions(008_Depth PRIVATE TARGET_NAME="008_Depth")

target_sources(008_Depth
PRIVATE
    008_Depth.c
)

target_link_libraries(008_Depth PRIVATE SDL3::SDL3)

find_and_compile_shaders(${ShaderCrossExe} 008_Depth Shaders ${ShadersOutputDir})

get_target_property(targetSources 008_Depth SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})

set_property(TARGET 008_Depth PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

test_example_as_cpp_if_toplevel(008_Depth)
