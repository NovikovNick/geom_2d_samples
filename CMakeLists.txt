cmake_minimum_required (VERSION 3.21)
project ("geometry-samples"
    VERSION 0.1.0
    DESCRIPTION "Bunch of geometry samples"
    LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

# Include sub-projects.
add_subdirectory("external")

add_subdirectory ("sample")
add_subdirectory ("vector-product")
#add_subdirectory ("menu")
#add_subdirectory ("grid")
