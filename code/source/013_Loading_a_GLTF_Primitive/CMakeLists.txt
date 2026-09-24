cmake_minimum_required(VERSION 3.28.1)
project(013_Loading_a_GLTF_Primitive C)

if (PROJECT_IS_TOP_LEVEL)
    include(CMake/TopLevel.cmake)
endif()

add_executable(013_Loading_a_GLTF_Primitive)
target_compile_definitions(013_Loading_a_GLTF_Primitive PRIVATE TARGET_NAME="013_Loading_a_GLTF_Primitive")
target_sources(013_Loading_a_GLTF_Primitive PRIVATE
    013_Loading_a_GLTF_Primitive.c
    cgltf.h
)
target_include_directories(013_Loading_a_GLTF_Primitive PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(013_Loading_a_GLTF_Primitive PRIVATE SDL3::SDL3)
find_and_compile_shaders(${ShaderCrossExe} 013_Loading_a_GLTF_Primitive Shaders ${ShadersOutputDir})
get_target_property(targetSources 013_Loading_a_GLTF_Primitive SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})
test_example_as_cpp_if_toplevel(013_Loading_a_GLTF_Primitive)
