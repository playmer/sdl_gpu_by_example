cmake_minimum_required(VERSION 3.28.1)
project(SDL_GPU_By_Example CXX C)
set_property(GLOBAL PROPERTY USE_FOLDERS ON) 

include(CMake/TopLevel.cmake)


message(STATUS "ShadersOutputDir = ${ShadersOutputDir}")

# When using the Visual Studio generator, if you were to open the project in VS, this would
# set the startup project to our example, and if you were to press F5 it would Build and then
# Run and Debug it
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT 001_Introduction_and_the_Template)
