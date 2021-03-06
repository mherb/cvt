
IF( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
    SET( CVT_COMPATIBLE_COMPILER YES )
endif()

IF( CVT_COMPATIBLE_COMPILER )
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
#	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weffc++")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual -Wnon-virtual-dtor -Wsign-promo")
	
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpmath=sse")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE} -msse")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE} -msse2")

# RELEASE:
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS}")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fomit-frame-pointer")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mssse3")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -funroll-loops")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fvisibility-inlines-hidden")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D NDEBUG")
	SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -D NDEBUG")

# DEBUG:
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS}")
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O3")
	#SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0") # enable to disable optimization
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
	#SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pg") # Enable this for profiling with gprof
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D DEBUG")
	SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D DEBUG")

# ZEROOPT
	SET(CMAKE_CXX_FLAGS_ZEROOPT "${CMAKE_CXX_FLAGS}")
	SET(CMAKE_CXX_FLAGS_ZEROOPT "${CMAKE_CXX_FLAGS_ZEROOPT} -O0")
	SET(CMAKE_CXX_FLAGS_ZEROOPT "${CMAKE_CXX_FLAGS_ZEROOPT} -g")
	SET(CMAKE_CXX_FLAGS_ZEROOPT "${CMAKE_CXX_FLAGS_ZEROOPT} -D DEBUG")
	SET(CMAKE_C_FLAGS_ZEROOPT "${CMAKE_C_FLAGS_ZEROOPT} -D DEBUG")
	
# ATOM
	SET(CMAKE_CXX_FLAGS_ATOM "${CMAKE_CXX_FLAGS}")
	SET(CMAKE_CXX_FLAGS_ATOM "${CMAKE_CXX_FLAGS_ATOM} -O3")
	SET(CMAKE_CXX_FLAGS_ATOM "${CMAKE_CXX_FLAGS_ATOM} -g")
	SET(CMAKE_CXX_FLAGS_ATOM "${CMAKE_CXX_FLAGS_ATOM} -m32")
	SET(CMAKE_CXX_FLAGS_ATOM "${CMAKE_CXX_FLAGS_ATOM} -march=core2")
	SET(CMAKE_CXX_FLAGS_ATOM "${CMAKE_CXX_FLAGS_ATOM} -msse3")
	SET(CMAKE_CXX_FLAGS_ATOM "${CMAKE_CXX_FLAGS_ATOM} -mssse3")
	SET(CMAKE_CXX_FLAGS_ATOM "${CMAKE_CXX_FLAGS_ATOM} -funroll-loops")
#	SET(CMAKE_LXX_FLAGS_ATOM "${CMAKE_LXX_FLAGS} -m32")
#	SET(CMAKE_EXE_LINKER_FLAGS_ATOM "${CMAKE_EXE_LINKER_FLAGS} -m32")	
#	SET(CMAKE_SHARED_LINKER_FLAGS_ATOM "${CMAKE_SHARED_LINKER_FLAGS} -m32")
#	SET(CMAKE_MODULE_LINKER_FLAGS_ATOM "${CMAKE_MODULE_LINKER_FLAGS} -m32")

	# ADD DEBUG FLAGS FOR EACH BUILD TYPE (IT CANNOT BE SET BY ADD_DEFINITIONS (BECAUSE OF MSVS))
	SET(CMAKE_CXX_FLAGS_ATOM "${CMAKE_CXX_FLAGS_ATOM} -D DEBUG")
	SET(CMAKE_C_FLAGS_ATOM   "${CMAKE_C_FLAGS_ATOM} -D DEBUG")
ELSE()
    MESSAGE( FATAL_ERROR "Unknown Compiler!" )
ENDIF()
