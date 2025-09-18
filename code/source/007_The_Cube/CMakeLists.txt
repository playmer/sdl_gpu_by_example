cmake_minimum_required(VERSION 3.28.1)
project(007_The_Cube C)

if (PROJECT_IS_TOP_LEVEL)
    include(CMake/TopLevel.cmake)
endif()

add_executable(007_The_Cube)

target_sources(007_The_Cube
PRIVATE
    007_The_Cube.c
)

target_link_libraries(007_The_Cube PRIVATE SDL3::SDL3)

find_and_compile_shaders(${ShaderCrossExe} 007_The_Cube Shaders ${ShadersOutputDir})

get_target_property(targetSources 007_The_Cube SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})

set_property(TARGET 007_The_Cube PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")