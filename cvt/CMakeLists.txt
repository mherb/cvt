PROJECT(cvt CXX C)

INCLUDE(LibraryConfig)
INCLUDE(MacroModuleConfiguration)

INCLUDE(GLCLTOHEADER)

INCLUDE(CVTTESTS)

SET(MODULE_NAME "cvt")

cmake_policy(SET CMP0012 NEW)

PROJECT(${MODULE_NAME} CXX C)

FIND_PACKAGE(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../doc/Doxyfile.in
${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)

FIND_PACKAGE(Eigen3 REQUIRED)
FIND_PACKAGE(LibPng REQUIRED)
FIND_PACKAGE(ffmpeg REQUIRED)
FIND_PACKAGE(OpenCL REQUIRED)

IF( UNIX AND NOT APPLE )
	FIND_PACKAGE(OpenGL REQUIRED)
	FIND_PACKAGE(X11 REQUIRED)
ENDIF()


# libs
SET(${MODULE_NAME}_DEP_LIBRARIES
	${LIBPNG_LIBRARY}
	${OPENGL_LIBRARIES}
	${OPENCL_LIBRARIES}
	${X11_X11_LIB}
	${FFMPEG_LIBRARIES}
)

IF( UNIX AND NOT APPLE )
	SET(${MODULE_NAME}_DEP_LIBRARIES
		${${MODULE_NAME}_DEP_LIBRARIES}
		rt )
ENDIF()

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
	gl/shader/120/texmls_120.frag
	gl/shader/120/texmls_120.vert
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
	gl/shader/150/texmls_150.frag
	gl/shader/150/texmls_150.vert
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
GLSLTOHEADER(gl/shader/120/texmls_120.frag)
GLSLTOHEADER(gl/shader/120/texmls_120.vert)
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
GLSLTOHEADER(gl/shader/150/texmls_150.frag)
GLSLTOHEADER(gl/shader/150/texmls_150.vert)

SET( CL_KERNELS
	cl/kernel/gaussiir.cl
	cl/kernel/gaussiir2.cl
	cl/kernel/BC.cl
	cl/kernel/clear.cl
	cl/kernel/fill.cl
	cl/kernel/gradx.cl
	cl/kernel/gradxy.cl
	cl/kernel/median3.cl
	cl/kernel/fgp/fgp.cl
	cl/kernel/fgp/fgp_data.cl
	cl/kernel/boxfilter/boxfilter_prefixsum.cl
	cl/kernel/boxfilter/boxfilter.cl
	cl/kernel/prefixsum/prefixsum_pblock.cl
	cl/kernel/prefixsum/prefixsum_pblock_sqr.cl
	cl/kernel/prefixsum/prefixsum_pblock_mul2.cl
	cl/kernel/prefixsum/prefixsum_pblock_mul2_shifted.cl
	cl/kernel/prefixsum/prefixsum_pblock_outerrgb.cl
	cl/kernel/prefixsum/prefixsum_horiz.cl
	cl/kernel/prefixsum/prefixsum_vert.cl
	cl/kernel/prefixsum/prefixsum_block2.cl
	cl/kernel/guidedfilter/guidedfilter_calcab.cl
	cl/kernel/guidedfilter/guidedfilter_calcab_outerrgb.cl
	cl/kernel/guidedfilter/guidedfilter_applyab_gc.cl
	cl/kernel/guidedfilter/guidedfilter_applyab_gc_outer.cl
	cl/kernel/guidedfilter/guidedfilter_applyab_cc.cl
	cl/kernel/pyr/pyrdown_binom3.cl
	cl/kernel/pyr/pyrdown_binom5.cl
	cl/kernel/pyr/pyrup.cl
	cl/kernel/pyr/pyrupmul.cl
	cl/kernel/pyr/pyrdown.cl
	cl/kernel/tvl1flow/tvl1.cl
	cl/kernel/tvl1flow/tvl1_dataadd.cl
	cl/kernel/tvl1flow/tvl1_warp.cl
	cl/kernel/stereogcv/costdepth.cl
	cl/kernel/stereogcv/costdepthgrad.cl
	cl/kernel/stereogcv/costdepthncc.cl
	cl/kernel/stereogcv/costmin.cl
	cl/kernel/stereogcv/costdepthconv.cl
	cl/kernel/stereogcv/occlusioncheck.cl
	cl/kernel/tvl1stereo/tvl1.cl
	cl/kernel/tvl1stereo/tvl1_warp.cl
)

CLTOHEADER( cl/kernel/gaussiir.cl )
CLTOHEADER( cl/kernel/gaussiir2.cl )
CLTOHEADER( cl/kernel/BC.cl )
CLTOHEADER( cl/kernel/clear.cl )
CLTOHEADER( cl/kernel/fill.cl )
CLTOHEADER( cl/kernel/gradx.cl )
CLTOHEADER( cl/kernel/gradxy.cl )
CLTOHEADER( cl/kernel/median3.cl )
CLTOHEADER( cl/kernel/fgp/fgp.cl )
CLTOHEADER( cl/kernel/fgp/fgp_data.cl )
CLTOHEADER( cl/kernel/boxfilter/boxfilter_prefixsum.cl )
CLTOHEADER( cl/kernel/boxfilter/boxfilter.cl )
CLTOHEADER( cl/kernel/prefixsum/prefixsum_pblock.cl )
CLTOHEADER( cl/kernel/prefixsum/prefixsum_pblock_sqr.cl )
CLTOHEADER( cl/kernel/prefixsum/prefixsum_pblock_mul2.cl )
CLTOHEADER( cl/kernel/prefixsum/prefixsum_pblock_mul2_shifted.cl )
CLTOHEADER( cl/kernel/prefixsum/prefixsum_pblock_outerrgb.cl )
CLTOHEADER( cl/kernel/prefixsum/prefixsum_horiz.cl )
CLTOHEADER( cl/kernel/prefixsum/prefixsum_vert.cl )
CLTOHEADER( cl/kernel/prefixsum/prefixsum_block2.cl )
CLTOHEADER( cl/kernel/guidedfilter/guidedfilter_calcab.cl )
CLTOHEADER( cl/kernel/guidedfilter/guidedfilter_calcab_outerrgb.cl )
CLTOHEADER( cl/kernel/guidedfilter/guidedfilter_applyab_gc.cl )
CLTOHEADER( cl/kernel/guidedfilter/guidedfilter_applyab_gc_outer.cl )
CLTOHEADER( cl/kernel/guidedfilter/guidedfilter_applyab_cc.cl )
CLTOHEADER( cl/kernel/pyr/pyrdown_binom3.cl )
CLTOHEADER( cl/kernel/pyr/pyrdown_binom5.cl )
CLTOHEADER( cl/kernel/pyr/pyrup.cl )
CLTOHEADER( cl/kernel/pyr/pyrupmul.cl )
CLTOHEADER( cl/kernel/pyr/pyrdown.cl )
CLTOHEADER( cl/kernel/tvl1flow/tvl1.cl )
CLTOHEADER( cl/kernel/tvl1flow/tvl1_dataadd.cl )
CLTOHEADER( cl/kernel/tvl1flow/tvl1_warp.cl )
CLTOHEADER( cl/kernel/stereogcv/costdepth.cl )
CLTOHEADER( cl/kernel/stereogcv/costdepthgrad.cl )
CLTOHEADER( cl/kernel/stereogcv/costdepthncc.cl )
CLTOHEADER( cl/kernel/stereogcv/costmin.cl )
CLTOHEADER( cl/kernel/stereogcv/costdepthconv.cl )
CLTOHEADER( cl/kernel/stereogcv/occlusioncheck.cl )
CLTOHEADER( cl/kernel/tvl1stereo/tvl1.cl )
CLTOHEADER( cl/kernel/tvl1stereo/tvl1_warp.cl )


SET(${MODULE_NAME}_HEADERS
	cl/CLBuffer.h
	cl/CLCommandQueue.h
	cl/CLContext.h
	cl/CLDevice.h
	cl/CLEvent.h
	cl/CLException.h
	cl/CLImage2D.h
	cl/CLImageFormat.h
	cl/CLKernel.h
	cl/CLLocalSpace.h
	cl/CLNDRange.h
	cl/CLObject.h
	cl/CLObject.inl
	cl/CLPlatform.h
	cl/CLProgram.h
	cl/CLUtil.h
	cl/OpenCL.h
	cl/kernel/gaussiir.h
	cl/kernel/gaussiir2.h
	cl/kernel/BC.h
	cl/kernel/clear.h
	cl/kernel/fill.h
	cl/kernel/median3.h
	cl/kernel/gradx.h
	cl/kernel/gradxy.h
	cl/kernel/fgp/fgp.h
	cl/kernel/fgp/fgp_data.h
	cl/kernel/boxfilter/boxfilter_prefixsum.h
	cl/kernel/boxfilter/boxfilter.h
	cl/kernel/prefixsum/prefixsum_pblock.h
	cl/kernel/prefixsum/prefixsum_pblock_sqr.h
	cl/kernel/prefixsum/prefixsum_pblock_mul2.h
	cl/kernel/prefixsum/prefixsum_pblock_mul2_shifted.h
	cl/kernel/prefixsum/prefixsum_pblock_outerrgb.h
	cl/kernel/prefixsum/prefixsum_horiz.h
	cl/kernel/prefixsum/prefixsum_vert.h
	cl/kernel/prefixsum/prefixsum_block2.h
	cl/kernel/guidedfilter/guidedfilter_calcab.h
	cl/kernel/guidedfilter/guidedfilter_calcab_outerrgb.h
	cl/kernel/guidedfilter/guidedfilter_applyab_gc.h
	cl/kernel/guidedfilter/guidedfilter_applyab_gc_outer.h
	cl/kernel/guidedfilter/guidedfilter_applyab_cc.h
	cl/kernel/pyr/pyrdown_binom3.h
	cl/kernel/pyr/pyrdown_binom5.h
	cl/kernel/pyr/pyrdown.h
	cl/kernel/pyr/pyrup.h
	cl/kernel/pyr/pyrupmul.h
	cl/kernel/tvl1flow/tvl1.h
	cl/kernel/tvl1flow/tvl1_warp.h
	cl/kernel/tvl1flow/tvl1_dataadd.h
	cl/kernel/stereogcv/costdepth.h
	cl/kernel/stereogcv/costdepthgrad.h
	cl/kernel/stereogcv/costdepthncc.h
	cl/kernel/stereogcv/costmin.h
	cl/kernel/stereogcv/costdepthconv.h
	cl/kernel/stereogcv/occlusioncheck.h
	cl/kernel/tvl1stereo/tvl1.h
	cl/kernel/tvl1stereo/tvl1_warp.h
	geom/ArcBall.h
	geom/Bezier.h
	geom/KDTree.h
	geom/Line2D.h
	geom/Model.h
	geom/Polygon.h
	geom/PointSet.h
	geom/Rect.h
	geom/Box.h
	geom/scene/SceneCamera.h
	geom/scene/SceneGeometry.h
	geom/scene/Scene.h
	geom/scene/SceneLight.h
	geom/scene/SceneLoader.h
	geom/scene/SceneMaterial.h
	geom/scene/SceneMesh.h
	geom/scene/SceneNode.h
	geom/scene/SceneSpatial.h
	geom/scene/SceneTexture.h
	gfx/Color.h
	gfx/Image.h
	gfx/IFormat.h
	gfx/IConvert.h
	gfx/ifilter/ROFDenoise.h
	gfx/ifilter/ROFFGPFilter.h
	gfx/ifilter/Homography.h
	gfx/ifilter/GaussIIR.h
	gfx/ifilter/BrightnessContrast.h
	gfx/ifilter/ITransform.h
	gfx/ifilter/IWarp.h
	gfx/ifilter/IntegralFilter.h
	gfx/ifilter/BoxFilter.h
	gfx/ifilter/GuidedFilter.h
	gfx/ifilter/StereoGCVFilter.h
	gfx/ifilter/TVL1Flow.h
	gfx/ifilter/TVL1Stereo.h
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
	gl/GLFBO.h
	gl/GLFormat.h
	gl/GLContext.h
	gl/GLException.h
	gl/GLProgram.h
	gl/GLBuffer.h
	gl/GLTexture.h
	gl/GLVertexArray.h
	gl/GLModel.h
	gl/GFXEngineGL.h
	gl/GFXEngineGLFBO.h
	gl/GLRBO.h
	gl/GLTexFont.h
	gl/GLTeapot.h
	gl/progs/GLBasicProg.h
	gl/progs/GLFillRoundRectProg.h
	gl/progs/GLDrawTextProg.h
	gl/progs/GLDrawImageProg.h
	gl/progs/GLDrawIconProg.h
	gl/progs/GLDrawModelProg.h
	gl/progs/GLFillPathProg.h
	gl/progs/GLTexMLSProg.h
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
	gl/shader/120/texmls_120_frag.h
	gl/shader/120/texmls_120_vert.h
	gl/shader/150/texmls_150_frag.h
	gl/shader/150/texmls_150_vert.h
	gui/Application.h
	gui/BasicTimer.h
	gui/Button.h
	gui/GLView.h
	gui/ImageView.h
	gui/internal/TimerInfo.h
	gui/internal/TimerInfoList.h
	gui/internal/WidgetImplDefault.h
	gui/Label.h
	gui/Moveable.h
	gui/PlotView.h
	gui/PlotData.h
	gui/PlotDataSamples.h
	gui/Slider.h
	gui/ToggleButton.h
	gui/Window.h
	gui/Widget.h
	gui/WidgetContainer.h
	gui/WidgetLayout.h
	io/CameraInfo.h
	io/CameraMode.h
	io/CameraModeSet.h
	io/Camera.h
	io/FileSystem.h
	io/FloFile.h
	io/ImageSequence.h
	io/IOHandler.h
	io/IOSelect.h
	io/Resources.h
	io/RawVideoWriter.h
	io/RawVideoReader.h
	io/VideoInput.h
	io/VideoReader.h
	io/Postscript.h
	io/GFXEnginePS.h
	io/Host.h
	io/Socket.h
	io/TCPClient.h
	io/TCPServer.h
	io/UDPClient.h
	io/AsyncTCPServer.h
	io/AsyncTCPClient.h
	io/AsyncUDPClient.h
	math/CostFunction.h
	math/Complex.h
	math/Fixed.h
	math/JointMeasurements.h
	math/GaussNewton.h
	math/LevenbergMarquard.h
	math/Math.h
	math/Matrix.h
	math/Matrix2.h
	math/Matrix3.h
	math/Matrix4.h
	math/TerminationCriteria.h
	math/Polynomial.h
	math/Quaternion.h
	math/sac/SampleConsensusModel.h
	math/sac/RANSAC.h
	math/sac/Line2DSAC.h
	math/sac/HomographySAC.h
	math/sac/EssentialSAC.h
	math/sac/EPnPSAC.h
	math/SE3.h
	math/SL3.h
	math/Translation2D.h
	math/Sim2.h
	math/GA2.h
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
	util/Condition.h
	util/Delegate.h
	util/Exception.h
	util/EigenBridge.h
	util/Inserter.h
	util/Mutex.h
	util/ParamInfo.h
	util/ParamSet.h
	util/Range.h
	util/RNG.h
	util/Signal.h
	util/SIMDDebug.h
	util/SIMD.h
	util/SIMDSSE.h
	util/SIMDSSE2.h
	util/SIMDSSE3.h
	util/SIMDSSSE3.h
	util/SIMDSSE41.h
	util/SIMDSSE42.h
	util/SIMDAVX.h
	util/TQueue.h
	util/Thread.h
	util/Time.h
	util/Util.h
	util/Flags.h
	util/Stack.h
	util/String.h
	util/Plugin.h
	util/PluginManager.h
	util/PluginFile.h
	util/Data.h
    util/DataIterator.h
	vision/BinaryDescriptor.h
	vision/Brief.h
	vision/CameraCalibration.h
	vision/DLT.h
	vision/ESM.h
	vision/EPnP.h
	vision/FAST.h
	vision/fast/FAST.inl
	vision/Feature2D.h
	vision/FeatureMatcher.h
	vision/FeatureMatch.h
	vision/Ferns.h
	vision/internal/Fern.h
	vision/IntegralImage.h
	vision/Flow.h
	vision/HCalibration.h
	vision/KLTPatch.h
	vision/KLTTracker.h
	vision/LSH.h
	vision/MeasurementModel.h
	vision/ORB.h
	vision/ORBPatterns.h
	vision/ORB2.h
	vision/ORB2Patterns.h
	vision/Patch.h
	vision/PatchGenerator.h
	vision/PointCorrespondences3d2d.h
	vision/Vision.h
	vision/SparseBundleAdjustment.h
	vision/slam/SlamMap.h
	vision/slam/Keyframe.h
	vision/slam/MapFeature.h
	vision/slam/MapMeasurement.h
	vision/slam/stereo/StereoSLAM.h
	vision/slam/stereo/DescriptorDatabase.h
	vision/slam/stereo/FeatureTracking.h
	vision/slam/stereo/FASTFeatureTracking.h
	vision/slam/stereo/ORBTracking.h
	vision/slam/stereo/ORBTrackingSequential.h
	vision/slam/stereo/KLTTracking.h
	vision/slam/stereo/ORBStereoMatching.h
	vision/slam/stereo/MapOptimizer.h
	vision/slam/stereo/FeatureAnalyzer.h
	io/xml/XMLAttribute.h
	io/xml/XMLCData.h
	io/xml/XMLComment.h
	io/xml/XMLDecl.h
	io/xml/XMLDecoder.h
	io/xml/XMLDecoderUTF8.h
	io/xml/XMLDocument.h
	io/xml/XMLElement.h
	io/xml/XMLEncoding.h
	io/xml/XMLLeaf.h
	io/xml/XMLNode.h
	io/xml/XMLText.h
	${GLSL_SHADER}
	${CL_KERNELS}
)

SOURCE_GROUP( GLSL FILES ${GLSL_SHADER} )
SOURCE_GROUP( CL FILES ${CL_KERNELS} )

SET(${MODULE_NAME}_SOURCES
	cl/OpenCL.cpp
	cl/CLBuffer.cpp
	cl/CLCommandQueue.cpp
	cl/CLContext.cpp
	cl/CLDevice.cpp
	cl/CLEvent.cpp
	cl/CLException.cpp
	cl/CLImage2D.cpp
	cl/CLImageFormat.cpp
	cl/CLKernel.cpp
	cl/CLProgram.cpp
	gfx/GFX.cpp
	gfx/GFXEngineImage.cpp
	gfx/IConvert.cpp
	gfx/IFormat.cpp
	gfx/Color.cpp
	gfx/Image.cpp
	gfx/ImageOperations.cpp
	gfx/ImageTest.cpp
	gfx/ifilter/ROFDenoise.cpp
	gfx/ifilter/ROFFGPFilter.cpp
	gfx/ifilter/Homography.cpp
	gfx/ifilter/GaussIIR.cpp
	gfx/ifilter/BrightnessContrast.cpp
	gfx/ifilter/ITransform.cpp
	gfx/ifilter/IWarp.cpp
	gfx/ifilter/IntegralFilter.cpp
	gfx/ifilter/BoxFilter.cpp
	gfx/ifilter/GuidedFilter.cpp
	gfx/ifilter/StereoGCVFilter.cpp
	gfx/ifilter/TVL1Flow.cpp
	gfx/ifilter/TVL1Stereo.cpp
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
	geom/PointSetTest.cpp
	geom/scene/Scene.cpp
	geom/scene/SceneGeometry.cpp
	geom/scene/SceneMesh.cpp
	gl/GLContext.cpp
	gl/GLBuffer.cpp
	gl/GLFBO.cpp
	gl/GLModel.cpp
	gl/GLProgram.cpp
	gl/GLTexture.cpp
	gl/GLVertexArray.cpp
	gl/OpenGL.cpp
	gl/GFXEngineGL.cpp
	gl/GLTexFont.cpp
	gl/GLTeapot.cpp
	gl/progs/GLBasicProg.cpp
	gl/progs/GLFillRoundRectProg.cpp
	gl/progs/GLDrawTextProg.cpp
	gl/progs/GLDrawImageProg.cpp
	gl/progs/GLDrawIconProg.cpp
	gl/progs/GLDrawModelProg.cpp
	gl/progs/GLFillPathProg.cpp
	gl/progs/GLTexMLSProg.cpp
	gui/Application.cpp
	gui/BasicTimer.cpp
	gui/Button.cpp
	gui/GLView.cpp
	gui/Moveable.cpp
	gui/PlotView.cpp
	gui/ImageView.cpp
	gui/internal/TimerInfo.cpp
	gui/internal/TimerInfoList.cpp
	gui/ToggleButton.cpp
	gui/Widget.cpp
	gui/WidgetContainer.cpp
	gui/Window.cpp
	gui/Label.cpp
	io/CameraInfo.cpp
	io/CameraModeSet.cpp
	io/Camera.cpp
	io/FileSystem.cpp
	io/FloFile.cpp
	io/ImageSequence.cpp
	io/IOSelect.cpp
	io/Resources.cpp
	io/RawVideoWriter.cpp
	io/RawVideoReader.cpp
	io/VideoReader.cpp
	io/Host.cpp
	io/Socket.cpp
	io/TCPClient.cpp
	io/TCPServer.cpp
	io/UDPClient.cpp
	math/Complex.cpp
	math/ESM.cpp
	math/Fixed.cpp
	math/JointMeasurements.cpp
	math/JointMeasurementsTest.cpp
	math/Math.cpp
	math/Vector.cpp
	math/Matrix.cpp
	math/Polynomial.cpp
	math/SE3Test.cpp
	math/SL3Test.cpp
	math/Sim2Test.cpp
	math/GA2Test.cpp
	util/ParamInfo.cpp
	util/ParamSet.cpp
	util/Range.cpp
	util/SIMD.cpp
	util/SIMDSSE.cpp
	util/SIMDSSE2.cpp
	util/SIMDSSE3.cpp
	util/SIMDSSSE3.cpp
	util/SIMDSSE41.cpp
	util/SIMDSSE42.cpp
	util/SIMDAVX.cpp
	util/SIMDTest.cpp
	util/Time.cpp
	util/String.cpp
	util/PluginManager.cpp
	util/PluginFile.cpp
	vision/Brief.cpp
	vision/CameraCalibrationTest.cpp
	vision/DLT.cpp
	vision/EPnP.cpp
	vision/FAST.cpp
	vision/fast/fast9.cpp
	vision/fast/fast10.cpp
	vision/fast/fast11.cpp
	vision/fast/fast12.cpp
	vision/Ferns.cpp
	vision/Flow.cpp
	vision/IntegralImage.cpp
	vision/ORB.cpp
	vision/ORB2.cpp
	vision/ORB2Test.cpp
	vision/ORBTest.cpp
	vision/PatchGenerator.cpp
	vision/Patch.cpp
	vision/SparseBundleAdjustment.cpp
	vision/slam/Keyframe.cpp
	vision/slam/SlamMap.cpp
	vision/slam/stereo/FeatureTracking.cpp
	vision/slam/stereo/FASTFeatureTracking.cpp
	vision/slam/stereo/KLTTracking.cpp
	vision/slam/stereo/ORBTracking.cpp
	vision/slam/stereo/ORBTrackingSequential.cpp
	vision/slam/stereo/StereoSLAM.cpp
	vision/Vision.cpp
	io/xml/XMLDecoder.cpp
	io/xml/XMLDecoderUTF8.cpp
)


IF(UNIX)
	IF(APPLE)
		SET(${MODULE_NAME}_HEADERS ${${MODULE_NAME}_HEADERS}
			io/QTKitCamera.h
			gui/internal/OSX/ApplicationOSX.h
			gui/internal/OSX/WidgetImplWinGLOSX.h
			gui/internal/OSX/OSXGLView.h
			gui/internal/OSX/OSXTimerHandler.h
			gui/internal/OSX/TimerInfoListOSX.h
			gui/internal/OSX/TimerInfoOSX.h
		)

		SET(${MODULE_NAME}_SOURCES ${${MODULE_NAME}_SOURCES}
			io/QTKitCamera.mm
			gui/internal/OSX/ApplicationOSX.mm
			gui/internal/OSX/WidgetImplWinGLOSX.mm
			gui/internal/OSX/OSXGLView.mm
			gui/internal/OSX/TimerInfoListOSX.cpp
			gui/internal/OSX/OSXTimerHandler.mm
			gui/internal/OSX/TimerInfoOSX.mm
		)

		SET(${MODULE_NAME}_DEP_LIBRARIES
			${${MODULE_NAME}_DEP_LIBRARIES}
			"-framework Cocoa"
			"-framework CoreFoundation"
			"-framework QuartzCore"
			"-framework QTKit"
			"-framework OpenGL"
		)
	ELSE(APPLE)
		SET(${MODULE_NAME}_HEADERS ${${MODULE_NAME}_HEADERS}
			io/V4L2Camera.h
			gui/internal/X11/GLXContext.h
			gui/internal/X11/ApplicationX11.h
			gui/internal/X11/WidgetImplWinGLX11.h
			gui/internal/X11/X11Handler.h
		)
		SET(${MODULE_NAME}_SOURCES ${${MODULE_NAME}_SOURCES}
			io/V4L2Camera.cpp
			gui/internal/X11/ApplicationX11.cpp
			gui/internal/X11/WidgetImplWinGLX11.cpp
			gui/internal/X11/X11Handler.cpp
		)
	ENDIF(APPLE)
ENDIF()

# optional uEyeUsbCamera driver if available
FIND_PACKAGE(uEyeUsbCamera)
OPTION( CVT_USEUEYE "Use UEyeUsbCamera" TRUE )
IF( UEYEUSB_FOUND AND CVT_USEUEYE )
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
	ADD_DEFINITIONS( -DUEYE_FOUND )
ENDIF()

OPTION( CVT_USE_DC1394 "Use DC1394 library if available" TRUE )

IF( ${CVT_USE_DC1394} )
	FIND_PACKAGE(DC1394)
	IF( ${LIBDC1394_FOUND} )
		SET(${MODULE_NAME}_HEADERS
			${${MODULE_NAME}_HEADERS}
			io/DC1394Camera.h
		)
		SET(${MODULE_NAME}_SOURCES
			${${MODULE_NAME}_SOURCES}
			io/DC1394Camera.cpp
		)
		INCLUDE_DIRECTORIES(
			BEFORE
			${LIBDC1394_INCLUDE_DIR}
		)
		SET(${MODULE_NAME}_DEP_LIBRARIES
			${${MODULE_NAME}_DEP_LIBRARIES}
			${LIBDC1394_LIBRARY}
		)
	ENDIF()
ENDIF()

FIND_PACKAGE( OpenNI )
OPTION( CVT_USE_OPENNI "Use OpenNI" TRUE )
IF( OPENNI_FOUND AND CVT_USE_OPENNI )
	SET(${MODULE_NAME}_HEADERS
		${${MODULE_NAME}_HEADERS}
		io/OpenNICamera.h
		io/OpenNIManager.h
	)
	SET(${MODULE_NAME}_SOURCES
		${${MODULE_NAME}_SOURCES}
		io/OpenNICamera.cpp
		io/OpenNIManager.cpp
	)
	INCLUDE_DIRECTORIES(
		BEFORE
		${OPENNI_INCLUDES}
	)
	SET(${MODULE_NAME}_DEP_LIBRARIES
		${${MODULE_NAME}_DEP_LIBRARIES}
		${OPENNI_LIBRARIES}
	)
ENDIF()

# include directories
INCLUDE_DIRECTORIES(
    BEFORE
	${CMAKE_CURRENT_SOURCE_DIR}
	${EIGEN3_INCLUDE_DIR}
	${LIBPNG_INCLUDE_DIR}
	${OPENCL_INCLUDE_DIR}
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
IF( ${CMAKE_BUILD_TYPE} MATCHES "DEBUG" )
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
set_source_files_properties(vision/internal/orbsimd.cpp PROPERTIES COMPILE_FLAGS "-mmmx -msse -msse2")
set_source_files_properties(util/SIMDSSE3.cpp PROPERTIES COMPILE_FLAGS "-mmmx -msse -msse2 -msse3")
set_source_files_properties(util/SIMDSSSE3.cpp PROPERTIES COMPILE_FLAGS "-mmmx -msse -msse2 -msse3 -mssse3")
set_source_files_properties(util/SIMDSSE41.cpp PROPERTIES COMPILE_FLAGS "-mmmx -msse -msse2 -msse3 -mssse3 -msse4.1")
set_source_files_properties(util/SIMDSSE42.cpp PROPERTIES COMPILE_FLAGS "-mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2")
set_source_files_properties(util/SIMDAVX.cpp PROPERTIES COMPILE_FLAGS "-mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2")

