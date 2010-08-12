PROJECT(cvt CXX C)

INCLUDE(LibraryConfig)
INCLUDE(MacroModuleConfiguration)
INCLUDE(CVTTESTS)

SET(MODULE_NAME "cvt")

PROJECT(${MODULE_NAME} CXX C)

FIND_PACKAGE(Boost 1.40 COMPONENTS filesystem system program_options REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(Eigen2 REQUIRED)

# libs
SET(${MODULE_NAME}_DEP_LIBRARIES
	${Boost_LIBRARIES}
	${OpenCV_LIBS}
)

SET(${MODULE_NAME}_HEADERS
	io/AbstractCamera.hpp
	math/LevenbergMarquard.h
)

SET(${MODULE_NAME}_SOURCES
	math/LevenbergMarquard.cpp
)

# optional uEyeUsbCamera driver if available 
FIND_PACKAGE(uEyeUsbCamera)
IF(UEYEUSB_FOUND)
	SET(${MODULE_NAME}_HEADERS
		${${MODULE_NAME}_HEADERS}
		io/muEyeUsbCamera.h
	)
	SET(${MODULE_NAME}_SOURCES
		${${MODULE_NAME}_SOURCES}
		io/muEyeUsbCamera.cpp
	)
	INCLUDE_DIRECTORIES(
		BEFORE
		${UEYEUSB_INCLUDE_DIR}		
	)
	SET(${MODULE_NAME}_DEP_LIBRARIES
		${${MODULE_NAME}_DEP_LIBRARIES}
		${UEYEUSB_LIBRARY}		
	)	
ENDIF(UEYEUSB_FOUND)

MESSAGE("Headers: ${${MODULE_NAME}_HEADERS}")

# include directories
INCLUDE_DIRECTORIES(
    BEFORE
	${CMAKE_CURRENT_SOURCE_DIR}
	${Boost_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
	${Eigen2_INCLUDE_DIR}	
)

MODULE_CONFIGURATION("${MODULE_NAME}" 
					 "${${MODULE_NAME}_HEADERS}" 
					 "${${MODULE_NAME}_SOURCES}" 
					 "${${MODULE_NAME}_DEP_LIBRARIES}")

CVTTESTS(/lib/libcvt_d.so util/cvttestsproto.h)
add_executable(cvttest util/CVTTest.cpp util/cvttestsproto.h)

