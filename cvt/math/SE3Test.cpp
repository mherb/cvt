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

	static bool testHessian()
	{
        Eigen::Matrix<double, 6, 1> delta = Eigen::Matrix<double, 6, 1>::Zero();        
		Eigen::Matrix<double, 24, 6> hN, hA;
        
		SE3<double> pose;
		pose.set( Math::deg2Rad( 10.0 ), Math::deg2Rad( 40.0 ), Math::deg2Rad( -120.0 ), -100.0, 200.0, 300.0 );

		Eigen::Matrix<double, 3, 3> K( Eigen::Matrix<double, 3, 3>::Zero() );
		K( 0, 0 ) = 650.0; K( 0, 2 ) = 320.0;
		K( 1, 1 ) = 650.0; K( 1, 2 ) = 240.0;
		K( 2, 2 ) = 1.0;
		pose.setIntrinsics( K );
        
		Eigen::Matrix<double, 3, 1> point;
		Eigen::Matrix<double, 3, 1> p, ff, fb, bf, bb, xxf, xxb, hess;
		point[ 0 ] = 16; 
		point[ 1 ] = 80; 
		point[ 2 ] = 13;
        
		pose.transform( p, point );

		double h = 0.0001;
		for( size_t i = 0; i < 6; i++ ){
            for( size_t j = 0; j < 6; j++ ){
				delta.setZero();
				if( i == j ){
					// +
					delta[ j ] = h;
					pose.apply( delta );
					pose.transform( xxf, point );
					pose.apply( -delta );

					delta[ j ] = -h;
					pose.apply( delta );
					pose.transform( xxb, point );
					pose.apply( -delta );

					hess = ( xxb - 2 * p + xxf ) / ( h*h );
				} else {
					delta[ i ] = h;
					delta[ j ] = h;
					pose.apply( delta );
					pose.transform( ff, point );
					pose.apply( -delta );

					delta[ i ] = h;
					delta[ j ] = -h;
					pose.apply( delta );
					pose.transform( fb, point );
					pose.apply( -delta );

					delta[ i ] = -h;
					delta[ j ] =  h;
					pose.apply( delta );
					pose.transform( bf, point );
					pose.apply( -delta );
					
					delta[ i ] = -h;
					delta[ j ] = -h;
					pose.apply( delta );
					pose.transform( bb, point );
					pose.apply( -delta );

					hess = ( ff - bf - fb + bb ) / ( 4 * h * h );
				}

                hN( 4 * i , j ) = hess[ 0 ];
                hN( 4 * i + 1 , j ) = hess[ 1 ];
                hN( 4 * i + 2 , j ) = hess[ 2 ];
                hN( 4 * i + 3 , j ) = 0.0;
            }
		}
		
		pose.hessian( hA, p );
        
		bool b, ret = true;
        Eigen::Matrix<double, 24, 6> jDiff;
        jDiff = hN - hA;
		b = ( jDiff.array().abs().sum() / ( 24.0 * 6.0 ) ) < 0.00001;
        
		CVTTEST_PRINT( "Pose Hessian", b );
		if( !b ){
			std::cout << "Analytic:\n" << hA << std::endl;
			std::cout << "Numeric:\n" << hN << std::endl;
			std::cout << "Difference:\n" << jDiff << std::endl;
		}
		ret &= b;
        
		return ret;
	}

	static void projectWithCam( Eigen::Matrix<double, 2, 1> & sp,
								const Eigen::Matrix<double, 3, 1> & p3d,
								const Eigen::Matrix<double, 3, 3> & K )
	{
		sp[ 0 ] = K( 0, 0 ) * p3d.x() / p3d.z() + K( 0, 2 );
		sp[ 1 ] = K( 1, 1 ) * p3d.y() / p3d.z() + K( 1, 2 );
	}

    static bool testScreenHessian()
    {        
        Eigen::Matrix<double, 6, 1> delta = Eigen::Matrix<double, 6, 1>::Zero();        
		Eigen::Matrix<double, 6, 6> shNumericX, shNumericY, shX, shY;
        
        
		SE3<double> pose;
		pose.set( Math::deg2Rad( 10.0 ), Math::deg2Rad( 40.0 ), Math::deg2Rad( -120.0 ), -100.0, 200.0, 300.0 );

		Eigen::Matrix<double, 3, 3> K( Eigen::Matrix<double, 3, 3>::Zero() );
		K( 0, 0 ) = 650.0; K( 0, 2 ) = 320.0;
		K( 1, 1 ) = 650.0; K( 1, 2 ) = 240.0;
		K( 2, 2 ) = 1.0;
		pose.setIntrinsics( K );

		Eigen::Matrix<double, 3, 1> point, ptrans;
		Eigen::Matrix<double, 2, 1> sp, ff, fb, bf, bb, xxf, xxb, hess;
		point[ 0 ] = 100; point[ 1 ] = 200; point[ 2 ] = 300;
        
        // project the point with current parameters
		pose.transform( ptrans, point );
		projectWithCam( sp, ptrans, K );

		double h = 0.001;
		for( size_t i = 0; i < 6; i++ ){
            for( size_t j = 0; j < 6; j++ ){

				if( i == j ){
					// +
					delta[ j ] = h;
					pose.apply( delta );
					pose.transform( ptrans, point );
					projectWithCam( xxf, ptrans, K );
					delta[ j ] = -2 * h;
					pose.apply( delta );
					pose.transform( ptrans, point );
					projectWithCam( xxb, ptrans, K );

					hess = ( xxb - 2 * sp + xxf ) / ( h*h );
					
					// back to start
					delta[ j ] = h;
					pose.apply( delta );
					delta[ j ] = 0;
				} else {
					delta[ i ] = h;
					delta[ j ] = h;
					pose.apply( delta );
					pose.transform( ptrans, point );
					projectWithCam( ff, ptrans, K );
					pose.apply( -delta );

					delta[ i ] = h;
					delta[ j ] = -h;
					pose.apply( delta );
					pose.transform( ptrans, point );
					projectWithCam( fb, ptrans, K );
					pose.apply( -delta );

					delta[ i ] = -h;
					delta[ j ] =  h;
					pose.apply( delta );
					pose.transform( ptrans, point );
					projectWithCam( bf, ptrans, K );
					pose.apply( -delta );
					
					delta[ i ] = -h;
					delta[ j ] = -h;
					pose.apply( delta );
					pose.transform( ptrans, point );
					projectWithCam( bb, ptrans, K );
					pose.apply( -delta );

					hess = ( ff - bf - fb + bb ) / ( 4 * h * h );
					delta.setZero();
				}

                shNumericX( i, j ) = hess[ 0 ];
                shNumericY( i, j ) = hess[ 1 ];
                
            }
		}
        
		pose.transform( ptrans, point );
		pose.screenHessian( shX, shY, ptrans );
        
		bool b, ret = true;
        Eigen::Matrix<double, 6, 6> jDiff;
        jDiff = shNumericX - shX;
		b = ( jDiff.array().abs().sum() / 36.0 ) < 0.001;
        
		CVTTEST_PRINT( "Pose ScreenHessian X", b );
		if( !b ){
			std::cout << "Analytic:\n" << shX << std::endl;
			std::cout << "Numeric:\n" << shNumericX << std::endl;
			std::cout << "Difference:\n" << jDiff << std::endl;
		}
		ret &= b;
        
        jDiff = shNumericY - shY;
		b = ( jDiff.array().abs().sum() / 36.0 ) < 0.001;
        
		CVTTEST_PRINT( "Pose ScreenHessian Y", b );
		if( !b ){
			std::cout << "Analytic:\n" << shY << std::endl;
			std::cout << "Numeric:\n" << shNumericY << std::endl;
			std::cout << "Difference:\n" << jDiff << std::endl;
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
	ret &= testHessian();
	ret &= testScreenHessian();

	return ret;
END_CVTTEST

}
