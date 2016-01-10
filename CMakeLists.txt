
cmake_minimum_required(VERSION 2.8)


SET(PRJ trackering)

project(${PRJ})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(Qt5Widgets REQUIRED)

set( UI_FILES
mainWindow.ui
)
set( QT_WRAP
mainWindow.h
)

set(
SRC
main.cxx
vtkTracking3D.cxx
QinStyle.cxx
vtkTrackingSlice.cxx
vtkTrackingRegistration.cxx
mainWindow.cpp
)

set(
HEAD
vtkTracking3D.h
QinStyle.h
vtkTrackingSlice.h
vtkTrackingRegistration.h
vtkTrackingMarkCapture.h
vtkTrackingMarkCapture.hxx
)

add_subdirectory(API)
include_directories(API)

add_subdirectory(atclib)
include_directories(atclib)

qt5_wrap_ui(UISrcs ${UI_FILES})
add_executable(${PRJ} ${SRC} ${HEAD} ${UISrcs} ${QT_WRAP})
qt5_use_modules(${PRJ} Core Gui)

target_link_libraries(${PRJ} ${VTK_LIBRARIES})
target_link_libraries(${PRJ} Polaris_Vicra_lib)
target_link_libraries(${PRJ} ATC3DG_lib)

