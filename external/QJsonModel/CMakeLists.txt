cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0048 NEW)
project(qjsonmodel-lib VERSION 2.1.0 LANGUAGES CXX)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)


add_library(${PROJECT_NAME} qjsonmodel.cpp
        )

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE: ${CMAKE_CURRENT_SOURCE_DIR}>
        )

target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        QJSONMODEL_COMPILE_LIBRARY
        )

target_link_libraries(${PROJECT_NAME} PUBLIC
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
        )

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)