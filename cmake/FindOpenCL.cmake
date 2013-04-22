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
	FIND_LIBRARY(OPENCL_LIBRARY 
		opencl64
	)
	
	IF( OPENCL_LIBRARY )
		FIND_LIBRARY(OPENCL_LIBRARY 
			opencl32 
		)
	ENDIF( OPENCL_LIBRARY )

ELSE (WIN32)
	IF(APPLE)
		FIND_PATH(OPENCL_INCLUDE_DIR
			NAMES
			opencl.h
			PATHS
			/System/Library/Frameworks/OpenCL.framework/Headers
		)
		SET(OPENCL_LIBRARY 
			"-framework OpenCL"
			CACHE STRING "Apple OpenCL Framework"
		)
	ELSE(APPLE)
		# Unix style platforms
		# NVIDIA SDK default location
		FIND_PATH(OPENCL_INCLUDE_DIR
		   NAMES	
			CL/opencl.h 
		   PATHS
			/usr/local/cuda/include/
			~/NVIDIA_GPU_Computing_SDK/OpenCL/common/inc/ 
			/usr/local/nvgpu/OpenCL/common/inc/ 
			/usr/local/ati-stream-sdk/include/
			/opt/AMDAPP/include/
			$ENV{ATISTREAMSDKROOT}/include
		)
	
		# AMD Stream SDK, (should be updated to APPSDK) default location
		FIND_LIBRARY(OPENCL_LIBRARIES 
			OpenCL 
			ENV LD_LIBRARY_PATH
			/usr/local/ati-stream-sdk/lib/x86_64
			/usr/local/ati-stream-sdk/lib/x86
		)
	ENDIF (APPLE)
ENDIF (WIN32)

INCLUDE( LibFindMacros )

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set( OPENCL_PROCESS_INCLUDES OPENCL_INCLUDE_DIR OPENCL_INCLUDE_DIRS)
set( OPENCL_PROCESS_LIBS OPENCL_LIBRARY )
libfind_process( OPENCL )
