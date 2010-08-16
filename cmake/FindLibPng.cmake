#  LIBPNG_INCLUDE_DIR, where to find the libpng  folder
#  LIBPNG_FOUND, If false, do not try to use libpng.

FIND_PATH( LIBPNG_INCLUDE_DIR png.h
	      /usr/include/eigen2
	      /usr/local/include/eigen2
	      /opt/local/include/eigen2
	      ${CMAKE_INCLUDE_PATH}
	      $ENV{HOME}/include/
	      /usr/include/libpng/
	     )

FIND_LIBRARY(LIBPNG_LIBRARY
		 NAMES
		 libpng.so
		 PATHS
		 ${CMAKE_LIBRARY_PATH}
		 /usr/lib
		)

IF(LIBPNG_INCLUDE_DIR)
    IF(LIBPNG_LIBRARY)
	SET( LIBPNG_FOUND "YES" )
    ENDIF(LIBPNG_LIBRARY)
ENDIF(LIBPNG_INCLUDE_DIR )

MARK_AS_ADVANCED( LIBPNG_FOUND
		  LIBPNG_LIBRARY
		  LIBPNG_INCLUDE_DIR )
