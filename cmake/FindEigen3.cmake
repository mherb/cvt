#  EIGEN3_INCLUDE_DIR, where to find the Eigen folder
#  EIGEN3_FOUND, If false, do not try to use GLUT.

FIND_PATH( EIGEN3_INCLUDE_DIR Eigen/Core
		/usr/include/eigen3
		/usr/local/include/eigen3
		/opt/local/include/eigen3
	 )

INCLUDE( FindPackageHandleStandardArgs )
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args( EIGEN3  DEFAULT_MSG
								   EIGEN3_INCLUDE_DIR )


MARK_AS_ADVANCED(EIGEN3_INCLUDE_DIR)
