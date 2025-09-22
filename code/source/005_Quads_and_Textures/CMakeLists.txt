cmake_minimum_required(VERSION 3.28.1)
project(005_Quads_and_Textures C)

if (PROJECT_IS_TOP_LEVEL)
    include(CMake/TopLevel.cmake)
endif()

add_executable(005_Quads_and_Textures)

target_sources(005_Quads_and_Textures
PRIVATE
    005_Quads_and_Textures.c
)

target_link_libraries(005_Quads_and_Textures PRIVATE SDL3::SDL3)

find_and_compile_shaders(${ShaderCrossExe} 005_Quads_and_Textures Shaders ${ShadersOutputDir})

get_target_property(targetSources 005_Quads_and_Textures SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})

set_property(TARGET 005_Quads_and_Textures PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

test_example_as_cpp_if_toplevel(005_Quads_and_Textures)
