#include <cvt/util/String.h>
#include <cvt/util/EigenBridge.h>
#include <cvt/math/Matrix.h>
#include <cvt/gui/Application.h>

#include <RGBDVOApp.h>


using namespace cvt;


int main( int argc, char* argv[] )
{
    if( argc < 2 ){
        std::cout << "Usage: " << argv[ 0 ] << " <rgbd_dataset_folder>" << std::endl;
        return 1;
    }

    String folder( argv[ 1 ] );

    Matrix3f K;
    K.setIdentity();
    K[ 0 ][ 0 ] = 520.9f; K[ 0 ][ 2 ] = 325.1f;
    K[ 1 ][ 1 ] = 521.0f; K[ 1 ][ 2 ] = 249.7f;


    RGBDVOApp app( folder, K );

    Application::run();

    return 0;
}
