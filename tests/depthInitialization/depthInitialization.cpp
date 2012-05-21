#include <cvt/vision/slam/stereo/DepthInitializer.h>
#include <cvt/io/Resources.h>
#include <cvt/io/RawVideoReader.h>
#include <cvt/util/String.h>
#include <cvt/io/FileSystem.h>

#include <DepthInitApp.h>

using namespace cvt;

void loadSequenceFromFolder( std::vector<VideoInput*> & videos,
                             std::vector<CameraCalibration> & calibrations,
                             const String& folder )
{
    std::vector<String> xmlFiles;
    FileSystem::filesWithExtension( folder, xmlFiles, "xml" );

    if( xmlFiles.size() != 2 )
        throw CVTException( "found more than 2 xml files, cannot determine which are the calibration files" );

    String id0 = xmlFiles[ 0 ];
    String id1 = xmlFiles[ 1 ];

    CameraCalibration& c0 = calibrations[ 0 ];
    CameraCalibration& c1 = calibrations[ 1 ];


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
    if( argc < 2 ){
        std::cout << "Usage: " << argv[ 0 ] << " <datafolder>" << std::endl;
        return 0;
    }

    Resources r;
    String folder( argv[ 1 ] );
    if( folder[ folder.length() - 1 ] != '/' )
        folder += "/";

    std::vector<CameraCalibration> calibrations( 2 );
    std::vector<VideoInput*> input;

    loadSequenceFromFolder( input,
                            calibrations,
                            folder );


    DepthInitApp app( input, calibrations );
    Application::run();

    for( size_t i = 0; i < input.size(); i++ ){
        delete input[ i ];
    }


    return 0;
}
