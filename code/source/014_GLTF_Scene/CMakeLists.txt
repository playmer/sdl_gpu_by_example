cmake_minimum_required(VERSION 3.28.1)
project(014_GLTF_Scene C)

if (PROJECT_IS_TOP_LEVEL)
    include(CMake/TopLevel.cmake)
endif()

add_executable(014_GLTF_Scene)
target_compile_definitions(014_GLTF_Scene PRIVATE TARGET_NAME="014_GLTF_Scene")
target_sources(014_GLTF_Scene PRIVATE
    014_GLTF_Scene.c
    cgltf.h
)
target_include_directories(014_GLTF_Scene PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(014_GLTF_Scene PRIVATE SDL3::SDL3)
find_and_compile_shaders(${ShaderCrossExe} 014_GLTF_Scene Shaders ${ShadersOutputDir})
get_target_property(targetSources 014_GLTF_Scene SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})
test_example_as_cpp_if_toplevel(014_GLTF_Scene)
