#include <iostream>

#include <cvt/io/Resources.h>
#include "cvt/io/xml/XMLDocument.h"
#include "cvt/io/ImageSequence.h"
#include <cvt/vision/CameraCalibration.h>
#include <cvt/vision/Vision.h>
#include <cvt/vision/ORB.h>
#include <cvt/vision/FeatureMatch.h>

#include <cvt/util/Time.h>

#include "StereoSLAM.h"

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

	String calib0 = r.find( "calib/ueye_stereo_4002738790.xml" );
	String calib1 = r.find( "calib/ueye_stereo_4002738788.xml" );

	CameraCalibration camCalib0, camCalib1;
	loadCameraCalib( camCalib0, calib0 );
	loadCameraCalib( camCalib1, calib1 );

	std::vector<VideoInput*> input;
	initImageSequences( input );

	Image i0( input[ 0 ]->width(), input[ 0 ]->height(), input[ 0 ]->format() );
	Image i1( input[ 1 ]->width(), input[ 1 ]->height(), input[ 1 ]->format() );

	StereoSLAM slam( camCalib0, input[ 0 ]->width(), input[ 0 ]->height(), 
					 camCalib1, input[ 1 ]->width(), input[ 1 ]->height() );

	Time t;
	while( true ){
		input[ 0 ]->nextFrame();
		input[ 1 ]->nextFrame();

		i0 = input[ 0 ]->frame();
		i1 = input[ 1 ]->frame();

		slam.newImages( i0, i1 );

		std::cout << "Loop Time: " << t.elapsedMilliSeconds() << "ms" << std::endl; 
	}

	delete input[ 0 ];
	delete input[ 1 ];

	return 0;
}
