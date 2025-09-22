cmake_minimum_required(VERSION 3.28.1)
project(009_Vertex_and_Index_Buffers C)

if (PROJECT_IS_TOP_LEVEL)
    include(CMake/TopLevel.cmake)
endif()

add_executable(009_Vertex_and_Index_Buffers)

target_sources(009_Vertex_and_Index_Buffers
PRIVATE
    009_Vertex_and_Index_Buffers.c
)

target_link_libraries(009_Vertex_and_Index_Buffers PRIVATE SDL3::SDL3)

find_and_compile_shaders(${ShaderCrossExe} 009_Vertex_and_Index_Buffers Shaders ${ShadersOutputDir})

get_target_property(targetSources 009_Vertex_and_Index_Buffers SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})

set_property(TARGET 009_Vertex_and_Index_Buffers PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

test_example_as_cpp_if_toplevel(009_Vertex_and_Index_Buffers)
