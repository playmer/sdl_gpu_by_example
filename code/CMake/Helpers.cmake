# Please don't ask how to understand this yet...I'll get around to documenting it line by line.
macro(find_and_compile_shaders aShaderCrossExecutable aTarget aShaderDirectory aOutputDirectory)
    file(GLOB ShadersSources 
        CONFIGURE_DEPENDS
        "${aShaderDirectory}/*.comp.hlsl"
        "${aShaderDirectory}/*.vert.hlsl"
        "${aShaderDirectory}/*.frag.hlsl"
    )

    foreach(shader ${ShadersSources})
        get_filename_component(shaderFilename ${shader} NAME_WE)
        get_filename_component(shaderExtension ${shader} EXT)

        if("${shaderExtension}" STREQUAL ".comp.hlsl")
            set(shaderExtension .comp)
            set(shaderStage compute)
        elseif("${shaderExtension}" STREQUAL ".vert.hlsl")
            set(shaderExtension .vert)
            set(shaderStage vertex)
        elseif("${shaderExtension}" STREQUAL ".frag.hlsl")
            set(shaderExtension .frag)
            set(shaderStage fragment)
        endif()
        
        set(spirvShader ${aOutputDirectory}/${shaderFilename}${shaderExtension}.spv)
        set(mslShader ${aOutputDirectory}/${shaderFilename}${shaderExtension}.msl)
        set(dxilShader ${aOutputDirectory}/${shaderFilename}${shaderExtension}.dxil)
            
        add_custom_command(OUTPUT ${spirvShader} ${mslShader} ${dxilShader}
            COMMAND ${aShaderCrossExecutable} ARGS ${shader} --source HLSL --dest SPIRV --stage ${shaderStage} --output ${spirvShader}
            COMMAND ${aShaderCrossExecutable} ARGS ${shader} --source HLSL --dest MSL --stage ${shaderStage} --output ${mslShader}
            COMMAND ${aShaderCrossExecutable} ARGS ${shader} --source HLSL --dest DXIL --stage ${shaderStage} --output ${dxilShader}
            DEPENDS ${shader})
        list(APPEND CompiledShaderFiles ${spirvShader} ${mslShader} ${dxilShader})
    endforeach(shader)

    add_custom_target(${aTarget}_Shaders DEPENDS ${CompiledShaderFiles})
    
    add_dependencies(${aTarget} ${aTarget}_Shaders)

    target_sources(${aTarget} 
    PRIVATE
        ${ShadersSources}
    )

    foreach(shader ${ShadersSources})
        set_source_files_properties(${shader} PROPERTIES VS_TOOL_OVERRIDE "None")
        set_source_files_properties(SOURCE ${shader} PROPERTIES VS_SETTINGS "ExcludedFromBuild=true")
    endforeach(shader)
endmacro()

