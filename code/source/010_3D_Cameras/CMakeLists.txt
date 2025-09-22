cmake_minimum_required(VERSION 3.28.1)
project(010_3D_Cameras C)

if (PROJECT_IS_TOP_LEVEL)
    include(CMake/TopLevel.cmake)
endif()

add_executable(010_3D_Cameras)

target_sources(010_3D_Cameras
PRIVATE
    010_3D_Cameras.c
)

target_link_libraries(010_3D_Cameras PRIVATE SDL3::SDL3)

find_and_compile_shaders(${ShaderCrossExe} 010_3D_Cameras Shaders ${ShadersOutputDir})

get_target_property(targetSources 010_3D_Cameras SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})

set_property(TARGET 010_3D_Cameras PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

test_example_as_cpp_if_toplevel(010_3D_Cameras)
