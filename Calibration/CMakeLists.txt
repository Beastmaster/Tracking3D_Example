cmake_minimum_required(VERSION 2.8)


project(Calibration)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set( HEAD
Calibration.h
)


SET( SRC
Calibration.cpp
test.cpp
)

add_library(Calibration ${SRC} ${HEAD})
target_link_libraries(Calibration ${VTK_LIBRARIES})


