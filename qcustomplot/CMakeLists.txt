cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0048 NEW)
project(qcustomplot-lib VERSION 2.1.0 LANGUAGES CXX)

find_package(Qt6 COMPONENTS Core PrintSupport Widgets REQUIRED)

add_library(${PROJECT_NAME} SHARED
        src/qcustomplot.cpp

        public/qcustomplot.h
        )

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<INSTALL_INTERFACE: public>

        $<BUILD_INTERFACE: ${CMAKE_CURRENT_SOURCE_DIR}/public>

        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
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

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        AUTOMOC ON
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )

