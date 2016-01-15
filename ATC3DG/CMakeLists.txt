
cmake_minimum_required(VERSION 2.8)

project(ATC3DG_lib)


set(
HEAD
ATC3DG.h
ATC3DGConfiguration.h
../TrackingClass/TrackerBase.h
)

set(
SRC
ATC3DGConfiguration.cpp
)


ADD_LIBRARY(ATC3DG64 STATIC IMPORTED)
set_property(TARGET ATC3DG64 PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/ATC3DG64.lib)
add_library(ATC3DG_lib ${HEAD} ${SRC})
TARGET_LINK_LIBRARIES(ATC3DG_lib  ATC3DG64)