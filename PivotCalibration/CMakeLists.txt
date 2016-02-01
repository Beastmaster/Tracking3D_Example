cmake_minimum_required(VERSION 2.8)


project(PivotCalibration)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set( HEAD
PivotCalibration.h
)


SET( SRC
PivotCalibration.cpp
test.cpp
)

add_library(PivotCalibration ${SRC} ${HEAD})
target_link_libraries(PivotCalibration ${VTK_LIBRARIES})


