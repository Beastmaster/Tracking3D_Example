
cmake_minimum_required(VERSION 2.8)


project(vtkTracking3D)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
find_package(Qt5Widgets REQUIRED)

set(
TrackBase
TrackerBase.h
)

set(
TrackQtHeadFile
vtkTrackingResliceView.h
QtWrapvtkTracking3D.h
)

set(
TrackSRC
vtkTracking3D.cxx
vtkTrackingResliceView.cpp
vtkTrackingRegistration.cxx
ConfigurationParse.cpp
)

set(
TrackHead
vtkTracking3D.h
vtkTrackingResliceView.h
vtkTrackingRegistration.h
vtkTrackingMarkCapture.h
vtkTrackingMarkCapture.hxx
ConfigurationParse.h
)

qt5_wrap_cpp(MOCSrcs ${TrackQtHeadFile})

add_library(vtkTracking3D ${TrackSRC} ${MOCSrcs} ${TrackQtHeadFile} ${TrackHead})
#target_link_libraries(vtkTracking3D Qt5::Widgets)
target_link_libraries(vtkTracking3D ${VTK_LIBRARIES})