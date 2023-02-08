set(CMAKE_CXX_STANDARD 20)

file(GLOB SRC "./**/*.h" "./**/*.cc")


# SFML for x64 with BUILD_SHARED_LIBS=false, SFML_USE_STATIC_STD_LIBS=false
set(SFML_DIR "../../SFML-2.5.1/SFML_build")
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "../../SFML-2.5.1/extlibs/libs-msvc-universal/x64")
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML COMPONENTS graphics window system audio REQUIRED)
add_executable (grid "main.cpp" ${SRC})
target_link_libraries(grid sfml-graphics sfml-window sfml-system sfml-network sfml-audio opengl32 winmm gdi32)
