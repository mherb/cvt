#include <iostream>

#include <cvt/io/Resources.h>
#include <cvt/io/xml/XMLDocument.h>
#include <cvt/vision/CameraCalibration.h>
#include <cvt/vision/Vision.h>

#include <cvt/gfx/ifilter/IWarp.h>


using namespace cvt;


void loadCameraCalib( CameraCalibration& camCalib, const String& file )
{
	XMLDocument xmlDoc;
	xmlDoc.load( file );
	
	XMLNode* node = xmlDoc.nodeByName( "CameraCalibration" );
	camCalib.deserialize( node );
	/*
	std::cout << "CamCalib:\n" << camCalib0.extrinsics() <<  "\n" 
						       << camCalib0.intrinsics() << "\n"
							   << camCalib0.radialDistortion() << "\n"
							   << camCalib0.tangentialDistortion() << std::endl;
							   */
}

int main( int argc, char* argv[] )
{
	Resources r;

	String calib0 = r.find( "calib_stereo/ueye_stereo_4002738788.xml" );

	CameraCalibration camCalib0;
	loadCameraCalib( camCalib0, calib0 );

	Image i0( r.find( "calib_stereo/ueye_stereo_4002738788.png" ) );
	Image out( i0 ), warp( i0.width(), i0.height(), IFormat::GRAYALPHA_FLOAT );

	Matrix3f knew;
	IWarp::warpUndistort( warp, knew, camCalib0, i0.width(), i0.height() );
	IWarp::apply( out, i0, warp );

	out.save( "undistort.png" );

	return 0;
}
