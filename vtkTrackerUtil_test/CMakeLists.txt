
cmake_minimum_required(VERSION 2.8)


SET(PRJ vtkTrackerUtil_test)

project(${PRJ})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(Qt5Widgets REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(registration)
include_directories(registration)


set( UI_FILES
)

set( QT_HEAD
)


set(
SRC
main.cxx
trackingclass_test.cxx
)

set(
HEAD
trackingclass_test.h
)



qt5_wrap_ui(UISrcs ${UI_FILES})
qt5_wrap_cpp(MOCSrcs ${QT_HEAD})
add_executable(${PRJ} ${SRC} ${HEAD} ${UISrcs} ${QT_HEAD} ${MOCSrcs} )
qt5_use_modules(${PRJ} Core Gui)

target_link_libraries(${PRJ} ${VTK_LIBRARIES})
target_link_libraries(${PRJ} ${ITK_LIBRARIES})
target_link_libraries(${PRJ} Polaris_Vicra_lib)
target_link_libraries(${PRJ} ATC3DG_lib)
target_link_libraries(${PRJ} vtkTrackerUtil)
target_link_libraries(${PRJ} PivotCalibration)
