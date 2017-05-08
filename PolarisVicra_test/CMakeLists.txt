cmake_minimum_required(VERSION 2.8)

set(PRJ Polaris_Vicra_lib_test)
project(${PRJ})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} Polaris_Vicra)

set(
SRC
main.cpp
api_module.cpp
)

set(
HEAD
api_module.h
)


add_executable(${PRJ} ${SRC} ${HEAD} )
target_link_libraries(${PRJ} Polaris_Vicra_lib)




