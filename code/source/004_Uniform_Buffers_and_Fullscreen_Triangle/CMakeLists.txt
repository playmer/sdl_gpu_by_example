cmake_minimum_required(VERSION 3.28.1)
project(004_Uniform_Buffers_and_Fullscreen_Triangle C)

if (PROJECT_IS_TOP_LEVEL)
    include(CMake/TopLevel.cmake)
endif()

add_executable(004_Uniform_Buffers_and_Fullscreen_Triangle)

target_compile_definitions(004_Uniform_Buffers_and_Fullscreen_Triangle PRIVATE TARGET_NAME="004_Uniform_Buffers_and_Fullscreen_Triangle")

target_sources(004_Uniform_Buffers_and_Fullscreen_Triangle
PRIVATE
    004_Uniform_Buffers_and_Fullscreen_Triangle.c
)

target_link_libraries(004_Uniform_Buffers_and_Fullscreen_Triangle PRIVATE SDL3::SDL3)

find_and_compile_shaders(${ShaderCrossExe} 004_Uniform_Buffers_and_Fullscreen_Triangle Shaders ${ShadersOutputDir})

get_target_property(targetSources 004_Uniform_Buffers_and_Fullscreen_Triangle SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})

set_property(TARGET 004_Uniform_Buffers_and_Fullscreen_Triangle PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

test_example_as_cpp_if_toplevel(004_Uniform_Buffers_and_Fullscreen_Triangle)
