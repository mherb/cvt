PROJECT(cvt CXX C)

INCLUDE(LibraryConfig)
INCLUDE(MacroModuleConfiguration)

INCLUDE(GLCLTOHEADER)

INCLUDE(CVTTESTS)

SET(MODULE_NAME "cvt")

PROJECT(${MODULE_NAME} CXX C)

FIND_PACKAGE(Eigen2 REQUIRED)
FIND_PACKAGE(LibPng REQUIRED)
FIND_PACKAGE(DC1394 REQUIRED)
FIND_PACKAGE(ffmpeg REQUIRED)

FIND_PACKAGE(OpenGL REQUIRED)
IF( APPLE )
	SET( OPENGL_gl_LIBRARY "/usr/X11/lib/libGL.dylib" )
	SET( OPENGL_glu_LIBRARY "/usr/X11/lib/libGLU.dylib" )
	SET( OPENGL_INCLUDE_DIR "/usr/X11/include/GL" )
	SET( X11_X11_LIB "/usr/X11/lib/libX11.dylib")
	SET( OPENGL_LIBRARIES ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} )
	LINK_DIRECTORIES(/usr/X11/lib)
ENDIF( APPLE )

FIND_PACKAGE(OpenCL REQUIRED)
FIND_PACKAGE(DC1394 REQUIRED)
FIND_PACKAGE(X11 REQUIRED)

# libs
SET(${MODULE_NAME}_DEP_LIBRARIES
	${LIBPNG_LIBRARY}
	${OPENGL_LIBRARIES}
	${OPENCL_LIBRARIES}
	${LIBDC1394_LIBRARY}
	${X11_X11_LIB}
	${FFMPEG_LIBRARIES}
)

