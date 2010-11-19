PROJECT(cvt CXX C)

INCLUDE(LibraryConfig)
INCLUDE(MacroModuleConfiguration)

INCLUDE(GLCLTOHEADER)

INCLUDE(CVTTESTS)

SET(MODULE_NAME "cvt")

PROJECT(${MODULE_NAME} CXX C)

FIND_PACKAGE(Boost 1.40 COMPONENTS filesystem system program_options signals thread REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(Eigen2 REQUIRED)
FIND_PACKAGE(LibPng REQUIRED)

FIND_PACKAGE(OpenGL REQUIRED)
IF( APPLE )
	SET( OPENGL_gl_LIBRARY "/usr/X11/lib/libGL.dylib" )
	SET( OPENGL_glu_LIBRARY "/usr/X11/lib/libGLU.dylib" )
	SET( OPENGL_INCLUDE_DIR "/usr/X11/include/GL" )
	SET( X11_X11_LIB "/usr/X11/lib/libX11.dylib")
	SET( OPENGL_LIBRARIES 
		${OPENGL_gl_LIBRARY}
		${OPENGL_glu_LIBRARY} )
	
	LINK_DIRECTORIES(/usr/X11/lib)
ENDIF( APPLE )

FIND_PACKAGE(OpenCL REQUIRED)
FIND_PACKAGE(DC1394 REQUIRED)
FIND_PACKAGE(X11 REQUIRED)

# libs
SET(${MODULE_NAME}_DEP_LIBRARIES
	${Boost_LIBRARIES}
	${OpenCV_LIBS}
	${LIBPNG_LIBRARY}
	${OPENGL_LIBRARIES}
	${OPENCL_LIBRARIES}
	${LIBDC1394_LIBRARY}
	${X11_X11_LIB}
)

GLSLTOHEADER(gl/shader/120/basic_120.vert)
GLSLTOHEADER(gl/shader/120/basic_120.frag)
GLSLTOHEADER(gl/shader/120/text_120.vert)
GLSLTOHEADER(gl/shader/120/text_120.frag)
GLSLTOHEADER(gl/shader/120/basictex_120.vert)
GLSLTOHEADER(gl/shader/120/basictex_120.frag)
GLSLTOHEADER(gl/shader/120/basicrrtex_120.frag)

GLSLTOHEADER(gl/shader/150/basic_150.vert)
GLSLTOHEADER(gl/shader/150/basic_150.frag)
GLSLTOHEADER(gl/shader/150/text_150.vert)
GLSLTOHEADER(gl/shader/150/text_150.frag)
GLSLTOHEADER(gl/shader/150/basictex_150.vert)
GLSLTOHEADER(gl/shader/150/basictex_150.frag)
GLSLTOHEADER(gl/shader/150/basicrrtex_150.frag)



SET(${MODULE_NAME}_HEADERS
	io/AbstractCamera.hpp
	io/FileSystem.h
	io/FloFile.h
	io/ImageIO.h
	io/VideoInput.h
	io/CameraInfo.h
	io/Camera.h
	io/DC1394Camera.h
	io/Resources.h
	io/IOHandler.h
	io/IOSelect.h
	math/ESM.cpp
	math/LevenbergMarquard.h
	math/Math.h
	math/Vector.h
	math/Matrix.h
	math/Optimization.h
	math/SBAData.h
	math/SL3Transform.h
	math/SparseBundleAdjustment.h
	util/Range.h
	util/Rect.h
	util/CVTTest.h
	util/Exception.h
	util/SIMD.h
	util/SIMDSSE.h
	util/CPU.h
	util/Delegate.h
	gfx/Image.h
	gfx/IFormat.h
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
	gfx/ImageAllocatorGL.h
	vision/Flow.h
	cl/CLContext.h
	cl/CLException.h
	cl/CLKernel.h
	gui/Application.h
	gui/Window.h
	gui/Widget.h
	gui/BasicTimer.h
	gui/internal/TimerInfo.h
	gui/internal/TimerInfoList.h
	gl/OpenGL.h
	gl/GLException.h
	gl/GLProgram.h
	gl/GLBuffer.h
	gl/GLVertexArray.h
	util/Thread.h
	gl/shader/120/basic_120_vert.h
	gl/shader/120/basic_120_frag.h
	gl/shader/120/text_120_vert.h
	gl/shader/120/text_120_frag.h
	gl/shader/120/basictex_120_vert.h
	gl/shader/120/basictex_120_frag.h
	gl/shader/120/basicrrtex_120_frag.h
	gl/shader/150/basic_150_vert.h
	gl/shader/150/basic_150_frag.h
	gl/shader/150/text_150_vert.h
	gl/shader/150/text_150_frag.h
	gl/shader/150/basictex_150_vert.h
	gl/shader/150/basictex_150_frag.h
	gl/shader/150/basicrrtex_150_frag.h
)

SET(${MODULE_NAME}_SOURCES
	io/FileSystem.cpp
	io/FloFile.cpp
	io/ImageIO.cpp
	io/Camera.cpp
	io/DC1394Camera.cpp
	io/Resources.cpp
	io/IOSelect.cpp
	math/ESM.cpp
	math/LevenbergMarquard.cpp
	math/Math.cpp
	math/Vector.cpp
	math/Matrix.cpp
	math/SL3Transform.cpp
	math/SparseBundleAdjustment.cpp
	util/Range.cpp
	util/Rect.cpp
	util/SIMD.cpp
	util/SIMDSSE.cpp
	util/SIMDTest.cpp
	util/Time.cpp
	gfx/Image.cpp
	gfx/IFormat.cpp
	gfx/ImageTypes.cpp
	gfx/ImageOperations.cpp
	gfx/ifilter/ROFDenoise.cpp
	gfx/ifilter/Homography.cpp
#	gfx/ifilter/Sobel.cpp
	gfx/IScaleFilter.cpp
	gfx/ImageAllocatorMem.cpp
	gfx/ImageAllocatorCL.cpp
	gfx/ImageAllocatorGL.cpp
	vision/Flow.cpp
	cl/CLContext.cpp
	cl/CLException.cpp
	cl/CLKernel.cpp
	gui/Application.cpp
	gui/Window.cpp
	gui/Widget.cpp
	gui/GFX.cpp
	gui/GFXGL.cpp
	gui/BasicTimer.cpp
	gui/internal/TimerInfo.cpp
	gui/internal/TimerInfoList.cpp
	gui/internal/WidgetImplWinGLX11.cpp
	gui/internal/ApplicationX11.cpp
	gui/internal/X11Handler.cpp
	gl/GLProgram.cpp
	gl/GLBuffer.cpp
	gl/GLVertexArray.cpp
	gl/OpenGL.cpp
)

IF(UNIX)
	IF(APPLE)
		SET(${MODULE_NAME}_HEADERS ${${MODULE_NAME}_HEADERS}
			io/QTKitCamera.h
		)
		
		SET(${MODULE_NAME}_SOURCES ${${MODULE_NAME}_SOURCES}
			io/QTKitCamera.mm
		)
		
		SET(${MODULE_NAME}_DEP_LIBRARIES
			${${MODULE_NAME}_DEP_LIBRARIES}			
			"-framework Cocoa"
			"-framework CoreFoundation"
			"-framework QuartzCore"
			"-framework QTKit"
		)
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
#	${OpenCV_INCLUDE_DIRS}
	${Eigen2_INCLUDE_DIR}	
	${LIBPNG_INCLUDE_DIR}
	${OPENCL_INCLUDE_DIR}
	${LIBDC1394_INCLUDE_DIR}
	${X11_X11_INCLUDE_PATH}
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

