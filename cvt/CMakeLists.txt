PROJECT(cvt CXX C)

INCLUDE(LibraryConfig)
INCLUDE(MacroModuleConfiguration)
INCLUDE(CVTTESTS)

SET(MODULE_NAME "cvt")

PROJECT(${MODULE_NAME} CXX C)

FIND_PACKAGE(Boost 1.40 COMPONENTS filesystem system program_options REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(Eigen2 REQUIRED)
FIND_PACKAGE(LibPng REQUIRED)

# libs
SET(${MODULE_NAME}_DEP_LIBRARIES
	${Boost_LIBRARIES}
	${OpenCV_LIBS}
	${LIBPNG_LIBRARY}
)

SET(${MODULE_NAME}_HEADERS
	io/AbstractCamera.hpp
	io/FileSystem.h
	io/FloFile.h
	math/LevenbergMarquard.h
	math/Math.h
	gfx/Color.h
	gfx/Image.h
	util/Range.h
	util/Rect.h
	util/CVTTest.h
	util/Exception.h
	util/Timer.h
	util/SIMD.h
	gfx/ifilter/ROFDenoise.h
	io/ImageIO.h
	gfx/IScaleFilter.h
	vision/Flow.h
)

SET(${MODULE_NAME}_SOURCES
	math/LevenbergMarquard.cpp
	util/Range.cpp
	util/Rect.cpp
	gfx/Image.cpp
	gfx/ImageOperations.cpp
	math/Math.cpp
	io/FileSystem.cpp
	io/FloFile.cpp
	util/SIMD.cpp
	gfx/ifilter/ROFDenoise.cpp
	io/ImageIO.cpp
	gfx/IScaleFilter.cpp
	vision/Flow.cpp
)
	
IF(UNIX AND NOT APPLE)
	SET(${MODULE_NAME}_HEADERS ${${MODULE_NAME}_HEADERS}
		io/V4L2Camera.h
	)
	SET(${MODULE_NAME}_SOURCES ${${MODULE_NAME}_SOURCES}
		io/V4L2Camera.cpp
	)
ENDIF()

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

# include directories
INCLUDE_DIRECTORIES(
    BEFORE
	${CMAKE_CURRENT_SOURCE_DIR}
	${Boost_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
	${Eigen2_INCLUDE_DIR}	
	${LIBPNG_INCLUDE_DIR}
)

MODULE_CONFIGURATION("${MODULE_NAME}" 
					 "${${MODULE_NAME}_HEADERS}" 
					 "${${MODULE_NAME}_SOURCES}" 
					 "${${MODULE_NAME}_DEP_LIBRARIES}")

CVTTESTS(cvt util/cvttestsproto.h)
link_directories (${CMAKE_BINARY_DIR}/lib/) 
add_executable(cvttest util/CVTTest.cpp util/cvttestsproto.h)
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	SET_TARGET_PROPERTIES(cvttest PROPERTIES PREFIX "../")	
ENDIF()
target_link_libraries (cvttest ${MODULE_NAME}) 

