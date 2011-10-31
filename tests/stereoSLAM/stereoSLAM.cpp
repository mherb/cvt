#include <iostream>

#include <cvt/io/Resources.h>
#include "cvt/io/xml/XMLDocument.h"
#include "cvt/io/ImageSequence.h"
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
	String tmp = base + "/left_";
	imageSequence.push_back( new ImageSequence( tmp, "cvtraw", 1, 2153, 5 ) );
	tmp = base + "/right_";
	imageSequence.push_back( new ImageSequence( tmp, "cvtraw", 1, 2153, 5 ) );
}

int main( int argc, char* argv[] )
{
	
	Resources r;
	String calib0 = r.find( "stereoSLAM/calib/ueye_stereo_4002738790.xml" );
	String calib1 = r.find( "stereoSLAM/calib/ueye_stereo_4002738788.xml" );

	CameraCalibration camCalib0, camCalib1;
	loadCameraCalib( camCalib0, calib0 );
	loadCameraCalib( camCalib1, calib1 );

	std::vector<VideoInput*> input;
	initImageSequences( input );

	StereoSLAMApp slamApp( input, camCalib0, camCalib1 );
	Application::run();

	return 0;
}
