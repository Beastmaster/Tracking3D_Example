
cmake_minimum_required(VERSION 2.8)


SET(PRJ trackering)

project(${PRJ})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(Qt5Widgets REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(ATC3DG)
include_directories(ATC3DG)

add_subdirectory(PolarisVicra)
include_directories(PolarisVicra)

add_subdirectory(vtkTrackerUtil)
include_directories(vtkTrackerUtil)

add_subdirectory(VisualizationUtil)
include_directories(VisualizationUtil)


add_subdirectory(PivotCalibration)
include_directories(PivotCalibration)

add_subdirectory(PolarisVicra_test)
add_subdirectory(vtkTrackerUtil_test)

set( UI_FILES
mainWindow.ui
CalibrationWindow.ui
)

set( QT_HEAD
mainWindow.h
CalibrationWindow.h
calibrateRW.h
)


set(
SRC
main.cxx
mainWindow.cpp
CalibrationWindow.cpp
)

set(
HEAD
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
target_link_libraries(${PRJ} visualizationUtil)
