PROJECT(cvt CXX C)

INCLUDE(LibraryConfig)
INCLUDE(MacroModuleConfiguration)

INCLUDE(GLCLTOHEADER)

INCLUDE(CVTTESTS)

SET(MODULE_NAME "cvt")

cmake_policy(SET CMP0012 NEW)

PROJECT(${MODULE_NAME} CXX C)

SET( CMAKE_LIBRARY_PATH
	${CMAKE_LIBRARY_PATH}
	"/usr/lib/nvidia-current" )

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

CONFIGURE_FILE (
   "CVTVersion.h.in"
   "${CMAKE_SOURCE_DIR}/cvt/util/Version.h"
)
# TODO: this file needs to be shipped as header!

IF( UNIX AND NOT APPLE )
   SET(${MODULE_NAME}_DEP_LIBRARIES
      ${${MODULE_NAME}_DEP_LIBRARIES}
      rt
      pthread )
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

SET( CL_KERNELS
    cl/kernel/gaussiir.cl
    cl/kernel/gaussiir2.cl
    cl/kernel/BC.cl
    cl/kernel/clear.cl
    cl/kernel/clearbuffer.cl
    cl/kernel/fill.cl
    cl/kernel/imageutil.cl
    cl/kernel/gradx.cl
    cl/kernel/gradxy.cl
    cl/kernel/median3.cl
        cl/kernel/FlowColorCode.cl
    cl/kernel/fgp/fgp.cl
    cl/kernel/fgp/fgp_data.cl
    cl/kernel/fgp/fgpgrayweightedhuber.cl
    cl/kernel/fgp/fgpgrayweightedhuber_data.cl
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
    cl/kernel/stereo/stereoCV.cl
    cl/kernel/stereo/stereoCVFBP.cl
    cl/kernel/stereo/stereoCVQSearch.cl
    cl/kernel/stereogcv/costdepth.cl
    cl/kernel/stereogcv/costdepthgrad.cl
    cl/kernel/stereogcv/costdepthncc.cl
    cl/kernel/stereogcv/costmin.cl
    cl/kernel/stereogcv/costdepthconv.cl
    cl/kernel/stereogcv/occlusioncheck.cl
    cl/kernel/tvl1stereo/tvl1.cl
    cl/kernel/tvl1stereo/tvl1_warp.cl
    cl/kernel/TSDFVolume/TSDFVolume.cl
	cl/kernel/PDROF.cl
	cl/kernel/PDHuber.cl
	cl/kernel/PDHuberWeighted.cl
	cl/kernel/PDHuberWeightedInpaint.cl
)

FOREACH( CLFILE ${CL_KERNELS} )
   CLTOHEADER( ${CLFILE} )
   # add it to the generated cl headers
   SET( CL_GEN_HEADER_${MODULE_NAME} ${CL_GEN_HEADER_${MODULE_NAME}} ${CL_GEN_OUTPUT} )
ENDFOREACH(CLFILE)

FOREACH( GLSLFILE ${GLSL_SHADER} )
   GLSLTOHEADER( ${GLSLFILE} )
   SET( GLSL_GEN_HEADER_${MODULE_NAME} ${GLSL_GEN_HEADER_${MODULE_NAME}} ${GLSL_GEN_OUTPUT} )
ENDFOREACH(GLSLFILE)


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
   container/List.h
   container/RBTree.h
   geom/ArcBall.h
   geom/Box.h
   geom/CBezier.h
   geom/Ellipse.h
   geom/KDTree.h
   geom/Line2D.h
   geom/MarchingCubes.h
   geom/Polygon.h
   geom/PointSet.h
   geom/QBezier.h
   geom/Rect.h
   geom/scene/Scene.h
   geom/scene/SceneCamera.h
   geom/scene/SceneGeometry.h
   geom/scene/SceneLight.h
   geom/scene/SceneLines.h
   geom/scene/SceneLoader.h
   geom/scene/SceneMaterial.h
   geom/scene/SceneMesh.h
   geom/scene/SceneNode.h
   geom/scene/ScenePoints.h
   geom/scene/SceneSpatial.h
   geom/scene/SceneTexture.h
   gfx/Color.h
   gfx/Image.h
   gfx/IBoxFilter.h
   gfx/IFill.h
   gfx/IFormat.h
   gfx/IComponents.h
   gfx/IConvert.h
   gfx/IConvolve.h
   gfx/IMapScoped.h
   gfx/IMorphological.h
   gfx/IThreshold.h
   gfx/ILoader.h
   gfx/ISaver.h
   gfx/ifilter/AdaptiveThreshold.h
   gfx/ifilter/ColorCode.h
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
   gfx/ifilter/Threshold.h
   gfx/ifilter/TVL1Flow.h
   gfx/ifilter/TVL1Stereo.h
   gfx/ifilter/MorphErode.h
   gfx/ifilter/MorphDilate.h
