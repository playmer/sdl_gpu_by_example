cmake_minimum_required(VERSION 3.28.1)
project(008_The_Cube_and_Math C)

if (PROJECT_IS_TOP_LEVEL)
    include(CMake/TopLevel.cmake)
endif()

add_executable(008_The_Cube_and_Math)

target_compile_definitions(008_The_Cube_and_Math PRIVATE TARGET_NAME="008_The_Cube_and_Math")

target_sources(008_The_Cube_and_Math
PRIVATE
    008_The_Cube_and_Math.c
)

target_link_libraries(008_The_Cube_and_Math PRIVATE SDL3::SDL3)

find_and_compile_shaders(${ShaderCrossExe} 008_The_Cube_and_Math Shaders ${ShadersOutputDir})

get_target_property(targetSources 008_The_Cube_and_Math SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})

test_example_as_cpp_if_toplevel(008_The_Cube_and_Math)
