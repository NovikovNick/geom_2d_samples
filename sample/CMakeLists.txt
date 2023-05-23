find_package(SFML 2.5 COMPONENTS graphics window main REQUIRED)

find_package(imgui CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)

add_executable (sample "main.cpp")
target_compile_features(sample PUBLIC cxx_std_20)
target_link_libraries(sample
    PRIVATE
        sfml-graphics
        sfml-window
        sfml-main
        imgui::imgui
        ImGui-SFML::ImGui-SFML)
set_target_properties(sample 
    PROPERTIES
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
        OUTPUT_NAME "${PROJECT_NAME}-simple-${CMAKE_PROJECT_VERSION}")