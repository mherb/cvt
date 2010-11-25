# On success this script defines
# UEYEUSB_INCLUDE_DIR           location of ueye.h
# UEYEUSB_LIBRARY               location of libueye_api library
# UEYEUSB_FOUND					whether the library has been found or not 

FIND_PATH(UEYEUSB_INCLUDE_DIR  ueye.h
	${CMAKE_INCLUDE_PATH}
	$ENV{HOME}/include/
	/usr/include/opencv
	$ENV{ProgramFiles}/IDS/ueye/Develop/include
)

FIND_LIBRARY(UEYEUSB_LIBRARY
	NAMES
	libueye_api.so
	PATHS
	${CMAKE_LIBRARY_PATH}
	/usr/lib
	$ENV{ProgramFiles}/IDS/ueye/Develop/lib
)

IF (UEYEUSB_LIBRARY)
	SET(UEYEUSB_LIBRARY_FOUND TRUE)
ENDIF (UEYEUSB_LIBRARY)


SET(
	UEYEUSB_INCLUDES
	${UEYEUSB_INCLUDE_DIR}
)

SET(
	UEYEUSB_LIBRARIES
	${UEYEUSB_LIBRARY}
)

IF (UEYEUSB_INCLUDE_DIR)
	SET(UEYEUSB_FOUND TRUE)
	ADD_DEFINITIONS( -DUEYE_FOUND )
ENDIF (UEYEUSB_INCLUDE_DIR)

MARK_AS_ADVANCED(
	UEYEUSB_FOUND
	UEYEUSB_INCLUDES
	UEYEUSB_LIBRARIES

) 

