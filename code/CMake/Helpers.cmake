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
