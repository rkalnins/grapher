cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0048 NEW)
project(grapher VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib)
add_subdirectory(ui-widgets)
add_subdirectory(qcustomplot)