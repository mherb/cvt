#include <cvt/vision/Warp.h>
#include <cvt/vision/CamModel.h>
#include <cvt/util/CVTTest.h>

#include <limits>

namespace cvt{
	
	static bool testToScreen()
	{
		bool b = true;
		
		SE3<double> pose;
		Eigen::Matrix<double, 3, 1> p;
		Eigen::Matrix<double, 2, 1> sp;
		
		p[ 0 ] = 1.0; p[ 1 ] = 2.0; p[ 2 ] = 3.0;
		
		Eigen::Matrix<double, 3, 3> K = Eigen::Matrix<double, 3, 3>::Identity();	
		CamModel<double> cam( K, Eigen::Matrix<double, 4, 4>::Identity() );
		Warp::toScreen( sp, cam, pose, p );
		
		b &= ( sp[ 0 ] - p[ 0 ]/p[ 2 ] ) < std::numeric_limits<double>::min();
		b &= ( sp[ 1 ] - p[ 1 ]/p[ 2 ] ) < std::numeric_limits<double>::min();
		
		pose.set( 0, 0, 0, 0, 0, 100 );
		Warp::toScreen( sp, cam, pose, p );
		b &= ( sp[ 0 ] - p[ 0 ]/103.0 ) < std::numeric_limits<double>::min();
		b &= ( sp[ 1 ] - p[ 1 ]/103.0 ) < std::numeric_limits<double>::min();		
		
		return b;
	}
		
	BEGIN_CVTTEST( Warp )
		bool ret = true;
		bool b;
		b = testToScreen();
		CVTTEST_PRINT( "Screen Projection", b );
		ret &= b;
	
		return ret;	
	END_CVTTEST	
}