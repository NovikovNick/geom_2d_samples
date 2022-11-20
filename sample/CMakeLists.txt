set(CMAKE_CXX_STANDARD 20)

# SFML for x64 with BUILD_SHARED_LIBS=false, SFML_USE_STATIC_STD_LIBS=false
set(SFML_DIR "../../SFML-2.5.1/SFML_build")
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML COMPONENTS graphics window system audio REQUIRED)
add_executable (geom_2d "main.cpp")
target_link_libraries(geom_2d sfml-graphics sfml-window sfml-system sfml-network sfml-audio opengl32 winmm gdi32)