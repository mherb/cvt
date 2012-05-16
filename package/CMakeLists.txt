SET( ADDITIONAL_FILES
	 "${PROJECT_SOURCE_DIR}/cmake/FindOpenCL.cmake" 
	 "${PROJECT_SOURCE_DIR}/cmake/FinduEyeUsbCamera.cmake" 
	 "${PROJECT_SOURCE_DIR}/cmake/FindOpenNI.cmake" 
	 "${PROJECT_SOURCE_DIR}/cmake/FindEigen3.cmake" 
	 "${PROJECT_SOURCE_DIR}/cmake/shipped/GLCLTOHEADER.cmake" 
	 "${PROJECT_SOURCE_DIR}/cmake/shipped/MacroCVTApplication.cmake" 
	 "${PROJECT_SOURCE_DIR}/cmake/shipped/FindCVT.cmake" 
)

INSTALL( FILES ${ADDITIONAL_FILES}
		 DESTINATION "share/cvt/cmake"
		 COMPONENT CMakeFiles )

INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Computer Vision Tools Library (CVT)")
SET(CPACK_PACKAGE_NAME "libcvt")
SET(CPACK_PACKAGE_VENDOR "Philipp Heise and Sebastian Klose")
SET(CPACK_PACKAGE_CONTACT "Sebastian Klose, kloses@in.tum.de")

# deb specific
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sebastian Klose, kloses@in.tum.de")

# dependencies: Eigen, ffmpeg
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libpng12-dev (>=1.2.43), libeigen3-dev (>=3.0.1), libdc1394-22-dev, libavformat-dev (>=4:0.6), libavcodec-dev (>=4:0.6)")

SET(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/Copyright.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
set(CPACK_COMPONENTS_ALL Headers Plugins CMakeFiles cvt utils)

#SET(CPACK_STRIP_FILES "bin/MyExecutable")
#SET(CPACK_SOURCE_STRIP_FILES "")
INCLUDE(CPack)
