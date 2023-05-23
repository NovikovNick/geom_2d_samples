find_package(Eigen3 3.4 REQUIRED)
find_package(SFML 2.5 COMPONENTS graphics window main REQUIRED)

add_subdirectory (resources)

file(GLOB VECTOR_PRODUCT_SRC "./*.h" "./*.cc")

add_executable (vector-product "main.cc" ${VECTOR_PRODUCT_SRC})
target_compile_features(vector-product PUBLIC cxx_std_20)
target_link_libraries(vector-product
    PRIVATE
        sfml-graphics
        sfml-window
        sfml-main)
set_target_properties(vector-product 
    PROPERTIES
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
        OUTPUT_NAME "${PROJECT_NAME}-vector-product-${CMAKE_PROJECT_VERSION}")
