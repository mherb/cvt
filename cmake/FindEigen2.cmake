#  EIGEN2_INCLUDE_DIR, where to find the Eigen folder
#  EIGEN2_FOUND, If false, do not try to use GLUT.

FIND_PATH( Eigen2_INCLUDE_DIR Eigen/Core
		/usr/include/eigen2
		/usr/local/include/eigen2
		/opt/local/include/eigen2
	 )

SET( EIGEN2_FOUND "NO" )
IF(Eigen2_INCLUDE_DIR)
    SET( EIGEN2_FOUND "YES" )
ENDIF(Eigen2_INCLUDE_DIR)

MARK_AS_ADVANCED(Eigen2_INCLUDE_DIR)
