cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0048 NEW)
project(grapher-lib VERSION 0.0.0 LANGUAGES CXX)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_library(${PROJECT_NAME} SHARED

        src/file_handler.cpp
        src/data_handler.cpp
        src/workspace_handler.cpp
        src/graph_data_csv_handler.cpp

        src/controllers/main_controller.cpp
        src/controllers/menu_controller.cpp
        src/controllers/workspace_controller.cpp
        src/controllers/data_controller.cpp

        src/models/data_model.cpp
        src/models/menu_model.cpp
        src/models/workspace_model.cpp


        # public
        public/controllers/workspace_controller.h
        public/controllers/data_controller.h
        public/controllers/main_controller.h
        public/controllers/menu_controller.h

        public/models/menu_model.h
        public/models/workspace_model.h
        public/models/data_model.h
        public/models/abstract_data_provider_model.h

        public/util/json_util.h

        public/data_channel.h
        public/workspace_handler.h
        public/file_handler.h
        public/data_handler.h
        public/globals.h
        public/graph_data_csv_handler.h

        src/data_channel.cpp
        src/models/abstract_data_provider_model.cpp
        public/models/data_providers_model.h
        src/models/data_providers_model.cpp
        public/providers/test_provider.h
        src/providers/test_provider.cpp
        )

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<INSTALL_INTERFACE:
        public
        public/util
        public/providers
        public/controllers
        public/models>

        $<BUILD_INTERFACE:
        ${CMAKE_CURRENT_SOURCE_DIR}/public
        ${CMAKE_CURRENT_SOURCE_DIR}/public/providers
        ${CMAKE_CURRENT_SOURCE_DIR}/public/util
        ${CMAKE_CURRENT_SOURCE_DIR}/public/controllers
        ${CMAKE_CURRENT_SOURCE_DIR}/public/models
        >

        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/providers
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
        qcustomplot-lib
        )

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        AUTOMOC ON
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )

configure_file(defaults.json ${CMAKE_BINARY_DIR}/bin/defaults.json)

add_subdirectory(tests)