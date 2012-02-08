# first find all dependencies
# this cannot be checked vie system dependencies!
FIND_PACKAGE(OpenCL REQUIRED)

FIND_PATH( CVT_INCLUDE_DIR 
	NAMES cvt/geom/Model.h
	PATHS
	${CMAKE_INCLUDE_PATH}
	/usr/include
	/usr/local/include
)

FIND_LIBRARY( CVT_LIBRARY
	NAMES cvt
	PATHS
	${CMAKE_LIBRARY_PATH}
	/usr/lib
	/usr/local/lib
)

IF( CVT_LIBRARY )
	SET( CVT_FOUND TRUE )
ENDIF ( CVT_LIBRARY )

SET( CVT_LIBRARIES "${CVT_LIBRARY}" )

MARK_AS_ADVANCED(
	CVT_INCLUDE_DIR
	CVT_LIBRARY
	CVT_LIBRARIES
)