#	gfx/ifilter/Sobel.h
   gfx/IFilter.h
   gfx/IScaleFilter.h
   gfx/ImageAllocator.h
   gfx/ImageAllocatorMem.h
   gfx/ImageAllocatorCL.h
   gfx/ImageAllocatorGL.h
   gfx/Clipping.h
   gfx/Drawable.h
   gfx/GFXEngine.h
   gfx/GFXEngineImage.h
   gfx/GFX.h
   gfx/IKernel.h
   gfx/ColorspaceXYZ.h
   gfx/Font.h
   gfx/Alignment.h
   gfx/Path.h
   gfx/DummyFont.h
   gl/OpenGL.h
   gl/GLFBO.h
   gl/GLFormat.h
   gl/GLContext.h
   gl/GLException.h
   gl/GLProgram.h
   gl/GLBuffer.h
   gl/GLLines.h
   gl/GLPoints.h
   gl/GLTexture.h
   gl/GLVertexArray.h
   gl/GFXEngineGL.h
   gl/GFXEngineGLFBO.h
   gl/GLRBO.h
   gl/GLTexFont.h
   gl/GLTeapot.h
   gl/GLTeapot.inl
   gl/progs/GLBasicProg.h
   gl/progs/GLFillRoundRectProg.h
   gl/progs/GLDrawTextProg.h
   gl/progs/GLDrawImageProg.h
   gl/progs/GLDrawIconProg.h
   gl/progs/GLDrawModelProg.h
   gl/progs/GLFillPathProg.h
   gl/progs/GLTexMLSProg.h
   gl/scene/GLSBaseModel.h
   gl/scene/GLSCamera.h
   gl/scene/GLSLight.h
   gl/scene/GLSMaterial.h
   gl/scene/GLSRenderable.h
   gl/scene/GLSRenderableGroup.h
   gl/scene/GLSShader.h
   gl/scene/GLSTransform.h
   gl/scene/GLSTransformable.h
   gl/scene/GLSVisitor.h
   gl/scene/GLScene.h
   gui/Application.h
   gui/BasicTimer.h
   gui/Button.h
   gui/Events.h
   gui/event/CloseEvent.h
   gui/event/Event.h
   gui/event/HideEvent.h
   gui/event/MouseMoveEvent.h
   gui/event/MousePressEvent.h
   gui/event/MouseReleaseEvent.h
   gui/event/MoveEvent.h
   gui/event/PaintEvent.h
   gui/event/ResizeEvent.h
   gui/event/ShowEvent.h
   gui/GLView.h
   gui/ImageView.h
   gui/internal/TimerInfo.h
   gui/internal/TimerInfoList.h
   gui/internal/WidgetImplDefault.h
   gui/Label.h
   gui/Moveable.h
   gui/ParamSetView.h
   gui/ParamView.h
   gui/PlotView.h
   gui/PlotData.h
   gui/PlotDataSamples.h
   gui/Slider.h
   gui/TimeoutHandler.h
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
   io/RGBDParser.h
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
   math/FFT.h
   math/Fixed.h
   math/Function.h
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
   math/sac/P3PSac.h
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
   math/SparseBlockMatrix.h
   util/Version.h
   util/CPU.h
   util/CVTAssert.h
   util/CVTTest.h
   util/Condition.h
   util/ConfigFile.h
   util/Delegate.h
   util/Delegate.inl
   util/Data.h
   util/DataIterator.h
   util/Exception.h
   util/EigenBridge.h
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
   util/internal/ParamTypes.def
   vision/BinaryDescriptor.h
   vision/BoardDetector.h
   vision/Brief.h
   vision/CameraCalibration.h
   vision/ESM.h
   vision/EPnP.h
   vision/features/BRIEF.h
   vision/features/BRIEFPattern.h
   vision/features/FAST.h
   vision/features/fast/FAST.inl
   vision/features/Feature.h
   vision/features/FeatureDescriptor.h
   vision/features/FeatureDescriptorExtractor.h
   vision/features/FeatureDetector.h
   vision/features/FeatureSet.h
   vision/features/Harris.h
   vision/features/NMSFilter.h
# vision/Ferns.h
#   vision/internal/Fern.h
   vision/IntegralImage.h
   vision/ImagePyramid.h
   vision/Flow.h
   vision/HCalibration.h
   vision/KLTPatch.h
   vision/LSH.h
   vision/MeasurementModel.h
   vision/features/ORB.h
   vision/features/ORBPattern.h
#   vision/ORB2.h
#   vision/ORB2Patterns.h
   vision/Patch.h
   vision/PatchGenerator.h
   vision/PointCorrespondences3d2d.h
   vision/StereoCameraCalibration.h
   vision/TSDFVolume.h
   vision/Vision.h
   vision/SparseBundleAdjustment.h
   vision/rgbdvo/ApproxMedian.h
   vision/rgbdvo/ESMKeyframe.h
   vision/rgbdvo/FCKeyframe.h
   vision/rgbdvo/ICKeyframe.h
   vision/rgbdvo/InformationSelection.h
   vision/rgbdvo/Optimizer.h
   vision/rgbdvo/GNOptimizer.h
   vision/rgbdvo/LMOptimizer.h
   vision/rgbdvo/DLOptimizer.h
   vision/rgbdvo/RGBDKeyframe.h
   vision/rgbdvo/RGBDVisualOdometry.h
   vision/rgbdvo/RGBDWarp.h
   vision/rgbdvo/RobustWeighting.h
   vision/rgbdvo/SystemBuilder.h
   vision/slam/SlamMap.h
   vision/slam/Keyframe.h
   vision/slam/MapFeature.h
   vision/slam/MapMeasurement.h
   vision/slam/stereo/StereoSLAM.h
   vision/slam/stereo/DescriptorDatabase.h
   vision/slam/stereo/FeatureTracking.h
