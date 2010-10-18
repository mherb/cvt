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
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(OpenCL REQUIRED)
FIND_PACKAGE(DC1394 REQUIRED)

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)
SET(QT_USE_QTOPENGL TRUE)
INCLUDE(${QT_USE_FILE})

# libs
SET(${MODULE_NAME}_DEP_LIBRARIES
	${Boost_LIBRARIES}
	${OpenCV_LIBS}
	${LIBPNG_LIBRARY}
	${QT_LIBRARIES}
	${OPENGL_LIBRARIES}
	${OPENCL_LIBRARIES}
	${LIBDC1394_LIBRARY}
)

SET(${MODULE_NAME}_HEADERS
	io/AbstractCamera.hpp
	io/FileSystem.h
	io/FloFile.h
	io/ImageIO.h
	io/DC1394Camera.h
	io/Resources.h
	math/ESM.cpp
	math/LevenbergMarquard.h
	math/Math.h
	math/Optimization.h
	math/SBAData.h
	math/SL3Transform.h
	math/SparseBundleAdjustment.h
	util/Range.h
	util/Rect.h
	util/CVTTest.h
	util/Exception.h
	util/Timer.h
	util/SIMD.h
	util/SIMDSSE.h
	util/CPU.h
	gfx/Image.h
	gfx/ImageTypes.h
	gfx/ifilter/ROFDenoise.h
	gfx/ifilter/Homography.h
#	gfx/ifilter/Sobel.h
	gfx/IScaleFilter.h
	gfx/IFilterParameter.h
	gfx/IFilterParameterInfo.h
	gfx/IFilterParameterSet.h
	gfx/IFilterScalar.h
	gfx/IFilterVector.h
	gfx/ImageAllocator.h
	gfx/ImageAllocatorMem.h
	gfx/ImageAllocatorCL.h
	vision/Flow.h
	cl/CLContext.h
	cl/CLException.h
	cl/CLKernel.h
)

SET(${MODULE_NAME}_SOURCES
	io/FileSystem.cpp
	io/FloFile.cpp
	io/ImageIO.cpp
	io/DC1394Camera.cpp
	io/Resources.cpp
	math/ESM.cpp
	math/LevenbergMarquard.cpp
	math/Math.cpp
	math/SL3Transform.cpp
	math/SparseBundleAdjustment.cpp
	util/Range.cpp
	util/Rect.cpp
	util/SIMD.cpp
	util/SIMDSSE.cpp
	util/SIMDTest.cpp
	gfx/Image.cpp
	gfx/ImageTypes.cpp
	gfx/ImageOperations.cpp
	gfx/ifilter/ROFDenoise.cpp
	gfx/ifilter/Homography.cpp
#	gfx/ifilter/Sobel.cpp
	gfx/IScaleFilter.cpp
	gfx/ImageAllocatorMem.cpp
	gfx/ImageAllocatorCL.cpp
	vision/Flow.cpp
	cl/CLContext.cpp
	cl/CLException.cpp
	cl/CLKernel.cpp
)

SET(${MODULE_NAME}_MOC_FILES
)
QT4_WRAP_CPP(${MODULE_NAME}_SOURCES ${${MODULE_NAME}_MOC_FILES})
	
IF(UNIX)
	IF(APPLE)
		SET(${MODULE_NAME}_HEADERS ${${MODULE_NAME}_HEADERS}
#			io/QTKit.h
#			io/QTKitCamera.h
		)
		SET(${MODULE_NAME}_SOURCES ${${MODULE_NAME}_SOURCES}
#			io/QTKit.mm
#			io/QTKitCamera.cpp
		)
		
#		SET(${MODULE_NAME}_DEP_LIBRARIES
#			${${MODULE_NAME}_DEP_LIBRARIES}
#			"-framework CoreFoundation"
#			"-framework QTKit"
#		)		
	ELSE(APPLE)
		SET(${MODULE_NAME}_HEADERS ${${MODULE_NAME}_HEADERS}
			io/V4L2Camera.h
		)
		SET(${MODULE_NAME}_SOURCES ${${MODULE_NAME}_SOURCES}
			io/V4L2Camera.cpp
		)
	ENDIF(APPLE)
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
	${LIBDC1394_INCLUDE_DIR}
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

