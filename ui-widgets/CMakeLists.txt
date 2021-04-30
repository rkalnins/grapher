cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0048 NEW)
project(grapher-widgets VERSION 0.0.0 LANGUAGES C CXX)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(${PROJECT_NAME}
        src/main.cpp
        )

target_include_directories(${PROJECT_NAME}
        PUBLIC
        src
        )

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        grapher-lib
        )

target_compile_features(${PROJECT_NAME}
        PUBLIC
        cxx_std_17
        )

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )