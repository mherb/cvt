#include <iostream>

#include <cvt/io/Resources.h>
#include <cvt/io/xml/XMLDocument.h>
#include <cvt/vision/CameraCalibration.h>
#include <cvt/vision/Vision.h>

#include <cvt/gfx/ifilter/IWarp.h>


using namespace cvt;

int main( int argc, char* argv[] )
{
	Resources r;

	String calib0 = r.find( "calib_stereo/ueye_4002738788.xml" );

	CameraCalibration camCalib0;
	camCalib0.load( calib0 );

	Image i0( r.find( "calib_stereo/ueye_4002738788.png" ) );
	Image out( i0 ), warp( i0.width(), i0.height(), IFormat::GRAYALPHA_FLOAT );

	Matrix3f knew;
	IWarp::warpUndistort( warp, knew, camCalib0, i0.width(), i0.height() );
	IWarp::apply( out, i0, warp );

	out.save( "undistort.png" );

	return 0;
}
