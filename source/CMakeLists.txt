add_executable(sdl_by_example)

target_sources(sdl_by_example
PRIVATE
    main.c
)

find_and_compile_shaders(${ShaderCrossExe} sdl_by_example Shaders ${ShadersOutputDir})

target_link_libraries(sdl_by_example PRIVATE SDL3::SDL3)

get_target_property(targetSources sdl_by_example SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})