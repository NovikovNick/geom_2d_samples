# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("geom_2d")

# Include sub-projects.
add_subdirectory("external")

add_subdirectory ("sample")
add_subdirectory ("menu")
add_subdirectory ("grid")
add_subdirectory ("math")
