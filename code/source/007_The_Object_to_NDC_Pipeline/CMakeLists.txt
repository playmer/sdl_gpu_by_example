cmake_minimum_required(VERSION 3.28.1)
project(007_The_Object_to_NDC_Pipeline C)

if (PROJECT_IS_TOP_LEVEL)
    include(CMake/TopLevel.cmake)
endif()

add_executable(007_The_Object_to_NDC_Pipeline)

target_compile_definitions(007_The_Object_to_NDC_Pipeline PRIVATE TARGET_NAME="007_The_Object_to_NDC_Pipeline")

target_sources(007_The_Object_to_NDC_Pipeline
PRIVATE
    007_The_Object_to_NDC_Pipeline.c
)

target_link_libraries(007_The_Object_to_NDC_Pipeline PRIVATE SDL3::SDL3)

find_and_compile_shaders(${ShaderCrossExe} 007_The_Object_to_NDC_Pipeline Shaders ${ShadersOutputDir})

get_target_property(targetSources 007_The_Object_to_NDC_Pipeline SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})

test_example_as_cpp_if_toplevel(007_The_Object_to_NDC_Pipeline)
