#include <cvt/util/String.h>
#include <cvt/util/EigenBridge.h>
#include <cvt/math/Matrix.h>
#include <cvt/gui/Application.h>

#include <RGBDVOApp.h>

using namespace cvt;

int main( int argc, char* argv[] )
{
    if( argc < 7 ){
        std::cout << "Usage: " << argv[ 0 ] << " <rgbd_dataset_folder> <iterations> <gradThresh> <maxssd> <maxRotDist> <maxTransDist>" << std::endl;
        return 1;
    }

    String folder( argv[ 1 ] );
    VOParams params;
    params.maxIters = atoi( argv[ 2 ] );
    params.gradientThreshold = atof( argv[ 3 ] );
    params.depthScale = 5000.0f / 1.031f;

    float maxSSD = atof( argv[ 4 ] );
    float maxRDist = atof( argv[ 5 ] );
    std::cout << maxRDist << std::endl;
    float maxTDist = atof( argv[ 6 ] );


    Matrix3f K;
    K.setIdentity();
    K[ 0 ][ 0 ] = 520.9f; K[ 0 ][ 2 ] = 325.1f;
    K[ 1 ][ 1 ] = 521.0f; K[ 1 ][ 2 ] = 249.7f;

    RGBDVOApp app( folder, K, params );
    app.setMaxRotationDistance( maxRDist );
    app.setMaxTranslationDistance( maxTDist );
    app.setMaxSSD( maxSSD );

    Application::run();

    return 0;
}
