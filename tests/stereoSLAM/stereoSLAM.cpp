#include <iostream>

#include <cvt/io/Resources.h>
#include "cvt/io/xml/XMLDocument.h"
#include <cvt/vision/CameraCalibration.h>
#include <cvt/vision/Vision.h>
#include <cvt/vision/ORB.h>
#include <cvt/vision/FeatureMatch.h>
#include <cvt/gfx/ifilter/IWarp.h>
#include <cvt/geom/Line2D.h>
#include "ORBStereoMatching.h"

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

	String calib0 = r.find( "calib/ueye_stereo_4002738790.xml" );
	String calib1 = r.find( "calib/ueye_stereo_4002738788.xml" );

	CameraCalibration camCalib0, camCalib1;
	loadCameraCalib( camCalib0, calib0 );
	loadCameraCalib( camCalib1, calib1 );

	Image i0( r.find( "ueye_stereo_left.cvtraw" ) );
	Image i1( r.find( "ueye_stereo_right.cvtraw" ) );
	Image id0, id1, tmp;

	Image warp;
	warp.reallocate( i0.width(), i0.height(), IFormat::GRAYALPHA_FLOAT );
	Vector3f radial = camCalib0.radialDistortion();
	Vector2f tangential = camCalib0.tangentialDistortion();
	float fx = camCalib0.intrinsics()[ 0 ][ 0 ];
	float fy = camCalib0.intrinsics()[ 1 ][ 1 ];
	float cx = camCalib0.intrinsics()[ 0 ][ 2 ];
	float cy = camCalib0.intrinsics()[ 1 ][ 2 ];
	IWarp::warpUndistort( warp, radial[ 0 ], radial[ 1 ], cx, cy, fx, fy, i0.width(), i0.height(), radial[ 2 ], tangential[ 0 ], tangential[ 1 ] );
	i0.convert( tmp, IFormat::RGBA_UINT8 );
	IWarp::apply( id0, tmp, warp );

	radial = camCalib1.radialDistortion();
	tangential = camCalib1.tangentialDistortion();
	fx = camCalib1.intrinsics()[ 0 ][ 0 ];
	fy = camCalib1.intrinsics()[ 1 ][ 1 ];
	cx = camCalib1.intrinsics()[ 0 ][ 2 ];
	cy = camCalib1.intrinsics()[ 1 ][ 2 ];
	IWarp::warpUndistort( warp, radial[ 0 ], radial[ 1 ], cx, cy, fx, fy, i1.width(), i1.height(), radial[ 2 ], tangential[ 0 ], tangential[ 1 ] );
	i1.convert( tmp, IFormat::RGBA_UINT8 );
	IWarp::apply( id1, tmp, warp );

	return 0;
}
