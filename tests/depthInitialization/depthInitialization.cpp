#include <cvt/vision/slam/stereo/DepthInitializer.h>
#include <cvt/io/Resources.h>
#include <cvt/io/RawVideoReader.h>
#include <cvt/util/String.h>

using namespace cvt;

void loadSequenceFromFolder( std::vector<VideoInput*> & videos,
                             CameraCalibration& c0,
                             CameraCalibration& c1,
                             String& id0,
                             String& id1,
                             const String& folder )
{
    String file;
    file.sprintf( "%sueye_%s.xml", folder.c_str(), id0.c_str() );
    c0.load( file );
    file.sprintf( "%sueye_%s.xml", folder.c_str(), id1.c_str() );
    c1.load( file );

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

    file.sprintf( "%sueye_%s.rawvideo", folder.c_str(), id0.c_str() );
    videos.push_back( new RawVideoReader( file ) );
    file.sprintf( "%sueye_%s.rawvideo", folder.c_str(), id1.c_str() );
    videos.push_back( new RawVideoReader( file ) );
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

    // TODO: find out from the folder which ids we have!
    //String id0( "4002738788" );
    //String id1( "4002738791" );

    String id0( "4002738790" );
    String id1( "4002738788" );

    CameraCalibration camCalib0, camCalib1;
    std::vector<VideoInput*> input;

    loadSequenceFromFolder( input,
                           camCalib0, camCalib1,
                           id0, id1,
                           folder );

    return 0;
}
