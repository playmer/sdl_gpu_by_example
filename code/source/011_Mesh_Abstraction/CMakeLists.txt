cmake_minimum_required(VERSION 3.28.1)
project(011_Mesh_Abstraction C)

if (PROJECT_IS_TOP_LEVEL)
    include(CMake/TopLevel.cmake)
endif()

add_executable(011_Mesh_Abstraction)
target_compile_definitions(011_Mesh_Abstraction PRIVATE TARGET_NAME="011_Mesh_Abstraction")
target_sources(011_Mesh_Abstraction PRIVATE
    011_Mesh_Abstraction.c
)
target_link_libraries(011_Mesh_Abstraction PRIVATE SDL3::SDL3)
find_and_compile_shaders(${ShaderCrossExe} 011_Mesh_Abstraction Shaders ${ShadersOutputDir})
get_target_property(targetSources 011_Mesh_Abstraction SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})
test_example_as_cpp_if_toplevel(011_Mesh_Abstraction)
