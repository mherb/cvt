#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/gfx/ifilter/IWarp.h>
#include <cvt/gfx/ifilter/StereoGCVFilter.h>
#include <cvt/gfx/ifilter/BoxFilter.h>
#include <cvt/io/Resources.h>
#include <cvt/io/xml/XMLDocument.h>
#include <cvt/vision/CameraCalibration.h>
#include <cvt/vision/Vision.h>


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

	std::vector<CLPlatform> platforms;
	CLPlatform::get( platforms );
	CL::setDefaultDevice( platforms[ 0 ].defaultDevice() );

/*	Resources r;

	String calib0 = r.find( "calib_stereo/ueye_stereo_4002738788.xml" );
	String calib1 = r.find( "calib_stereo/ueye_stereo_4002738790.xml" );

	CameraCalibration camCalib0, camCalib1;
	loadCameraCalib( camCalib0, calib0 );
	loadCameraCalib( camCalib1, calib1 );

	Matrix4f P0, P1;

	P0 = camCalib0.intrinsics().toMatrix4() * camCalib0.extrinsics();
	P1 = camCalib1.intrinsics().toMatrix4() * camCalib1.extrinsics();

	Image i0( r.find( "calib_stereo/ueye_stereo_4002738788.png" ) );
	Image i1( r.find( "calib_stereo/ueye_stereo_4002738790.png" ) );
	Image out;

	Image warp, tmp0, tmp1;
	// image 0
	Vector3f radial = camCalib0.radialDistortion();
	Vector2f tangential = camCalib0.tangentialDistortion();

	float fx = camCalib0.intrinsics()[ 0 ][ 0 ];
	float fy = camCalib0.intrinsics()[ 1 ][ 1 ];
	float cx = camCalib0.intrinsics()[ 0 ][ 2 ];
	float cy = camCalib0.intrinsics()[ 1 ][ 2 ];

	warp.reallocate( i0.width(), i0.height() , IFormat::GRAYALPHA_FLOAT );
	IWarp::warpUndistort( warp, radial[ 0 ], radial[ 1 ], cx, cy, fx, fy, i0.width(), i0.height(), radial[ 2 ], tangential[ 0 ], tangential[ 1 ] );
	tmp0.reallocate( i0.width() , i0.height(), IFormat::GRAY_FLOAT );
	i0.convert( tmp0, IFormat::GRAY_FLOAT );
	tmp1.reallocate( i0.width(), i0.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL );
	IWarp::apply( tmp1, tmp0, warp );
//	tmp1.save("undistort0.png");

	Image img0( i0.width(), i0.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL );
	img0 = tmp1;


	// image 1
	radial = camCalib1.radialDistortion();
	tangential = camCalib1.tangentialDistortion();

	fx = camCalib1.intrinsics()[ 0 ][ 0 ];
	fy = camCalib1.intrinsics()[ 1 ][ 1 ];
	cx = camCalib1.intrinsics()[ 0 ][ 2 ];
	cy = camCalib1.intrinsics()[ 1 ][ 2 ];

	warp.reallocate( i1.width(), i1.height() , IFormat::GRAYALPHA_FLOAT );
	IWarp::warpUndistort( warp, radial[ 0 ], radial[ 1 ], cx, cy, fx, fy, i1.width(), i1.height(), radial[ 2 ], tangential[ 0 ], tangential[ 1 ] );
	tmp0.reallocate( i1.width() , i1.height(), IFormat::GRAY_FLOAT );
	i1.convert( tmp0, IFormat::GRAY_FLOAT );
	IWarp::apply( tmp1, tmp0, warp );

	Image img1( i1.width(), i1.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL );
	img1 = tmp1;
//	tmp1.save("undistort1.png");
*/
	StereoGCVFilter stereogcv;
    Image i0, i1, tmp;
	tmp.load( argv[ 1 ] );
	tmp.convert( i0, IFormat::RGBA_UINT8 );
	tmp.load( argv[ 2 ] );
	tmp.convert( i1, IFormat::RGBA_UINT8 );

	float D = 100.0f;
	if( argc >= 4 )
		D = String( argv[ 3 ] ).toFloat();


	Image img0( i0, IALLOCATOR_CL );
	Image img1( i1, IALLOCATOR_CL );

	Image disp;//( i0.width(), i0.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL );
	Matrix4f T( 1.0f, 0.0f, D, 0.0f,
			    0.0f, 1.0f, 0.0f, 0.0f,
			    0.0f, 0.0f, 0.0f, 1.0f,
			    0.0f, 0.0f, 0.0f, 1.0f );
	stereogcv.apply( disp, img1, img0, T, 0.0f, 1.0f, 1.0f / D );
	disp.save( "disparity.png" );

	return 0;
}
