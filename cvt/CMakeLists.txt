PROJECT(cvt CXX C)

INCLUDE(LibraryConfig)
INCLUDE(MacroModuleConfiguration)

INCLUDE(GLCLTOHEADER)

INCLUDE(CVTTESTS)

SET(MODULE_NAME "cvt")

PROJECT(${MODULE_NAME} CXX C)

FIND_PACKAGE(Boost 1.40 REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(Eigen2 REQUIRED)
FIND_PACKAGE(LibPng REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(DC1394 REQUIRED)
FIND_PACKAGE(ffmpeg REQUIRED)

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
#	${Boost_LIBRARIES}
	${OpenCV_LIBS}
	${LIBPNG_LIBRARY}
	${OPENGL_LIBRARIES}
	${OPENCL_LIBRARIES}
	${LIBDC1394_LIBRARY}
	${X11_X11_LIB}
	${FFMPEG_LIBRARIES}
)

GLSLTOHEADER(gl/shader/120/basic_120.vert)
GLSLTOHEADER(gl/shader/120/basic_120.frag)
GLSLTOHEADER(gl/shader/120/text_120.vert)
GLSLTOHEADER(gl/shader/120/text_120.frag)
GLSLTOHEADER(gl/shader/120/icon_120.vert)
GLSLTOHEADER(gl/shader/120/icon_120.frag)
GLSLTOHEADER(gl/shader/120/basictex_120.vert)
GLSLTOHEADER(gl/shader/120/basictex_120.frag)
GLSLTOHEADER(gl/shader/120/basicrrtex_120.frag)
GLSLTOHEADER(gl/shader/120/basicrr_120.frag)
GLSLTOHEADER(gl/shader/120/basiclight_120.frag)
GLSLTOHEADER(gl/shader/120/basiclight_120.vert)

GLSLTOHEADER(gl/shader/150/basic_150.vert)
GLSLTOHEADER(gl/shader/150/basic_150.frag)
GLSLTOHEADER(gl/shader/150/text_150.vert)
GLSLTOHEADER(gl/shader/150/text_150.frag)
GLSLTOHEADER(gl/shader/150/icon_150.vert)
GLSLTOHEADER(gl/shader/150/icon_150.frag)
GLSLTOHEADER(gl/shader/150/basictex_150.vert)
GLSLTOHEADER(gl/shader/150/basictex_150.frag)
GLSLTOHEADER(gl/shader/150/basicrrtex_150.frag)
GLSLTOHEADER(gl/shader/150/basicrr_150.frag)
GLSLTOHEADER(gl/shader/150/basiclight_150.frag)
GLSLTOHEADER(gl/shader/150/basiclight_150.vert)



SET(${MODULE_NAME}_HEADERS
	io/CameraInfo.h
	io/CameraMode.h
	io/CameraModeSet.h
	io/Camera.h
	io/DC1394Camera.h
	io/FileSystem.h
	io/FloFile.h
	io/ImageIO.h
	io/IOHandler.h
	io/IOSelect.h
	io/Resources.h
	io/VideoInput.h
	io/VideoReader.h
	io/PlyModel.h
	math/ESM.cpp
	math/LevenbergMarquard.h
	math/Math.h
	math/Vector.h
	math/Vector2.h
	math/Vector3.h
	math/Vector4.h
	math/Matrix2.h
	math/Matrix3.h
	math/Matrix4.h
	math/Quaternion.h
	math/Optimization.h
	math/SBAData.h
	math/SL3Transform.h
	math/SparseBundleAdjustment.h	
	util/CPU.h
	util/CVTTest.h
	util/Delegate.h
	util/Exception.h
	util/ParamInfo.h
	util/ParamSet.h
	util/Range.h
	util/RNG.h
	util/SIMD.h
	util/SIMDSSE.h	
	util/Thread.h
	gfx/Image.h
	gfx/IFormat.h
	gfx/IConvert.h
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
	vision/AGAST.h
	vision/DLT.h
	vision/FAST.h
	vision/FeatureExtractor.h
	vision/Ferns.h
	vision/internal/Fern.h
	vision/Flow.h
	vision/PatchGenerator.h
	cl/CLContext.h
	cl/CLException.h
	cl/CLKernel.h
	geom/KDTree.h
	geom/Rect.h
	geom/Model.h
	geom/ArcBall.h
	gui/Application.h
	gui/Window.h
	gui/Widget.h
	gui/BasicTimer.h
	gui/WidgetContainer.h
	gui/Moveable.h
	gui/WidgetLayout.h
	gui/Button.h
	gui/ToggleButton.h
	gui/ImageView.h
	gui/View3D.h
	gui/internal/WidgetImplDefault.h
	gui/internal/TimerInfo.h
	gui/internal/TimerInfoList.h
	gl/OpenGL.h
	gl/GLException.h
	gl/GLProgram.h
	gl/GLBuffer.h
	gl/GLTexture.h
	gl/GLVertexArray.h
	gl/GLModel.cpp
	gl/progs/GLFillRectProg.h
	gl/progs/GLFillRoundRectProg.h
	gl/progs/GLDrawTextProg.h
	gl/progs/GLDrawImageProg.h
	gl/progs/GLDrawIconProg.h
	gl/progs/GLDrawModelProg.h
	gl/shader/120/basic_120_vert.h
	gl/shader/120/basic_120_frag.h
	gl/shader/120/basicrr_120_frag.h
	gl/shader/120/text_120_vert.h
	gl/shader/120/text_120_frag.h
	gl/shader/120/icon_120_vert.h
	gl/shader/120/icon_120_frag.h
	gl/shader/120/basictex_120_vert.h
	gl/shader/120/basictex_120_frag.h
	gl/shader/120/basicrrtex_120_frag.h
	gl/shader/120/basiclight_120_frag.h
	gl/shader/120/basiclight_120_vert.h
	gl/shader/150/basic_150_vert.h
	gl/shader/150/basic_150_frag.h
	gl/shader/150/basicrr_150_frag.h
	gl/shader/150/text_150_vert.h
	gl/shader/150/text_150_frag.h
	gl/shader/150/icon_150_vert.h
	gl/shader/150/icon_150_frag.h
	gl/shader/150/basictex_150_vert.h
	gl/shader/150/basictex_150_frag.h
	gl/shader/150/basicrrtex_150_frag.h
	gl/shader/150/basiclight_150_frag.h
	gl/shader/150/basiclight_150_vert.h
)

SET(${MODULE_NAME}_SOURCES
	io/CameraInfo.cpp
	io/CameraModeSet.cpp
	io/Camera.cpp
	io/DC1394Camera.cpp
	io/FileSystem.cpp
	io/FloFile.cpp
	io/ImageIO.cpp
	io/IOSelect.cpp
	io/Resources.cpp
	io/VideoReader.cpp
	io/PlyModel.cpp
	math/ESM.cpp
	math/LevenbergMarquard.cpp
	math/Math.cpp
	math/Vector.cpp
	math/Matrix.cpp
	math/SL3Transform.cpp
	math/SparseBundleAdjustment.cpp
	util/ParamInfo.cpp
	util/ParamSet.cpp
	util/Range.cpp
	util/SIMD.cpp
	util/SIMDSSE.cpp
	util/SIMDTest.cpp
	util/Time.cpp
	gfx/Image.cpp
	gfx/ImageTest.cpp
	gfx/IFormat.cpp
	gfx/IConvert.cpp
	gfx/ImageOperations.cpp
	gfx/ifilter/ROFDenoise.cpp
	gfx/ifilter/Homography.cpp
#	gfx/ifilter/Sobel.cpp
	gfx/IScaleFilter.cpp
	gfx/ImageAllocatorMem.cpp
	gfx/ImageAllocatorCL.cpp
	gfx/ImageAllocatorGL.cpp
	vision/AGAST.cpp
	vision/agast/agast5_8.cpp
	vision/agast/agast7_12s.cpp
	vision/agast/agast7_12d.cpp
	vision/agast/oast9_16.cpp
	vision/DLT.cpp
	vision/FAST.cpp
	vision/fast/fast9.cpp
	vision/fast/fast10.cpp
	vision/fast/fast11.cpp
	vision/fast/fast12.cpp
	vision/Ferns.cpp
	vision/Flow.cpp
	vision/PatchGenerator.cpp
	cl/CLContext.cpp
	cl/CLException.cpp
	cl/CLKernel.cpp
	geom/KDTree.cpp
	geom/Model.cpp
	geom/Rect.cpp
	gui/Application.cpp
	gui/Window.cpp
	gui/Widget.cpp
	gui/GFX.cpp
	gui/GFXGL.cpp
	gui/BasicTimer.cpp
	gui/WidgetContainer.cpp
	gui/Moveable.cpp
	gui/Button.cpp
	gui/ToggleButton.cpp
	gui/ImageView.cpp
	gui/internal/TimerInfo.cpp
	gui/internal/TimerInfoList.cpp
	gui/internal/WidgetImplWinGLX11.cpp
	gui/internal/ApplicationX11.cpp
	gui/internal/X11Handler.cpp
	gl/GLProgram.cpp
	gl/GLBuffer.cpp
	gl/GLTexture.cpp
	gl/GLVertexArray.cpp
	gl/GLModel.cpp
	gl/OpenGL.cpp
	gl/progs/GLFillRectProg.cpp
	gl/progs/GLFillRoundRectProg.cpp
	gl/progs/GLDrawTextProg.cpp
	gl/progs/GLDrawImageProg.cpp
	gl/progs/GLDrawIconProg.cpp
	gl/progs/GLDrawModelProg.cpp
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
		io/UEyeUsbCamera.h
	)
	SET(${MODULE_NAME}_SOURCES
		${${MODULE_NAME}_SOURCES}
		io/UEyeUsbCamera.cpp
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
	${FFMPEG_INCLUDES}
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

IF( ${CMAKE_GENERATOR} MATCHES "Xcode" )
	SET_TARGET_PROPERTIES(cvttest PROPERTIES PREFIX "../")	
ENDIF()
target_link_libraries (cvttest ${MODULE_NAME}) 

