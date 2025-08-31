cmake_minimum_required(VERSION 3.28.1)
project(006_The_Object_to_NDC_Pipeline C)

if (PROJECT_IS_TOP_LEVEL)
    include(CMake/TopLevel.cmake)
endif()

add_executable(006_The_Object_to_NDC_Pipeline)

target_sources(006_The_Object_to_NDC_Pipeline
PRIVATE
    006_The_Object_to_NDC_Pipeline.c
)

target_link_libraries(006_The_Object_to_NDC_Pipeline PRIVATE SDL3::SDL3)

find_and_compile_shaders(${ShaderCrossExe} 006_The_Object_to_NDC_Pipeline Shaders ${ShadersOutputDir})

get_target_property(targetSources 006_The_Object_to_NDC_Pipeline SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})

set_property(TARGET 006_The_Object_to_NDC_Pipeline PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")