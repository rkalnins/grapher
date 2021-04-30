cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0048 NEW)
project(qcustomplot-lib VERSION 2.1.0 LANGUAGES C CXX)

find_package(Qt6 COMPONENTS Core PrintSupport Widgets REQUIRED)

add_library(${PROJECT_NAME}
        qcustomplot.cpp
        )

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE: ${CMAKE_CURRENT_SOURCE_DIR}>

        )

target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        QCUSTOMPLOT_COMPILE_LIBRARY
        )

target_link_libraries(${PROJECT_NAME} PUBLIC
        Qt6::Core
        Qt6::PrintSupport
        Qt6::Widgets
        )

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
