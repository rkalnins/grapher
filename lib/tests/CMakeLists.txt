cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0048 NEW)
project(grapher-lib-test VERSION 0.0.0 LANGUAGES C CXX)

find_package(Qt6 COMPONENTS Test REQUIRED)

add_executable(${PROJECT_NAME}
        src/main.cpp)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        src
        )

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Qt6::Test
        grapher-lib
        )

target_compile_features(${PROJECT_NAME}
        PRIVATE
        cxx_std_17
        )

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        AUTOMOC ON
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )