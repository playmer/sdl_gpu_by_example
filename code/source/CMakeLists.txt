list_directories(example_directories)

foreach(example_directory ${example_directories})
    message(STATUS "example: ${example_directory}")
    add_subdirectory(${example_directory})
endforeach()
