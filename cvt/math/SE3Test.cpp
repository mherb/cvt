#include <cvt/math/SE3.h>
#include <cvt/math/Math.h>
#include <cvt/vision/CamModel.h>
#include <cvt/util/CVTTest.h>

#include <Eigen/Core>

namespace cvt {

	static bool testJacobian()
	{
		Eigen::Matrix<double, 6, 1> parameter = Eigen::Matrix<double, 6, 1>::Zero();
		Eigen::Matrix<double, 6, 1> delta = Eigen::Matrix<double, 6, 1>::Zero();
		Eigen::Matrix<double, 3, 6> jNumeric, jAnalytic, jDiff;
		Eigen::Matrix<double, 2, 6> sJN, sJA, sJDiff;

		parameter[ 0 ] = Math::deg2Rad(  10 );
		parameter[ 1 ] = Math::deg2Rad( -10 );
		parameter[ 2 ] = Math::deg2Rad(  30 );
		parameter[ 3 ] = 40;
		parameter[ 4 ] = 50;
		parameter[ 5 ] = 60;

		SE3<double> pose;
		pose.set( parameter );

		Eigen::Matrix<double, 3, 1> point;
		Eigen::Matrix<double, 3, 1> p, pp;
		Eigen::Matrix<double, 2, 1> sp, spp;
		point[ 0 ] = 13; point[ 1 ] = 8; point[ 2 ] = 12;;

		pose.transform( p, point );

		Eigen::Matrix<double, 3, 3> K = Eigen::Matrix<double, 3, 3>::Identity();
		K( 0, 0 ) = K( 1, 1 ) = 600;
		K( 0, 2 ) = 320; K( 1, 2 ) = 240;
		CamModel<double> cam( K );
		pose.project( sp, cam, point );

		double h = 0.000000001;
		for( size_t i = 0; i < 6; i++ ){
			delta[ i ] = h;

			pose.apply( delta );

			pose.transform( pp, point );
			jNumeric.col( i ) = ( pp - p ) / h;

			pose.project( spp, cam, point );
			sJN.col( i ) = ( spp - sp ) / h;

			delta[ i ] = 0;
			pose.set( parameter );
		}

		pose.jacobian( jAnalytic, point );
		pose.project( spp, sJA, cam, point );

		bool b, ret = true;
		jDiff = jAnalytic - jNumeric;

		b = ( jDiff.array().abs().sum() / 18.0 ) < 0.00001;

		CVTTEST_PRINT( "Pose Jacobian", b );
		if( !b ){
			std::cout << "Analytic:\n" << jAnalytic << std::endl;
			std::cout << "Numeric:\n" << jNumeric << std::endl;
			std::cout << "Difference:\n" << jDiff << std::endl;
		}
		ret &= b;

		sJDiff = sJA - sJN;
		b = ( sJDiff.array().abs().sum() / 12.0 ) < 0.0001;
		CVTTEST_PRINT( "Screen Jacobian", b );
		if( !b ){
			std::cout << "Analytic:\n" << sJA << std::endl;
			std::cout << "Numeric:\n" << sJN << std::endl;
			std::cout << "Difference:\n" << sJDiff << std::endl;
			std::cout << "Avg. Diff: " << sJDiff.array().abs().sum() / 12.0 << std::endl;
		}
		ret &= b;

		return ret;
	}

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

	pose.set( Math::deg2Rad( 90.0 ), 0, 0, 0, 0, 0 );
	b &= ( pose.transformation()( 0, 3 ) ==   0 );
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

	b = ( diff.array().abs().sum() / 12 < 0.001 );

	if( !b ){
		std::cout << expectedT << std::endl;
		std::cout << pose.transformation() << std::endl;
		std::cout << "avg SAD: " << diff.array().abs().sum() / 12 << std::endl;
	}

	pose.apply( -delta );
	expectedT.setIdentity();

	b &= ( ( expectedT - pose.transformation() ).array().abs().sum() / 12 < 0.000001 );
	CVTTEST_PRINT( "apply", b );
	ret &= b;

	ret &= testJacobian();

	return ret;
END_CVTTEST

}
