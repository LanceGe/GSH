cmake_minimum_required(VERSION 2.8)
project (GSH)
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_COMPILER "gcc")
set(CMAKE_CXX_FLAGS "-std=c99")
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS "-g")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/executable)
include_directories(${PROJECT_SOURCE_DIR}/include)
aux_source_directory(${PROJECT_SOURCE_DIR}/source SRC_LIST)
add_definitions("-g -lreadline -ltermcap")
add_executable(gsh ${SRC_LIST})
target_link_libraries(gsh readline)
