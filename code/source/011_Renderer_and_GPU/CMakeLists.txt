cmake_minimum_required(VERSION 3.28.1)
project(011_Renderer_and_GPU C)

if (PROJECT_IS_TOP_LEVEL)
    include(CMake/TopLevel.cmake)
endif()

add_executable(011_Renderer_and_GPU)

target_compile_definitions(011_Renderer_and_GPU PRIVATE TARGET_NAME="011_Renderer_and_GPU")

target_sources(011_Renderer_and_GPU
PRIVATE
    011_Renderer_and_GPU.c
)

target_link_libraries(011_Renderer_and_GPU PRIVATE SDL3::SDL3)

find_and_compile_shaders(${ShaderCrossExe} 011_Renderer_and_GPU Shaders ${ShadersOutputDir})

get_target_property(targetSources 011_Renderer_and_GPU SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})

test_example_as_cpp_if_toplevel(011_Renderer_and_GPU)
