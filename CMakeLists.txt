
cmake_minimum_required(VERSION 2.8)


SET(PRJ trackering)

project(${PRJ})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(Qt5Widgets REQUIRED)

set( UI_FILES
mainWindow.ui
)
set( QT_HEAD
mainWindow.h
ResliceView.h
QtWrapvtkTracking3D.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})


set(
SRC
main.cxx
vtkTracking3D.cxx
QinStyle.cxx
vtkTrackingSlice.cxx
vtkTrackingRegistration.cxx
mainWindow.cpp
ResliceView.cpp
)

set(
HEAD
vtkTracking3D.h
QinStyle.h
vtkTrackingSlice.h
vtkTrackingRegistration.h
vtkTrackingMarkCapture.h
vtkTrackingMarkCapture.hxx
ImageManagement.h
)

add_subdirectory(API)
include_directories(API)

add_subdirectory(atclib)
include_directories(atclib)

qt5_wrap_ui(UISrcs ${UI_FILES})
qt5_wrap_cpp(MOCSrcs ${QT_HEAD})
add_executable(${PRJ} ${SRC} ${HEAD} ${UISrcs} ${QT_HEAD} ${MOCSrcs})
qt5_use_modules(${PRJ} Core Gui)

target_link_libraries(${PRJ} ${VTK_LIBRARIES})
target_link_libraries(${PRJ} Polaris_Vicra_lib)
target_link_libraries(${PRJ} ATC3DG_lib)

