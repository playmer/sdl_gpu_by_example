cmake_minimum_required(VERSION 3.28.1)
project(012_Mesh_Data_and_Upload C)

if (PROJECT_IS_TOP_LEVEL)
    include(CMake/TopLevel.cmake)
endif()

add_executable(012_Mesh_Data_and_Upload)
target_compile_definitions(012_Mesh_Data_and_Upload PRIVATE TARGET_NAME="012_Mesh_Data_and_Upload")
target_sources(012_Mesh_Data_and_Upload PRIVATE
    012_Mesh_Data_and_Upload.c
)
target_link_libraries(012_Mesh_Data_and_Upload PRIVATE SDL3::SDL3)
find_and_compile_shaders(${ShaderCrossExe} 012_Mesh_Data_and_Upload Shaders ${ShadersOutputDir})
get_target_property(targetSources 012_Mesh_Data_and_Upload SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})
test_example_as_cpp_if_toplevel(012_Mesh_Data_and_Upload)
