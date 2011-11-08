#include <iostream>

#include <cvt/io/Resources.h>
#include "cvt/io/xml/XMLDocument.h"
#include "cvt/io/ImageSequence.h"
#include <cvt/io/Camera.h>
#include <cvt/vision/CameraCalibration.h>

#include <cvt/gui/Application.h>

#include "StereoSLAM.h"
#include "StereoSLAMApp.h"

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

void initImageSequences( std::vector<VideoInput*> & imageSequence ) 
{
	Resources r;
	String base = r.find( "stereoSLAM" );
	String tmp = base + "/right_";
	imageSequence.push_back( new ImageSequence( tmp, "cvtraw", 1, 2153, 5 ) );
	tmp = base + "/left_";
	imageSequence.push_back( new ImageSequence( tmp, "cvtraw", 1, 2153, 5 ) );
}

void initCameras( std::vector<VideoInput*> & cameras, const String & id0, const String& id1 )
{
    Camera::updateInfo();
	size_t numCams = Camera::count();

	if( numCams < 2 ){
		std::cerr << "Not enough cameras connected" << std::endl;
		exit( 1 );
	}

	Camera * cam0 = 0;
	Camera * cam1 = 0;
	for( size_t i = 0; i < numCams; i++ ){
		Camera * cam = Camera::get( i );
		if( cam->identifier() == id0 ){
			cam0 = cam;
		} else if( cam->identifier() == id1 ){
			cam1 = cam;
		} else {
			delete cam;
		}
	}

	if( cam0 && cam1 ){
		cam0->startCapture();
		cam1->startCapture();
		cameras.push_back( cam0 );
		cameras.push_back( cam1 );
	} else {
		if( cam0 )
			delete cam0;
		if( cam1 )
			delete cam1;
		std::cerr << "Could not find cameras with id" << std::endl;
		exit( 1 );
	}
}

int main( int argc, char* argv[] )
{
	bool useSeq = false;
	if( argc > 1 ){
		String option( argv[ 1 ] );
		if( option == "SEQUENCE" )
			useSeq = true;
	}

	Resources r;
	String id0( "4002738790" );
	String id1( "4002738788" );
	String calib0 = r.find( "stereoSLAM/calib/ueye_stereo_4002738790.xml" );
	String calib1 = r.find( "stereoSLAM/calib/ueye_stereo_4002738788.xml" );

	CameraCalibration camCalib0, camCalib1;
	loadCameraCalib( camCalib0, calib0 );
	loadCameraCalib( camCalib1, calib1 );

	std::vector<VideoInput*> input;

	if( useSeq )
		initImageSequences( input );
	else
		initCameras( input, id0, id1 );

	StereoSLAMApp slamApp( input, camCalib0, camCalib1 );
	Application::run();

	return 0;
}
