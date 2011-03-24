#  EIGEN3_INCLUDE_DIR, where to find the Eigen folder
#  EIGEN3_FOUND, If false, do not try to use GLUT.

FIND_PATH( EIGEN3_INCLUDE_DIR Eigen/Core
		/usr/include/eigen3
		/usr/local/include/eigen3
		/opt/local/include/eigen3
	 )

SET( EIGEN3_FOUND "NO" )
IF(EIGEN3_INCLUDE_DIR)
    SET( EIGEN3_FOUND "YES" )
ENDIF(EIGEN3_INCLUDE_DIR)

MARK_AS_ADVANCED(EIGEN3_INCLUDE_DIR)