SET( GLSL_SHADER
	gl/shader/120/basic_120.vert
	gl/shader/120/basic_120.frag
	gl/shader/120/text_120.vert
	gl/shader/120/text_120.frag
	gl/shader/120/icon_120.vert
	gl/shader/120/icon_120.frag
	gl/shader/120/basictex_120.vert
	gl/shader/120/basictex_120.frag
	gl/shader/120/basicrrtex_120.frag
	gl/shader/120/basicrr_120.frag
	gl/shader/120/basiclight_120.frag
	gl/shader/120/basiclight_120.vert
	gl/shader/150/basic_150.vert
	gl/shader/150/basic_150.frag
	gl/shader/150/text_150.vert
	gl/shader/150/text_150.frag
	gl/shader/150/icon_150.vert
	gl/shader/150/icon_150.frag
	gl/shader/150/basictex_150.vert
	gl/shader/150/basictex_150.frag
	gl/shader/150/basicrrtex_150.frag
	gl/shader/150/basicrr_150.frag
	gl/shader/150/basiclight_150.frag
	gl/shader/150/basiclight_150.vert
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

SET( CL_KERNELS
	cl/kernel/gaussiir.cl
	cl/kernel/gaussiir2.cl
	cl/kernel/BC.cl
)

CLTOHEADER( cl/kernel/gaussiir.cl )
CLTOHEADER( cl/kernel/gaussiir2.cl )
CLTOHEADER( cl/kernel/BC.cl )

SET(${MODULE_NAME}_HEADERS
	cl/CLContext.h
	cl/CLException.h
	cl/CLKernel.h
	cl/kernel/gaussiir.h
	cl/kernel/gaussiir2.h
	cl/kernel/BC.h
	geom/ArcBall.h
	geom/KDTree.h
	geom/Model.h
	geom/Rect.h
	geom/Line2D.h
	geom/Polygon.h
	geom/Bezier.h
	gfx/Image.h
	gfx/IFormat.h
	gfx/IConvert.h
	gfx/Color.h
	gfx/ifilter/ROFDenoise.h
	gfx/ifilter/Homography.h
	gfx/ifilter/GaussIIR.h
	gfx/ifilter/BrightnessContrast.h
	gfx/ifilter/ITransform.h
#	gfx/ifilter/Sobel.h
	gfx/IFilter.h
	gfx/IScaleFilter.h
	gfx/ImageAllocator.h
	gfx/ImageAllocatorMem.h
	gfx/ImageAllocatorCL.h
	gfx/ImageAllocatorGL.h
	gfx/Drawable.h
	gfx/GFXEngine.h
	gfx/GFXEngineImage.h
	gfx/GFX.h
	gfx/IKernel.h
	gfx/ColorspaceXYZ.h
	gfx/Font.h
	gfx/Alignment.h
	gl/OpenGL.h
	gl/GLFormat.h
	gl/GLContext.h
	gl/GLException.h
	gl/GLProgram.h
	gl/GLBuffer.h
	gl/GLTexture.h
	gl/GLVertexArray.h
	gl/GLModel.h
	gl/GFXEngineGL.h
	gl/GLTexFont.h
	gl/progs/GLBasicProg.h
	gl/progs/GLFillRoundRectProg.h
	gl/progs/GLDrawTextProg.h
	gl/progs/GLDrawImageProg.h
	gl/progs/GLDrawIconProg.h
	gl/progs/GLDrawModelProg.h
	gl/progs/GLFillPathProg.h
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
	gui/Application.h
	gui/BasicTimer.h
	gui/Button.h
	gui/ImageView.h
	gui/Slider.h
	gui/internal/TimerInfo.h
	gui/internal/TimerInfoList.h
	gui/internal/WidgetImplDefault.h
	gui/internal/GLXContext.h
	gui/Moveable.h
	gui/ToggleButton.h
	gui/View3D.h
	gui/Window.h
	gui/Widget.h
	gui/WidgetContainer.h
	gui/WidgetLayout.h
	gui/Label.h
	io/CameraInfo.h
	io/CameraMode.h
	io/CameraModeSet.h
	io/Camera.h
	io/DC1394Camera.h
	io/FileSystem.h
	io/FloFile.h
	io/ImageSequence.h
	io/IOHandler.h
	io/IOSelect.h
	io/Resources.h
	io/VideoInput.h
	io/VideoReader.h
	io/PlyModel.h
	io/Postscript.h
	io/GFXEnginePS.h
	math/CostFunction.h
	math/Fixed.h
	math/GaussNewton.h
	math/LevenbergMarquard.h
	math/Math.h
	math/Matrix.h
	math/Matrix2.h
	math/Matrix3.h
	math/Matrix4.h
	math/Optimization.h
	math/Quaternion.h
	math/SBAData.h
	math/SE3.h
	math/SL3.h
	math/SparseBundleAdjustment.h
	math/Vector.h
	math/Vector2.h
	math/Vector3.h
	math/Vector4.h
	math/graph/Graph.h
	math/graph/GraphNode.h
	math/graph/GraphEdge.h
	math/graph/GraphVisitor.h
	util/CPU.h
	util/CVTTest.h
	util/Delegate.h
	util/Exception.h
	util/ParamInfo.h
	util/ParamSet.h
	util/Range.h
	util/RNG.h
	util/Signal.h
	util/SIMDDebug.h
	util/SIMD.h
	util/SIMDSSE.h
	util/SIMDSSE2.h
	util/SIMDSSE41.h
	util/SIMDSSE42.h
	util/SIMDAVX.h
	util/Thread.h
	util/Util.h
	util/Flags.h
	util/Stack.h
	util/String.h
	util/Plugin.h
	util/PluginManager.h
	util/PluginFile.h
	util/Data.h
    util/DataIterator.h
	vision/AGAST.h
	vision/Brief.h
	vision/CamModel.h
	vision/DLT.h
	vision/ESM.h
	vision/FAST.h
	vision/Feature2D.h
	vision/FeatureExtractor.h
	vision/FeatureMatch.h
	vision/Ferns.h
	vision/internal/Fern.h
	vision/IntegralImage.h
	vision/Flow.h
	vision/HCalibration.h
	vision/MeasurementModel.h
	vision/ORB.h
	vision/Patch.h
	vision/PatchGenerator.h
	vision/PointCorrespondences3d2d.h
	io/xml/XMLAttribute.h
	io/xml/XMLCData.h
	io/xml/XMLComment.h
	io/xml/XMLDecoder.h
	io/xml/XMLDecoderUTF8.h
	io/xml/XMLDocument.h
	io/xml/XMLElement.h
	io/xml/XMLLeaf.h
	io/xml/XMLNode.h
	io/xml/XMLText.h
	${GLSL_SHADER}
	${CL_KERNELS}
)

SOURCE_GROUP( GLSL FILES ${GLSL_SHADER} )
SOURCE_GROUP( CL FILES ${CL_KERNELS} )

SET(${MODULE_NAME}_SOURCES
	cl/CLContext.cpp
	cl/CLException.cpp
	cl/CLKernel.cpp
	gfx/GFX.cpp
	gfx/GFXEngineImage.cpp
	gfx/IConvert.cpp
	gfx/IFormat.cpp
	gfx/Color.cpp
	gfx/Image.cpp
	gfx/ImageOperations.cpp
	gfx/ImageTest.cpp
	gfx/ifilter/ROFDenoise.cpp
	gfx/ifilter/Homography.cpp
	gfx/ifilter/GaussIIR.cpp
	gfx/ifilter/BrightnessContrast.cpp
	gfx/ifilter/ITransform.cpp
#	gfx/ifilter/Sobel.cpp
	gfx/ImageAllocatorCL.cpp
	gfx/ImageAllocatorGL.cpp
	gfx/ImageAllocatorMem.cpp
	gfx/IScaleFilter.cpp
	gfx/IKernel.cpp
	gfx/ColorspaceXYZ.cpp
	geom/KDTree.cpp
	geom/Model.cpp
	geom/Rect.cpp
	gl/GLContext.cpp
	gl/GLBuffer.cpp
	gl/GLModel.cpp
	gl/GLProgram.cpp
	gl/GLTexture.cpp
	gl/GLVertexArray.cpp
	gl/OpenGL.cpp
	gl/GFXEngineGL.cpp
	gl/GLTexFont.cpp
	gl/progs/GLBasicProg.cpp
	gl/progs/GLFillRoundRectProg.cpp
	gl/progs/GLDrawTextProg.cpp
	gl/progs/GLDrawImageProg.cpp
	gl/progs/GLDrawIconProg.cpp
	gl/progs/GLDrawModelProg.cpp
	gl/progs/GLFillPathProg.cpp
	gui/Application.cpp
	gui/BasicTimer.cpp
	gui/Button.cpp
	gui/Moveable.cpp
	gui/ImageView.cpp
	gui/internal/ApplicationX11.cpp
	gui/internal/TimerInfo.cpp
	gui/internal/TimerInfoList.cpp
	gui/internal/WidgetImplWinGLX11.cpp
	gui/internal/X11Handler.cpp
	gui/ToggleButton.cpp
	gui/Widget.cpp
	gui/WidgetContainer.cpp
	gui/Window.cpp
	gui/Label.cpp
	io/CameraInfo.cpp
	io/CameraModeSet.cpp
	io/Camera.cpp
	io/DC1394Camera.cpp
	io/FileSystem.cpp
	io/FloFile.cpp
	io/ImageSequence.cpp
	io/IOSelect.cpp
	io/Resources.cpp
	io/VideoReader.cpp
	io/PlyModel.cpp
	math/ESM.cpp
	math/Fixed.cpp
	math/Math.cpp
	math/Vector.cpp
	math/Matrix.cpp
	math/SE3Test.cpp
	math/SL3Test.cpp
	math/SparseBundleAdjustment.cpp
	util/ParamInfo.cpp
	util/ParamSet.cpp
	util/Range.cpp
	util/SIMD.cpp
	util/SIMDSSE.cpp
	util/SIMDSSE2.cpp
	util/SIMDSSE41.cpp
	util/SIMDSSE42.cpp
	util/SIMDAVX.cpp
	util/SIMDTest.cpp
	util/Time.cpp
	util/String.cpp
	util/PluginManager.cpp
	util/PluginFile.cpp
	vision/AGAST.cpp
	vision/agast/agast5_8.cpp
	vision/agast/agast7_12s.cpp
	vision/agast/agast7_12d.cpp
	vision/agast/oast9_16.cpp
	vision/Brief.cpp
	vision/DLT.cpp
	vision/FAST.cpp
	vision/fast/fast9.cpp
	vision/fast/fast9simd.cpp
	vision/fast/fast10.cpp
	vision/fast/fast11.cpp
	vision/fast/fast12.cpp
	vision/Ferns.cpp
	vision/Flow.cpp
	vision/IntegralImage.cpp
	vision/ORB.cpp
	vision/ORBTest.cpp
	vision/PatchGenerator.cpp
	vision/Patch.cpp
	io/xml/XMLDecoder.cpp
	io/xml/XMLDecoderUTF8.cpp
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

		INCLUDE_DIRECTORIES(
		    BEFORE
			/usr/X11/include
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
	${Eigen2_INCLUDE_DIR}
	${LIBPNG_INCLUDE_DIR}
	${OPENCL_INCLUDE_DIR}
	${LIBDC1394_INCLUDE_DIR}
	${X11_X11_INCLUDE_PATH}
	${FFMPEG_INCLUDES}
)

MODULE_CONFIGURATION("${MODULE_NAME}"
					 "${${MODULE_NAME}_HEADERS}"
					 "${${MODULE_NAME}_SOURCES}"
					 "${${MODULE_NAME}_DEP_LIBRARIES}")

set_target_properties(${MODULE_NAME}
	      PROPERTIES COMPILE_DEFINITIONS
	      PLUGIN_PATH="${CMAKE_BINARY_DIR}/bin/plugins/" )

CVTTESTS(cvt util/cvttestsproto.h)

#link_directories (${CMAKE_BINARY_DIR}/lib/)
IF( ${CMAKE_BUILD_TYPE} MATCHES "debug" )
	add_executable(cvttest util/CVTTest.cpp util/cvttestsproto.h)

	IF( ${CMAKE_GENERATOR} MATCHES "Xcode" )
		SET_TARGET_PROPERTIES(cvttest PROPERTIES PREFIX "../")
	ENDIF()
	target_link_libraries (cvttest ${MODULE_NAME})
ENDIF()

#special flags for some files
set_source_files_properties(util/SIMDSSE.cpp PROPERTIES COMPILE_FLAGS "-mmmx -msse")
set_source_files_properties(util/SIMDSSE2.cpp PROPERTIES COMPILE_FLAGS "-mmmx -msse -msse2")
set_source_files_properties(vision/fast/fast9simd.cpp PROPERTIES COMPILE_FLAGS "-mmmx -msse -msse2")
set_source_files_properties(util/SIMDSSE41.cpp PROPERTIES COMPILE_FLAGS "-mmmx -msse -msse2 -msse3 -mssse3 -msse4.1")
set_source_files_properties(util/SIMDSSE42.cpp PROPERTIES COMPILE_FLAGS "-mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2")
set_source_files_properties(util/SIMDAVX.cpp PROPERTIES COMPILE_FLAGS "-mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2")

