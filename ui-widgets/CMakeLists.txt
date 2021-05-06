cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0048 NEW)
project(grapher-widgets VERSION 0.0.0 LANGUAGES CXX)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/main_window.cpp
        src/plot_handler.cpp

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
        qcustomplot-lib
        grapher-lib
        )

target_compile_features(${PROJECT_NAME}
        PUBLIC
        cxx_std_17
        )

target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        QCUSTOMPLOT_USE_LIBRARY
        )

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        AUTOUIC ON
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )