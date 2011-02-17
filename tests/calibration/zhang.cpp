#include <cvt/vision/HCalibration.h>

#include <Eigen/Geometry>

using namespace cvt;

int main()
{
	HCalibration calib;

	Eigen::Matrix<double,3,3> K;
	Eigen::Matrix3d R;	Eigen::Vector3d t;
	Eigen::Matrix3d H;

	srand( time( NULL ) );

	K.setZero();
	K( 0, 0 ) = 650.0f + Math::rand( -10.0f, 10.0f );
	K( 0, 1 ) = 0.0f + Math::rand( -0.5f, 0.5f );
	K( 1, 1 ) = 650.0f + Math::rand( -10.0f, 10.0f );
	K( 0, 2 ) = 400.0f + Math::rand( -10.0f, 10.0f );
	K( 1, 2 ) = 300.0f + Math::rand( -10.0f, 10.0f );
	K( 2, 2 ) = 1.0f;

#define NUMHOM 10
	for( size_t i = 0; i < NUMHOM; i++ ) {
		R = ( Eigen::AngleAxisd( Math::deg2Rad( Math::rand( -20.0f, 20.0f ) ), Eigen::Vector3d::UnitZ() ) *
						  Eigen::AngleAxisd( Math::deg2Rad( Math::rand( -20.0f, 20.0f ) ), Eigen::Vector3d::UnitY() ) *
						  Eigen::AngleAxisd( Math::deg2Rad( Math::rand( -20.0f, 20.0f ) ), Eigen::Vector3d::UnitX() ) ).toRotationMatrix();
		t( 0 ) = Math::rand( -200.0f, 200.0f );
		t( 1 ) = Math::rand( -200.0f, 200.0f );
		t( 2 ) = Math::rand( -200.0f, 200.0f );

		H.setZero();
		H.col( 0 ) = K * R.col( 0 );
		H.col( 1 ) = K * R.col( 1 );
		H.col( 2 ) = K * t;
		calib.addHomography( H );
	}
	calib.calibration( H );
	std::cout << K << std::endl;
	std::cout << H << std::endl;
}
