cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0048 NEW)
project(grapher-lib VERSION 0.0.0 LANGUAGES C CXX)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_library(${PROJECT_NAME} SHARED
        src/tmp.cpp
        )

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<INSTALL_INTERFACE:
        public
        public/controllers
        public/models>

        $<BUILD_INTERFACE:
        ${CMAKE_CURRENT_SOURCE_DIR}/public
        ${CMAKE_CURRENT_SOURCE_DIR}/public/controllers
        ${CMAKE_CURRENT_SOURCE_DIR}/public/models
        >

        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/controllers
        ${CMAKE_CURRENT_SOURCE_DIR}/src/models
        )

target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        GRAPHER_LIBRARY
        )

target_link_libraries(${PROJECT_NAME} PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        )

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

add_subdirectory(tests)