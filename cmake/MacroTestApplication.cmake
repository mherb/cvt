# (C) 2010, Sebastian Klose, <kloses@in.tum.de>

#	this automatically generates a target executable, using all 
#	.cpp, .c, .hpp and .h files of the current directory
#	The name of the target will be either the one passed as argument,
#	or (if none is passed) the name of the folder
#
#	Dependencies we search for and link against:
#	- Boost
#	- Eigen2

MACRO (MACRO_TEST_APPLICATION)

#FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(Eigen2 REQUIRED)
FIND_PACKAGE(Boost 1.40 REQUIRED)
FIND_PACKAGE(OpenCL REQUIRED)

include(GLCLTOHEADER)

SET(DATA_FOLDER	"${CMAKE_SOURCE_DIR}/data")
ADD_DEFINITIONS(-DDATA_FOLDER="${DATA_FOLDER}")

IF(${ARGC} GREATER 0)
	SET(APP_NAME "${ARGV0}")
ELSE(${ARGC} GREATER 0)
	GET_FILENAME_COMPONENT(folderName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
	SET(APP_NAME "${folderName}")
ENDIF(${ARGC} GREATER 0)

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
#	"${OpenCV_INCLUDE_DIRS}"
	"${Eigen2_INCLUDE_DIR}"
	"${Boost_INCLUDE_DIRS}"
	"${OPENCL_INCLUDE_DIR}"
)

TARGET_LINK_LIBRARIES(
	${APP_NAME}
	cvt	
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
