cmake_minimum_required(VERSION 2.8)


project(Polaris_Vicra_lib)



set( HEAD
	PolarisVicraConfiguration.h
	APIStructures.h
	Comm32.h
	CommandHandling.h
	Conversions.h
	INIFileRW.h
	resource.h
	../vtkTrackerUtil/TrackerBase.h
)


SET( SRC
	PolarisVicraConfiguration.cpp
	Comm32.cpp
	CommandConstruction.cpp
	CommandHandling.cpp
	CommandParsing.cpp
	CommUtilities.cpp
	Conversions.cpp
	INIFileRW.cpp
	SystemCRC.cpp
)


add_library(Polaris_Vicra_lib ${SRC} ${HEAD})






