PROJECT(CVTools CXX C)

SET( CVT_VERSION_MAJOR 0 )
SET( CVT_VERSION_MINOR 1 )
SET( CVT_VERSION_PATCH 1 )

CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
cmake_policy(SET CMP0012 NEW)

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
INCLUDE(OutOfSourceBuild)
INCLUDE(BuildTypes)
INCLUDE(CompilerFlags)
INCLUDE(CompileDefinitions)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# tool to generate header files out of shader or cl sources
add_executable( glcltoheader util/glcltoheader.cpp )
INSTALL( TARGETS "glcltoheader" RUNTIME DESTINATION "share/cvt/utils" COMPONENT utils )

ADD_SUBDIRECTORY(cvt)
ADD_SUBDIRECTORY(plugins)

OPTION( CVT_BUILD_TESTS "Build the test apps or not" TRUE )
IF( CVT_BUILD_TESTS )
	ADD_SUBDIRECTORY(tests)
ENDIF()

IF( ${CMAKE_GENERATOR} MATCHES "Xcode" )
	SET_TARGET_PROPERTIES(glcltoheader PROPERTIES PREFIX "../")	
ENDIF()

ADD_SUBDIRECTORY( package )
