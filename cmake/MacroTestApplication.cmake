# (C) 2010, Sebastian Klose, <kloses@in.tum.de>

#	this automatically generates a target executable, using all 
#	.cpp, .c, .hpp and .h files of the current directory
#	The name of the target will be either the one passed as argument,
#	or (if none is passed) the name of the folder
#
#	Dependencies we search for and link against:
#	- Eigen3
#	- OpenCL

MACRO ( MACRO_TEST_APPLICATION ADDITIONAL_LIBS )

FIND_PACKAGE( Eigen3 REQUIRED )
FIND_PACKAGE( OpenCL REQUIRED )
FIND_PACKAGE( OpenGL REQUIRED )
IF( APPLE )
	SET( OPENGL_gl_LIBRARY "/usr/X11/lib/libGL.dylib" )
	SET( OPENGL_glu_LIBRARY "/usr/X11/lib/libGLU.dylib" )
	SET( OPENGL_INCLUDE_DIR "/usr/X11/include/GL" )
	SET( X11_X11_LIB "/usr/X11/lib/libX11.dylib")
	SET( OPENGL_LIBRARIES ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} )
	INCLUDE_DIRECTORIES( /usr/X11/include )
	LINK_DIRECTORIES(/usr/X11/lib)
ENDIF( APPLE )

include( GLCLTOHEADER )

SET(DATA_FOLDER	"${CMAKE_SOURCE_DIR}/data")
ADD_DEFINITIONS(-DDATA_FOLDER="${DATA_FOLDER}")

GET_FILENAME_COMPONENT(folderName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
SET(APP_NAME "${folderName}")

MESSAGE(STATUS "Configuring: ${APP_NAME}") 

# get all headers and cpp files in the folder:
FILE(GLOB SRCS_${APP_NAME} "*.cpp" "*.h" "*.hpp" "*.mm")
FILE(GLOB CLSRCS_${APP_NAME} "*.cl")

FOREACH (CLSOURCE ${CLSRCS_${APP_NAME}})
	GET_FILENAME_COMPONENT( CLFILE ${CLSOURCE} NAME )
	CLTOHEADER(${CLFILE})
	GET_FILENAME_COMPONENT( CLFILE ${CLSOURCE} NAME_WE )
	SET(CLHEADER_${APP_NAME} ${CLHEADER_${APP_NAME}} "${CMAKE_CURRENT_SOURCE_DIR}/${CLFILE}.h" )
ENDFOREACH(CLSOURCE)

ADD_EXECUTABLE(${APP_NAME} ${SRCS_${APP_NAME}} ${CLHEADER_${APP_NAME}} )

INCLUDE_DIRECTORIES( 
	"${CMAKE_SOURCE_DIR}"
	"${CMAKE_SOURCE_DIR}/cvt"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${EIGEN3_INCLUDE_DIR}"
	"${OPENCL_INCLUDE_DIR}"
	"${OPENCL_INCLUDE_DIR}"	
)

TARGET_LINK_LIBRARIES(
	${APP_NAME}
	cvt
	${ADDITIONAL_LIBS}
)

add_dependencies( ${APP_NAME} cvt ${CMAKE_CURRENT_SOURCE_DIR} )

SET_TARGET_PROPERTIES("${APP_NAME}" PROPERTIES DEBUG_POSTFIX "_d")

IF( ${CMAKE_GENERATOR} MATCHES "Xcode" )
	SET_TARGET_PROPERTIES("${APP_NAME}" PROPERTIES PREFIX "../")
ENDIF()

INSTALL(TARGETS "${APP_NAME}"
	RUNTIME DESTINATION bin
	COMPONENT Tests
)

ENDMACRO (MACRO_TEST_APPLICATION)
