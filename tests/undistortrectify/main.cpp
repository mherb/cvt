#include <iostream>

#include <cvt/io/Resources.h>
#include <cvt/io/xml/XMLDocument.h>
#include <cvt/vision/CameraCalibration.h>
#include <cvt/vision/Vision.h>

#include <cvt/gfx/ifilter/IWarp.h>
#include <cvt/gfx/ifilter/ITransform.h>


using namespace cvt;


void loadCameraCalib( CameraCalibration& camCalib, const String& file )
{
	XMLDocument xmlDoc;
	xmlDoc.load( file );

	XMLNode* node = xmlDoc.nodeByName( "CameraCalibration" );
	camCalib.deserialize( node );
}

int main( int argc, char* argv[] )
{
	if( argc != 3 ) {
		//std::cout << "usage: " << argv[ 0 ] << "calibration0 calibration1 image0 image1" << std::endl;
		std::cout << "usage: " << argv[ 0 ] << "image0 image1" << std::endl;
		return 1;
	}

	CameraCalibration calibration0, calibration1;
//	loadCameraCalib( calibration0, argv[ 1 ] );
//	loadCameraCalib( calibration1, argv[ 2 ] );

	Image tmp( argv[ 1 ] ), img0, img1;
	tmp.convert( img0, IFormat::RGBA_UINT8 );
	tmp.load( argv[ 2 ] );
	tmp.convert( img1, IFormat::RGBA_UINT8 );
	Image warp0, warp1, out, out2, out3;

	img0.save( "in0.png" );
	img1.save( "in1.png" );

	calibration0.setIntrinsics( Matrix3f(639.789626486223, 0, 373.708571344962, 0, 640.713611215267, 232.265724040287, 0, 0, 1));
	calibration1.setIntrinsics( Matrix3f(640.786137335592, 0, 379.943466306764, 0, 641.841431790401, 256.021219453508, 0, 0, 1));
	calibration0.setDistortion( Vector3f(-0.427950558945007, 0.183981146318444,0), Vector2f(0.000401750610214759, -0.000537801526079568) );
	calibration1.setDistortion( Vector3f(-0.425519082733471, 0.178269911004539,0), Vector2f(0.00183492038762636, 2.28397273935832e-05) );

	Matrix3f knew0, knew1, Timg0, Timg1;
	Matrix4f Tnew0, Tnew1;
	IWarp::warpUndistort( warp0, knew0, calibration0, img0.width(), img0.height() );
	calibration0.setIntrinsics( knew0 );
	IWarp::warpUndistort( warp1, knew1, calibration1, img1.width(), img1.height() );
	calibration1.setIntrinsics( knew1 );

	/* FIXME: do the rectification */
/*	Vision::stereoRectification( knew0, Tnew0, knew1, Tnew1, Timg0, Timg1, calibration0.intrinsics(), calibration0.extrinsics(), calibration1.intrinsics(), calibration1.extrinsics() );
	calibration0.setIntrinsics( knew0 );
	calibration0.setExtrinsics( Tnew0 );
	calibration1.setIntrinsics( knew1 );
	calibration1.setExtrinsics( Tnew1 );*/

	Timg0 = Matrix3f(0.99999557601144, -0.00148376900597234, 0.00257805878218369, 0.00149271341600604, 0.99999286201744, -0.00347098268889011, -0.00257289024351104, 0.00347481563623383, 0.99999065290236 );
	Timg1 = Matrix3f( 0.999954573908676, 0.00668518653448272, 0.00679399735919037, -0.00670874138003816, 0.999971544214094, 0.00345015234100927, -0.00677073911868406, -0.00349557478529341, 0.999970968602942 );

	
	Matrix3f Knew( 631.194799160303, 0, 373.159599304199, 0, 631.194799160303, 245.955177307129, 0, 0, 1 );

	IWarp::apply( out, img0, warp0 );
	out.save( "undistort0.png" );
	out2.reallocate( out.width(), out.height(), IFormat::RGBA_UINT8 );
	out2.fill( Color::BLACK );
	ITransform::apply( out2, out, Knew * Timg0 * calibration0.intrinsics().inverse() );
	out2.save( "undistortrectified0.png" );
	IWarp::apply( out, img1, warp1 );
	out.save( "undistort1.png" );
	out3.reallocate( out.width(), out.height(), IFormat::RGBA_UINT8 );
	out3.fill( Color::BLACK );
	ITransform::apply( out3, out, Knew * Timg1 * calibration1.intrinsics().inverse() );
	out3.save( "undistortrectified1.png" );

	out.reallocate( out2.width() + out3.width(), out2.height(), IFormat::RGBA_UINT8 );
	out.copyRect( 0, 0, out2, out2.rect() );
	out.copyRect( out2.width(), 0, out3, out3.rect() );
	out.save("undistortrectall.png");

	return 0;
}
