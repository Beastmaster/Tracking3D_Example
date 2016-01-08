
cmake_minimum_required(VERSION 2.8)


SET(PRJ trackering)

project(${PRJ})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})


set(
SRC
main.cxx
vtkTracking3D.cxx
QinStyle.cxx
vtkTrackingSlice.cxx
vtkTrackingRegistration.cxx
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

add_executable(${PRJ} ${SRC} ${HEAD})

target_link_libraries(${PRJ} ${VTK_LIBRARIES})
target_link_libraries(${PRJ} Polaris_Vicra_lib)
target_link_libraries(${PRJ} ATC3DG_lib)

