cmake_minimum_required(VERSION 2.8)

set(PRJ visualizationUtil)
project(${PRJ})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
find_package(Qt5Widgets REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
include_directories(../vtkTrackerUtil)

set(
TrackQtHeadFile
vtkTrackingResliceView.h
QtWrapvtkTracking3D.h
)

set(
TrackSRC
vtkTracking3D.cxx
vtkTrackingResliceView.cpp
)

set(
TrackHead
vtkTracking3D.h
vtkTrackingResliceView.h
)

qt5_wrap_cpp(MOCSrcs ${TrackQtHeadFile})

add_library(${PRJ} ${TrackSRC} ${MOCSrcs} ${TrackQtHeadFile} ${TrackHead})
#target_link_libraries(${PRJ} Qt5::Widgets)
target_link_libraries(${PRJ} ${VTK_LIBRARIES})
target_link_libraries(${PRJ} vtkTrackerUtil)