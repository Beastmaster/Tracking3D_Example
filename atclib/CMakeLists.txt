
cmake_minimum_required(VERSION 2.8)

project(ATC3DG_lib)


set(
HEAD
ATC3DG.h
ATC3DGConfiguration.h
TrackerBase.h
)

set(
SRC
ATC3DGConfiguration.cpp
)


add_library(ATC3DG_lib ${HEAD} ${SRC})

target_link_libraries(ATC3DG_lib ${CMAKE_SOURCE_DIR}/lib/ATC3DG64.lib)

