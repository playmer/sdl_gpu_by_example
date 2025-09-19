cmake_minimum_required(VERSION 3.28.1)
project(010_An_FPS_Camera C)

if (PROJECT_IS_TOP_LEVEL)
    include(CMake/TopLevel.cmake)
endif()

add_executable(010_An_FPS_Camera)

target_sources(010_An_FPS_Camera
PRIVATE
    010_An_FPS_Camera.c
)

target_link_libraries(010_An_FPS_Camera PRIVATE SDL3::SDL3)

find_and_compile_shaders(${ShaderCrossExe} 010_An_FPS_Camera Shaders ${ShadersOutputDir})

get_target_property(targetSources 010_An_FPS_Camera SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})

set_property(TARGET 010_An_FPS_Camera PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
