cmake_minimum_required(VERSION 3.28.1)
project(007_The_Cube_and_Math C)

if (PROJECT_IS_TOP_LEVEL)
    include(CMake/TopLevel.cmake)
endif()

add_executable(007_The_Cube_and_Math)

target_compile_definitions(007_The_Cube_and_Math PRIVATE TARGET_NAME="002_Window_and_Clearing")

target_sources(007_The_Cube_and_Math
PRIVATE
    007_The_Cube_and_Math.c
)

target_link_libraries(007_The_Cube_and_Math PRIVATE SDL3::SDL3)

find_and_compile_shaders(${ShaderCrossExe} 007_The_Cube_and_Math Shaders ${ShadersOutputDir})

get_target_property(targetSources 007_The_Cube_and_Math SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})

set_property(TARGET 007_The_Cube_and_Math PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

test_example_as_cpp_if_toplevel(007_The_Cube_and_Math)
