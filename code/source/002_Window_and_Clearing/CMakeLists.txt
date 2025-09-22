cmake_minimum_required(VERSION 3.28.1)
project(002_Window_and_Clearing C)

if (PROJECT_IS_TOP_LEVEL)
    include(CMake/TopLevel.cmake)
endif()

add_executable(002_Window_and_Clearing)

target_sources(002_Window_and_Clearing
PRIVATE
    002_Window_and_Clearing.c
)

target_link_libraries(002_Window_and_Clearing PRIVATE SDL3::SDL3)

find_and_compile_shaders(${ShaderCrossExe} 002_Window_and_Clearing Shaders ${ShadersOutputDir})

get_target_property(targetSources 002_Window_and_Clearing SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})

set_property(TARGET 002_Window_and_Clearing PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

test_example_as_cpp_if_toplevel(002_Window_and_Clearing)
