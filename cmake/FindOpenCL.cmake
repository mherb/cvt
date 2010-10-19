# - Find OpenCL
#  Afterwards, the following variables will be set
#
#  OPENCL_FOUND        - system has OpenCL installed
#  OPENCL_INCLUDE_DIR  - the OpenCL include directory
#  OPENCL_LIBRARIES    - link these to use OpenCL

IF (WIN32)
	FIND_PATH(OPENCL_INCLUDE_DIR 
		CL/cl.h 
	)

	# TODO this is only a hack assuming the 64 bit library will
	# not be found on 32 bit system
	FIND_LIBRARY(OPENCL_LIBRARIES 
		opencl64
	)
	
	IF( OPENCL_LIBRARIES )
		FIND_LIBRARY(OPENCL_LIBRARIES 
			opencl32 
		)
	ENDIF( OPENCL_LIBRARIES )

ELSE (WIN32)
	IF(APPLE)
		FIND_PATH(OPENCL_INCLUDE_DIR
			NAMES
			opencl.h
			PATHS
			/System/Library/Frameworks/OpenCL.framework/Headers
		)
		SET(OPENCL_LIBRARIES 
			"-framework OpenCL"
			CACHE STRING "Apple OpenCL Framework"
		)
	ELSE(APPLE)
		# Unix style platforms
		# We also search for OpenCL in the NVIDIA SDK default location
		FIND_PATH(OPENCL_INCLUDE_DIR
		   NAMES	
			CL/opencl.h 
		   PATHS
			/usr/local/cuda/include/
			~/NVIDIA_GPU_Computing_SDK/OpenCL/common/inc/ 
			/usr/local/nvgpu/OpenCL/common/inc/ 
			/usr/local/ati-stream-sdk/include/
		)
	
		FIND_LIBRARY(OPENCL_LIBRARIES 
			OpenCL 
			ENV LD_LIBRARY_PATH
			/usr/local/ati-stream-sdk/lib/x86_64
		)
	ENDIF (APPLE)
ENDIF (WIN32)

SET( OPENCL_FOUND "NO" )
IF(OPENCL_LIBRARIES )
	SET( OPENCL_FOUND "YES" )
ENDIF(OPENCL_LIBRARIES)

MARK_AS_ADVANCED(
	OPENCL_INCLUDE_DIR
	OPENCL_LIBRARIES
)
