PROJECT(Tools CXX C)

INCLUDE(LibraryConfig)
INCLUDE(MacroModuleConfiguration)

SET(MODULE_NAME "Tools")

PROJECT(${MODULE_NAME} CXX C)

FIND_PACKAGE(Boost 1.40 COMPONENTS filesystem system program_options REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(Eigen2 REQUIRED)

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
ENDIF(UEYEUSB_FOUND)

MESSAGE("Headers: ${${MODULE_NAME}_HEADERS}")

# include directories
INCLUDE_DIRECTORIES(
    BEFORE
	${CMAKE_CURRENT_SOURCE_DIR}
	${Boost_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
	${Eigen2_INCLUDE_DIR}
	${UEYEUSB_INCLUDE_DIR}
)

# libs
SET(${MODULE_NAME}_DEP_LIBRARIES
	${Boost_LIBRARIES}
	${OpenCV_LIBS}
	${UEYEUSB_LIBRARY}
)

MODULE_CONFIGURATION("${MODULE_NAME}" 
					 "${${MODULE_NAME}_HEADERS}" 
					 "${${MODULE_NAME}_SOURCES}" 
					 "${${MODULE_NAME}_DEP_LIBRARIES}")
