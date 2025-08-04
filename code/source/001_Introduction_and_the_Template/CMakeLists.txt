cmake_minimum_required(VERSION 3.28.1)
project(001_Introduction_and_the_Template C)

if (PROJECT_IS_TOP_LEVEL)
    include(CMake/TopLevel.cmake)
endif()

# This is how we declare an executable "target" to be built. There are _many_ optional parameters
# but for now we'll just declare this. (There are also library targets, which is what SDL is.)
add_executable(001_Introduction_and_the_Template)

# Target sources allows us to add source files to our target. When writing an executable, you'll
# need one of these to contain the definition of `main`.
#
# Also note that functions like this can be called many times. There _are_ some functions or 
# properties that can only be called from the same directory we called `add_executabe` or
# `add_library` in. 
target_sources(001_Introduction_and_the_Template
PRIVATE
    001_Introduction_and_the_Template.c
)

# target_link_libraries lets us add dependencies to our target. These can be as simple as .lib
# or .a files, or targets unto themselves such as SDL3 here. The SDL3:: bit of the name is a
# convention used to specify this is a target, so CMake knows it's _not_ just looking for a 
# lib file somewhere. Targets don't naturally get that naming scheme though, it's done using
# ALIAS targets, which SDL3 has done for us.
#
# The fact that we link to a target is really helpful. Targets a bundles of information that 
# represent how to build and build against libraries. You see here that we're linking SDL3
# and that it's marked as PRIVATE, as opposed to PUBLIC. This is because all of these target_*
# functions are specifying properties that may be transitive (if PUBLIC), and thus percolate
# upwards when a target is depended on. This is how we can simply `#include <SDL3/SDL.h>` 
# without ever specifying an include directory here.
target_link_libraries(001_Introduction_and_the_Template PRIVATE SDL3::SDL3)

# This is a helper written for the sake of creating a target that represents our Shaders.
# it will automatically compile them for us, and place them into the asset directory 
# whenever they change. And it makes sure to build them before 001_Introduction_and_the_Template, which
# can come in handy if we ever want to use shader reflection in a later example.
find_and_compile_shaders(${ShaderCrossExe} 001_Introduction_and_the_Template Shaders ${ShadersOutputDir})

# These two lines are essentially a trick to make IDE (Visual Studio, XCode, etc) 
# projects look nicer. `get_target_property` is just getting all  paths to all of the 
# sources we've specified so far for our `001_Introduction_and_the_Template` target, and putting them into 
# a list.
#
# `source_group` takes that list of files and basically just ensures that the IDE project
# looks like the directory structure we've set up. The details here aren't that interesting 
# unless you're familiar with how Visual Studio works and how it shows you folders that 
# aren't folders.
get_target_property(targetSources 001_Introduction_and_the_Template SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${targetSources})

# Setting this property means that when using a generated Visual Studio solution, the 
# Working directory of 001_Introduction_and_the_Template when running in the debugger is set to the value of 
# CMAKE_SOURCE_DIR, which itself is the top level folder in this CMake file structure. In
# this case, one level up, the root of the repository.
#
# This is helpful as we'll always assume we're executing our code from the root of the repo,
# and path our assets accordingly.
set_property(TARGET 001_Introduction_and_the_Template PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

set_vs_startup_project_if_toplevel(001_Introduction_and_the_Template)