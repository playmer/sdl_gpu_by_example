add_executable(003_Triangle_and_Fullscreen_Triangle)

target_sources(003_Triangle_and_Fullscreen_Triangle
PRIVATE
    003_Triangle_and_Fullscreen_Triangle.c
)

target_link_libraries(003_Triangle_and_Fullscreen_Triangle PRIVATE SDL3::SDL3)

find_and_compile_shaders(${ShaderCrossExe} 003_Triangle_and_Fullscreen_Triangle Shaders ${ShadersOutputDir})

get_target_property(targetSources 003_Triangle_and_Fullscreen_Triangle SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})

set_property(TARGET 003_Triangle_and_Fullscreen_Triangle PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")