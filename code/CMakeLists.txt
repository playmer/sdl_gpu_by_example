cmake_minimum_required(VERSION 3.28.1)
set_property(GLOBAL PROPERTY USE_FOLDERS ON) 

project(SDL_GPU_By_Example CXX C)

# Don't move this too far away from the call to project. We want this to apply to all targets
if(NOT CMAKE_VS_GLOBALS MATCHES "(^|;)UseMultiToolTask=")
  list(APPEND CMAKE_VS_GLOBALS UseMultiToolTask=true)
endif()
if(NOT CMAKE_VS_GLOBALS MATCHES "(^|;)EnforceProcessCountAcrossBuilds=")
  list(APPEND CMAKE_VS_GLOBALS EnforceProcessCountAcrossBuilds=true)
endif()

include(CMake/Helpers.cmake)

set(ShadersOutputDir ${CMAKE_CURRENT_LIST_DIR}/Assets/Shaders)
set(ShaderCrossExe ${CMAKE_CURRENT_LIST_DIR}/tools/windows/bin/shadercross.exe)

# Set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIG>")

# add_subdirectory is a function that drops us down into and executes the CMakeLists.txt within 
# the directory you specify. Once done executing it comes back up to here and continues.
add_subdirectory(external)
add_subdirectory(source)

# When using the Visual Studio generator, if you were to open the project in VS, this would
# set the startup project to our example, and if you were to press F5 it would Build and then
# Run and Debug it
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT 001_Introduction_and_the_Template)
