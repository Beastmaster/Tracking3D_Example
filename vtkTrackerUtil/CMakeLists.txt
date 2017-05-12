
cmake_minimum_required(VERSION 2.8)

set(PRJ vtkTrackerUtil)
project(${PRJ})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set(
TrackBase
TrackerBase.h
)

set(
TrackSRC
vtkTrackingRegistration.cxx
ConfigurationParse.cpp
vtkTrackingMarkCapture.cxx
vtkTrackerUtil.cpp
)

set(
TrackHead
vtkTrackingMarkCapture.h
vtkTrackingRegistration.h
ConfigurationParse.h
vtkTrackeruTIL.h
util.h
)

add_library(${PRJ} ${TrackSRC} ${TrackHead})
target_link_libraries(${PRJ} ${VTK_LIBRARIES})

