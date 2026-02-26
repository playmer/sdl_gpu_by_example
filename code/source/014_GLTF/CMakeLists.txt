cmake_minimum_required(VERSION 3.28.1)
project(014_GLTF C)

if (PROJECT_IS_TOP_LEVEL)
    include(CMake/TopLevel.cmake)
endif()

add_executable(014_GLTF)

target_compile_definitions(014_GLTF PRIVATE TARGET_NAME="014_GLTF")

target_sources(014_GLTF
PRIVATE
    014_GLTF.c
    cgltf.h
)

target_link_libraries(014_GLTF PRIVATE SDL3::SDL3)

find_and_compile_shaders(${ShaderCrossExe} 014_GLTF Shaders ${ShadersOutputDir})

get_target_property(targetSources 014_GLTF SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})
