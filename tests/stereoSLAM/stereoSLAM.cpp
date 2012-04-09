#include <iostream>

#include <cvt/io/Resources.h>
#include <cvt/io/xml/XMLDocument.h>
#include <cvt/io/ImageSequence.h>
#include <cvt/io/RawVideoReader.h>
#include <cvt/io/Camera.h>
#include <cvt/vision/CameraCalibration.h>

#include <cvt/gui/Application.h>
#include <cvt/vision/slam/stereo/StereoSLAM.h>

#include "StereoSLAMApp.h"

using namespace cvt;

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

void loadSequenceFromFolder( std::vector<VideoInput*> & videos,
                             CameraCalibration& c0,
                             CameraCalibration& c1,
                             const String& id0,
                             const String& id1,
                             const String& folder )
{
    String file;
    file.sprintf( "%sueye_%s.xml", folder.c_str(), id0.c_str() );
    c0.load( file );
    file.sprintf( "%sueye_%s.xml", folder.c_str(), id1.c_str() );
    c1.load( file );

    file.sprintf( "%sueye_%s.rawvideo", folder.c_str(), id0.c_str() );
    videos.push_back( new RawVideoReader( file ) );
    file.sprintf( "%sueye_%s.rawvideo", folder.c_str(), id1.c_str() );
    videos.push_back( new RawVideoReader( file ) );
}

int main( int argc, char* argv[] )
{
    bool useSeq = false;
    String folder = "";
    Resources r;
    if( argc > 1 ){
        String option( argv[ 1 ] );
        if( option == "SEQUENCE" )
            useSeq = true;
        if( argc > 2 ){
            folder = argv[ 2 ];
        } else {
            folder = r.find( "stereoSLAM/floor_2min_44fps" );
        }
    }

    //String id0( "4002738788" );
    //String id1( "4002738791" );
    
	String id0( "4002738790" );
    String id1( "4002738788" );

    CameraCalibration camCalib0, camCalib1;
    std::vector<VideoInput*> input;

    if( useSeq ){
        if( folder[ folder.length() - 1 ] != '/' )
            folder += "/";
        loadSequenceFromFolder( input,
                                camCalib0, camCalib1,
                                id0, id1,
                                folder );
    } else {
        initCameras( input, id0, id1 );
        String calib0 = r.find( "stereoSLAM/calib/ueye_4002738788.xml" );
        String calib1 = r.find( "stereoSLAM/calib/ueye_4002738790.xml" );
        camCalib0.load( calib0 );
        camCalib1.load( calib1 );
    }

    input[ 0 ]->nextFrame();
    input[ 1 ]->nextFrame();

    StereoSLAMApp slamApp( input, camCalib0, camCalib1 );
    Application::run();

    return 0;
}
