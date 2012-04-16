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
                             const String& folder )
{
    std::vector<String> xmlFiles;
    FileSystem::filesWithExtension( folder, xmlFiles, "xml" );

    if( xmlFiles.size() != 2 )
        throw CVTException( "found more than 2 xml files, cannot determine which are the calibration files" );

    String id0 = xmlFiles[ 0 ];
    String id1 = xmlFiles[ 1 ];

    c0.load( id0 );
    c1.load( id1 );

    cvt::Matrix4f I;
    I.setIdentity();
    if( c0.extrinsics() != I ) {
        CameraCalibration tmp = c0;
        c0 = c1;
        c1 = tmp;

        String tmpS;
        tmpS = id0;
        id0 = id1;
        id1 = tmpS;
    }

    // remove the extensions
    String file0 = id0.substring( 0, id0.length() - 3 );
    String file1 = id1.substring( 0, id0.length() - 3 );
    file0 += "rawvideo";
    file1 += "rawvideo";
    videos.push_back( new RawVideoReader( file0 ) );
    videos.push_back( new RawVideoReader( file1 ) );
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


    CameraCalibration camCalib0, camCalib1;
    std::vector<VideoInput*> input;

    if( useSeq ){
        if( folder[ folder.length() - 1 ] != '/' )
            folder += "/";
        loadSequenceFromFolder( input,
                                camCalib0, camCalib1,    
                                folder );
    } else {
        String id0( "4002738790" );
        String id1( "4002738788" );
        initCameras( input, id0, id1 );
        String path;
        path.sprintf( "stereoSLAM/calib/ueye_%s.xml", id0.c_str() );
        String calib0 = r.find( path );
        path.sprintf( "stereoSLAM/calib/ueye_%s.xml", id1.c_str() );
        String calib1 = r.find( path );
        camCalib0.load( calib0 );
        camCalib1.load( calib1 );
    }

    std::cout << "Calib0: " << std::endl;
    std::cout << camCalib0.extrinsics() << std::endl;
    std::cout << camCalib0.intrinsics() << std::endl;
    std::cout << camCalib0.projectionMatrix() << std::endl;


    input[ 0 ]->nextFrame();
    input[ 1 ]->nextFrame();

    StereoSLAMApp slamApp( input, camCalib0, camCalib1 );
    Application::run();

    return 0;
}
