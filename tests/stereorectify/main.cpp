#include <iostream>

#include <cvt/io/Resources.h>
#include <cvt/io/xml/XMLDocument.h>
#include <cvt/vision/StereoCameraCalibration.h>
#include <cvt/vision/Vision.h>

#include <cvt/gfx/ifilter/IWarp.h>
#include <cvt/util/Delegate.h>
#include <cvt/math/Quaternion.h>

using namespace cvt;
void loadCameraCalib( CameraCalibration& camCalib, const String& file )
{
	XMLDocument xmlDoc;
	xmlDoc.load( file );
	
	XMLNode* node = xmlDoc.nodeByName( "CameraCalibration" );
	camCalib.deserialize( node );
}

struct UndistortOp
{
	UndistortOp( const CameraCalibration& calib ) : _calib( calib ) {}

	inline Vector2f operator()( const Vector2f& pt ) const { return _calib.undistortPoint( pt ); }

	CameraCalibration _calib;
};

int main( int argc, char** argv )
{
	Matrix3f mat(   1.0000,  -0.0072,  -0.0059,
				   0.0071,   0.9999,  -0.0151,
				   0.0060,   0.0151,   0.9999 );
	Vector3f aa( 0.0151079052 ,  -0.0059286737,  0.0071693550 );



	std::cout << mat<< std::endl << std::endl;
	mat.setRotation( aa / aa.length(), aa.length() );

	std::cout << mat<< std::endl << std::endl;
	Quaternionf q( mat );
	std::cout << q.toMatrix3() << std::endl << std::endl;

	Vector3f axis;
	float angle;
	q.toAxisAngle( axis, angle );
	std::cout << axis << std::endl;
	std::cout << angle << std::endl << std::endl;;

	mat.toAxisAngle( axis, angle );
	std::cout << axis << std::endl;
	std::cout << angle << std::endl;
	mat.setRotation( axis, angle * 0.5 );
	std::cout << mat * mat << std::endl;
	return 0;
	// FIXME: add argc/argv check
	CameraCalibration left;
	CameraCalibration right;

	StereoCameraCalibration( left, right );

	loadCameraCalib( left, argv[ 1 ] );
	loadCameraCalib( right, argv[ 2 ] );

	Image input( argv[ 3 ] );
	Image warp( input.width(), input.height(), IFormat::GRAYALPHA_FLOAT );
	Image out( input );

	UndistortOp op( left );
	IWarp::warpGeneric( warp, op );
//	IWarp::warpGeneric( warp, Delegate<Vector2f ( const Vector2f& )>( &left, &CameraCalibration::undistortPoint ) );
	IWarp::apply( out, input, warp );
	out.save("output.png");
}
