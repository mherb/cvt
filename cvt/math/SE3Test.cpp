#include <cvt/math/SE3.h>
#include <cvt/util/CVTTest.h>
#include <cvt/math/Math.h>

#include <Eigen/Array>

namespace cvt {
	
BEGIN_CVTTEST( SE3 )
	bool ret = true;

	SE3<double> pose;
	bool b = true;
	b = ( pose.transformation() == Eigen::Matrix<double, 4, 4>::Identity() );
	CVTTEST_PRINT( "Initialization", b );
	ret &= b;

	pose.set( 0, 0, 0, 100, 200, 300 );
	b = ( pose.transformation()( 0, 3 ) == 100 ) &&
		( pose.transformation()( 1, 3 ) == 200 ) &&
		( pose.transformation()( 2, 3 ) == 300 ) &&
		( pose.transformation()( 3, 3 ) ==   1 ) &&
		( pose.transformation()( 2, 2 ) ==   1 ) &&
		( pose.transformation()( 1, 1 ) ==   1 ) &&
		( pose.transformation()( 0, 0 ) ==   1 );
	
	pose.set( Math::deg2Rad( 90 ), 0, 0, 0, 0, 0 );
	b =  ( pose.transformation()( 0, 3 ) ==   0 );
	b &= ( pose.transformation()( 1, 3 ) ==   0 );
	b &= ( pose.transformation()( 2, 3 ) ==   0 );
	b &= ( pose.transformation()( 3, 3 ) ==   1 );
	b &= ( pose.transformation()( 0, 0 ) ==   1 );
	b &= ( pose.transformation()( 0, 1 ) ==   0 );
	b &= ( pose.transformation()( 0, 2 ) ==   0 );
	b &= ( pose.transformation()( 1, 0 ) ==   0 );
	b &= ( pose.transformation()( 2, 0 ) ==   0 );
	b &= ( pose.transformation()( 1, 2 ) + 1 < 0.0000001 );
	b &= ( pose.transformation()( 2, 1 ) - 1 < 0.0000001 );
	b &= ( Math::abs( pose.transformation()( 2, 2 ) ) <  0.0000001 );
	b &= ( Math::abs( pose.transformation()( 1, 1 ) ) <  0.0000001 );	
	CVTTEST_PRINT( "Set", b );
	ret &= b;
		
	// apply delta:
	Eigen::Matrix<double, 6, 1> delta = Eigen::Matrix<double, 6, 1>::Zero();
	Eigen::Matrix<double, 4, 4> expectedT = Eigen::Matrix<double, 4, 4>::Identity();
	Eigen::Matrix<double, 4, 4> diff;

	pose.set( delta );
	delta[ 0 ] = Math::deg2Rad( 1.5 ); 
	delta[ 1 ] = Math::deg2Rad( 1.1 ); 
	delta[ 2 ] = Math::deg2Rad( 1.6 );
	delta[ 3 ] = 1; 
	delta[ 4 ] = 1; 
	delta[ 5 ] = 1;
	pose.apply( delta );
	
	expectedT( 0, 3 ) = delta[ 3 ]; 
	expectedT( 1, 3 ) = delta[ 4 ]; 
	expectedT( 2, 3 ) = delta[ 5 ];
	
	Eigen::Matrix<double, 3, 1> axis = delta.segment<3>( 0 );
	double angle = axis.norm();	axis /= angle;

	expectedT.block<3, 3>( 0, 0 ) = Eigen::AngleAxis<double>( angle, axis ).toRotationMatrix();	
	diff = expectedT - pose.transformation();
	
	b = ( diff.cwise().abs().sum() / 12 < 0.01 );
	
	if( !b ){
		std::cout << expectedT << std::endl;
		std::cout << pose.transformation() << std::endl;
	}
	
	pose.apply( -delta );
	expectedT.setIdentity();
	
	b &= ( ( expectedT - pose.transformation() ).cwise().abs().sum() / 12 < 0.01 );
	
	CVTTEST_PRINT( "apply", b );
	ret &= b;
		
	return ret;
END_CVTTEST
	
}