macro(list_directories result)
    message(STATUS "List Dir: ${CMAKE_CURRENT_LIST_DIR}")
    #file(GLOB children LIST_DIRECTORIES true RELATIVE CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR})
    file(GLOB children RELATIVE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/*)
    
    message(STATUS "\tchildren: ${children}")
    set(directory_list "")
    
    foreach(child_item ${children})
        message(STATUS "\tChild Item: ${child_item}")
        if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${child_item})
            list(APPEND directory_list ${child_item})
        endif()
    endforeach()
    
    set(${result} ${directory_list})
endmacro()

macro(get_shadercross)
    set(ShaderCrossExe ${PROJECT_SOURCE_DIR}/tools/bin/shadercross.exe)

    set(HAVE_SHADER_CROSS_PRECOMPILED_BINARIES FALSE)
    if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL Darwin)
        set(SHADER_CROSS_HOST_PLATFORM "darwin")

        set(HAVE_SHADER_CROSS_PRECOMPILED_BINARIES TRUE)
        set(SHADER_CROSS_HOST_ARCHITECTURE "arm64-x64")
        set(SHADER_CROSS_ARCHIVE_EXT "tar.gz")
    elseif (${CMAKE_HOST_SYSTEM_NAME} STREQUAL Windows)
        set(SHADER_CROSS_HOST_PLATFORM "VC")
        
        if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
            set(HAVE_SHADER_CROSS_PRECOMPILED_BINARIES TRUE)
            set(SHADER_CROSS_HOST_ARCHITECTURE "x64")
            set(SHADER_CROSS_ARCHIVE_EXT "zip")
        endif()
    elseif (${CMAKE_HOST_SYSTEM_NAME} STREQUAL Linux)
        set(SHADER_CROSS_HOST_PLATFORM "linux")

        if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86_64")
            set(HAVE_SHADER_CROSS_PRECOMPILED_BINARIES TRUE)
            set(SHADER_CROSS_HOST_ARCHITECTURE "x64")
            set(SHADER_CROSS_ARCHIVE_EXT "tar.gz")
        endif()
    endif()


    if (NOT EXISTS ${ShaderCrossExe})
        set(SHADER_CROSS_NAME "SDL3_shadercross-${SHADER_CROSS_HOST_PLATFORM}-${SHADER_CROSS_HOST_ARCHITECTURE}")
        set(SHADER_CROSS_ZIP "${SHADER_CROSS_NAME}.${SHADER_CROSS_ARCHIVE_EXT}")

        # Figure out where it'll be after downloading, and check to see if we previously downloaded it.
        set(ShaderCrossExe ${PROJECT_SOURCE_DIR}/tools/${SHADER_CROSS_NAME}/bin/shadercross)
        
        if (NOT EXISTS ${ShaderCrossExe})

            find_program(ShaderCrossExe_Search shadercross)
            
            if(NOT ShaderCrossExe_Search)
                if (NOT ${HAVE_SHADER_CROSS_PRECOMPILED_BINARIES})
                    message(FATAL_ERROR 
                        "We couldn't find SDL_shadercross in your path, so we're trying to acquire a precompiled binary "
                        "for your system. However there are none for ${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR}. "
                        "Please visit https://github.com/libsdl-org/SDL_shadercross and build/download a copy and add it to your "
                        "path."
                    )
                endif()

                if (${INSIDE_FULL_REPO})
                    set(PATH_TO_ZIP "${PROJECT_SOURCE_DIR}/../site/static_data/assets/${SHADER_CROSS_ZIP}")
                else()
                    set(PATH_TO_ZIP "${CMAKE_CURRENT_BINARY_DIR}/${SHADER_CROSS_ZIP}")
                    file(DOWNLOAD "https://www.nullterminatedstrings.com/sdl_gpu_by_example/assets/${SHADER_CROSS_ZIP}" "${PATH_TO_ZIP}")
                endif()
                
                file(ARCHIVE_EXTRACT INPUT "${PATH_TO_ZIP}" DESTINATION "${PROJECT_SOURCE_DIR}/tools/")
                message(STATUS "exe: ${ShaderCrossExe}")
            else()
                set(ShaderCrossExe "${ShaderCrossExe_Search}")
            endif()
        endif()
    endif()

    unset(HAVE_SHADER_CROSS_PRECOMPILED_BINARIES)
    unset(PATH_TO_ZIP)
    unset(SHADER_CROSS_ARCHIVE_EXT)
    unset(SHADER_CROSS_HOST_ARCHITECTURE)
    unset(SHADER_CROSS_HOST_PLATFORM)
    unset(SHADER_CROSS_NAME)
    unset(SHADER_CROSS_ZIP)
    unset(ShaderCrossExe_Search)
endmacro()

macro(set_up_example_top_level)
    if (PROJECT_IS_TOP_LEVEL)
        if (${PROJECT_NAME} STREQUAL SDL_GPU_By_Example)
            set(INSIDE_FULL_REPO TRUE)
        else()
            set(INSIDE_FULL_REPO FALSE)
        endif()

        # Don't move this too far away from the call to project. We want this to apply to all targets
        if(NOT CMAKE_VS_GLOBALS MATCHES "(^|;)UseMultiToolTask=")
            list(APPEND CMAKE_VS_GLOBALS UseMultiToolTask=true)
        endif()
        if(NOT CMAKE_VS_GLOBALS MATCHES "(^|;)EnforceProcessCountAcrossBuilds=")
            list(APPEND CMAKE_VS_GLOBALS EnforceProcessCountAcrossBuilds=true)
        endif()

        set(ShadersOutputDir ${PROJECT_SOURCE_DIR}/Assets/Shaders)

        get_shadercross()

        # Set the output directory for built objects.
        # This makes sure that the dynamic library goes into the build directory automatically.
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIG>")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIG>")
        
        # add_subdirectory is a function that drops us down into and executes the CMakeLists.txt within 
        # the directory you specify. Once done executing it comes back up to here and continues.
        if (${INSIDE_FULL_REPO})
            add_subdirectory(external)
            add_subdirectory(source)
        else()
            # FetchContent downloads and configures dependencies
            include(FetchContent)
            FetchContent_Declare(
                SDL3
                GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
                GIT_TAG "main"
                EXCLUDE_FROM_ALL
            )
            FetchContent_MakeAvailable(SDL3)
        endif()
    endif()
endmacro()

# When using the Visual Studio generator, if you were to open the project in VS, this would
# set the startup project to our example, and if you were to press F5 it would Build and then
# Run and Debug it
macro(set_vs_startup_project_if_toplevel aTarget)
    if (PROJECT_IS_TOP_LEVEL)
        set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${aTarget})
    endif()
endmacro()