#  vision/slam/stereo/ORBTracking.h
   vision/slam/stereo/KLTTracking.h
#   vision/slam/stereo/ORBStereoMatching.h
   vision/slam/stereo/MapOptimizer.h
   vision/slam/stereo/FeatureAnalyzer.h
   vision/slam/stereo/DepthInitializer.h
#   vision/slam/stereo/ORBStereoInit.h
#   vision/slam/stereo/PatchStereoInit.h
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
   io/xml/XMLSerializable.h
   io/xml/XMLText.h
   ${GLSL_GEN_HEADER_${MODULE_NAME}}
   ${CL_GEN_HEADER_${MODULE_NAME}}
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
   gfx/IBoxFilter.cpp
   gfx/IConvert.cpp
   gfx/IConvolve.cpp
   gfx/IFill.cpp
   gfx/IFormat.cpp
   gfx/Color.cpp
   gfx/Image.cpp
   gfx/ImageOperations.cpp
   gfx/ImageTest.cpp
   gfx/IMorphological.cpp
   gfx/IThreshold.cpp
   gfx/ifilter/AdaptiveThreshold.cpp
   gfx/ifilter/ColorCode.cpp
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
   gfx/ifilter/Threshold.cpp
   gfx/ifilter/TVL1Flow.cpp
   gfx/ifilter/TVL1Stereo.cpp
   gfx/ifilter/MorphErode.cpp
   gfx/ifilter/MorphDilate.cpp
#	gfx/ifilter/Sobel.cpp
   gfx/ImageAllocatorCL.cpp
   gfx/ImageAllocatorGL.cpp
   gfx/ImageAllocatorMem.cpp
   gfx/IScaleFilter.cpp
   gfx/IKernel.cpp
   gfx/ColorspaceXYZ.cpp
   geom/KDTreeTest.cpp
   geom/MarchingCubes.cpp
   geom/Rect.cpp
   geom/PointSet.cpp
   geom/PointSetTest.cpp
   geom/scene/Scene.cpp
   geom/scene/SceneGeometry.cpp
   geom/scene/SceneMesh.cpp
   gl/GLContext.cpp
   gl/GLBuffer.cpp
   gl/GLFBO.cpp
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
   gl/scene/GLSVisitor.cpp
   gl/scene/GLSRenderableGroup.cpp
   gl/scene/GLSBaseModel.cpp
   gui/Application.cpp
   gui/BasicTimer.cpp
   gui/Button.cpp
   gui/GLView.cpp
   gui/Moveable.cpp
   gui/ParamSetView.cpp
   gui/ParamView.cpp
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
   io/RGBDParser.cpp
   io/VideoReader.cpp
   io/Host.cpp
   io/Socket.cpp
   io/TCPClient.cpp
   io/TCPServer.cpp
   io/UDPClient.cpp
   math/Complex.cpp
   math/FFT.cpp
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
   util/ConfigFile.cpp
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
   vision/BoardDetector.cpp
   vision/CameraCalibrationTest.cpp
   vision/features/FAST.cpp
   vision/features/fast/fast9.cpp
   vision/features/fast/fast10.cpp
   vision/features/fast/fast11.cpp
   vision/features/fast/fast12.cpp
   vision/features/FeatureSet.cpp
   vision/features/Harris.cpp
# vision/Ferns.cpp
   vision/Flow.cpp
   vision/IntegralImage.cpp
   vision/ImagePyramidTest.cpp
   vision/KLTPatchTest.cpp
   vision/features/ORB.cpp
#   vision/ORB2.cpp
#   vision/ORB2Test.cpp
#   vision/ORBTest.cpp
   vision/PatchGenerator.cpp
   vision/Patch.cpp
   vision/SparseBundleAdjustment.cpp
   vision/rgbdvo/InformationSelectionTest.cpp
   vision/slam/Keyframe.cpp
   vision/slam/SlamMap.cpp
   vision/slam/stereo/FeatureTracking.cpp
   vision/slam/stereo/KLTTracking.cpp
#  vision/slam/stereo/ORBTracking.cpp
   vision/slam/stereo/StereoSLAM.cpp
# vision/slam/stereo/ORBStereoInit.cpp
#   vision/slam/stereo/PatchStereoInit.cpp
   vision/TSDFVolume.cpp
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
      io/UEyeStereo.h
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
   IF( ${DC1394_FOUND} )
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
                "${${MODULE_NAME}_DEP_LIBRARIES}"
                #"${GENDEPS_${MODULE_NAME}}"
)

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

