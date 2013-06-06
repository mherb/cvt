#include <iostream>

#include <cvt/io/xml/XMLDocument.h>
#include <cvt/io/ImageSequence.h>
#include <cvt/io/RawVideoReader.h>
#include <cvt/io/Camera.h>
#include <cvt/io/KittiVOParser.h>
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
                             std::vector<CameraCalibration> & calibs,
                             const String& folder )
{
    std::vector<String> xmlFiles;
    FileSystem::filesWithExtension( folder, xmlFiles, "xml" );

    if( xmlFiles.size() != 2 )
        throw CVTException( "found more than 2 xml files, cannot determine which are the calibration files" );

    String id0 = xmlFiles[ 0 ];
    String id1 = xmlFiles[ 1 ];

    calibs.resize( 2 );
    calibs[ 0 ].load( id0 );
    calibs[ 1 ].load( id1 );

    cvt::Matrix4f I;
    I.setIdentity();
    if( calibs[ 0 ].extrinsics() != I ) {
        CameraCalibration tmp = calibs[ 0 ];
        calibs[ 0 ] = calibs[ 1 ];
        calibs[ 1 ] = tmp;

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
    srand( time(NULL) );

    if( argc < 2 ){
        std::cout << "Usage: " << argv[ 0 ] << " <kitti_folder>" << std::endl;
        return 0;
    }

    try {
        cvt::String folder( argv[ 1 ] );
        StereoInput* input = new KittiVOParser( folder );
        StereoSLAMApp slamApp( input );
        Application::run();
    } catch( const cvt::Exception& e ){
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
