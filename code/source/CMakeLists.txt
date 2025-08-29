set(CMAKE_COMPILE_WARNING_AS_ERROR TRUE)

list_directories(example_directories)

foreach(example_directory ${example_directories})
    if (EXISTS ${example_directory}/CMakeLists.txt)
        message(STATUS "example: ${example_directory}")
        add_subdirectory(${example_directory})
    endif()
endforeach()
