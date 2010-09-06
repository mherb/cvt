PROJECT(cvt CXX C)

INCLUDE(LibraryConfig)
INCLUDE(MacroModuleConfiguration)
INCLUDE(CVTTESTS)

SET(MODULE_NAME "cvt")

PROJECT(${MODULE_NAME} CXX C)

FIND_PACKAGE(Boost 1.40 COMPONENTS filesystem system program_options signals thread REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(Eigen2 REQUIRED)
FIND_PACKAGE(LibPng REQUIRED)
FIND_PACKAGE(OpenCL REQUIRED)

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)
SET(QT_USE_QTOPENGL TRUE)
INCLUDE(${QT_USE_FILE})

# libs
SET(${MODULE_NAME}_DEP_LIBRARIES
	${Boost_LIBRARIES}
	${OpenCV_LIBS}
	${LIBPNG_LIBRARY}
	${QT_LIBRARIES}
	${OPENCL_LIBRARIES}
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
	util/SIMDSSE.h
	gfx/ifilter/ROFDenoise.h
	io/ImageIO.h
	gfx/IScaleFilter.h
	vision/Flow.h
	gui/GLView.h
	gui/GLObject.h
	util/CPU.h
	cl/CLBuffer.h
	cl/CLColor.h
	cl/CLException.h
	cl/CLFilter.h
	cl/CLImage.h
	cl/CLManager.h
	cl/CLParameter.h
	cl/CLParameterInfo.h
	cl/CLParameterSet.h
	cl/CLScalar.h
	cl/CLVector.h
)

SET(${MODULE_NAME}_SOURCES
	math/LevenbergMarquard.cpp
	math/Math.cpp
	util/Range.cpp
	util/Rect.cpp
	util/SIMD.cpp
	util/SIMDSSE.cpp
	util/SIMDTest.cpp
	gfx/Image.cpp
	gfx/ImageOperations.cpp
	gfx/ifilter/ROFDenoise.cpp
	gfx/IScaleFilter.cpp
	io/FileSystem.cpp
	io/FloFile.cpp
	io/ImageIO.cpp
	vision/Flow.cpp
	gui/Display.cpp
	gui/GLView.cpp
	gui/GLImage.cpp
	cl/CLColor.cpp
	cl/CLException.cpp
	cl/CLFilter.cpp
   	cl/CLImage.cpp
   	cl/CLManager.cpp
   	cl/CLParameterSet.cpp
   	cl/CLScalar.cpp
)

SET(${MODULE_NAME}_MOC_FILES
	gui/Display.h
	gui/GLView.h
)
QT4_WRAP_CPP(${MODULE_NAME}_SOURCES ${${MODULE_NAME}_MOC_FILES})
	
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
	${OPENCL_INCLUDE_DIR}
)

#set opencl filter path


MODULE_CONFIGURATION("${MODULE_NAME}" 
					 "${${MODULE_NAME}_HEADERS}" 
					 "${${MODULE_NAME}_SOURCES}" 
					 "${${MODULE_NAME}_DEP_LIBRARIES}")

set_target_properties(${MODULE_NAME}
	      PROPERTIES COMPILE_DEFINITIONS
	      PLUGIN_PATH="${CMAKE_BINARY_DIR}/bin/plugins/" )

CVTTESTS(cvt util/cvttestsproto.h)
link_directories (${CMAKE_BINARY_DIR}/lib/) 
add_executable(cvttest util/CVTTest.cpp util/cvttestsproto.h)


IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	SET_TARGET_PROPERTIES(cvttest PROPERTIES PREFIX "../")	
ENDIF()
target_link_libraries (cvttest ${MODULE_NAME}) 

