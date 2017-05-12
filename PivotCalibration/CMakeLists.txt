cmake_minimum_required(VERSION 2.8)


project(PivotCalibration)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

#this block is controversial
#find_package(GDCM REQUIRED)
#include(${GDCM_USE_FILE})


set( HEAD
PivotCalibration.h
PivotCalibration2.h
)


SET( SRC
PivotCalibration.cpp
PivotCalibration2.cpp
)

add_library(PivotCalibration ${SRC} ${HEAD})
target_link_libraries(PivotCalibration ${VTK_LIBRARIES})
target_link_libraries(PivotCalibration ${ITK_LIBRARIES})

