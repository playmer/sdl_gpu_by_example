cmake_minimum_required(VERSION 3.28.1)
project(015_GPU_Text C)

if (PROJECT_IS_TOP_LEVEL)
    include(CMake/TopLevel.cmake)
endif()

add_executable(015_GPU_Text)

target_compile_definitions(015_GPU_Text PRIVATE TARGET_NAME="015_GPU_Text")

target_sources(015_GPU_Text
PRIVATE
    015_GPU_Text.c
    stb_truetype.c
    stb_truetype.h
)

target_include_directories(015_GPU_Text PRIVATE .)
target_link_libraries(015_GPU_Text PRIVATE SDL3::SDL3)

find_and_compile_shaders(${ShaderCrossExe} 015_GPU_Text Shaders ${ShadersOutputDir})

get_target_property(targetSources 015_GPU_Text SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})

test_example_as_cpp_if_toplevel(015_GPU_Text)
