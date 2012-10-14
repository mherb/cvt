#include <cvt/vision/rgbdvo/RGBDKeyframe.h>
#include <cvt/vision/rgbdvo/IntensityKeyframe.h>
#include <cvt/vision/rgbdvo/RGBDWarp.h>
#include <cvt/vision/rgbdvo/Optimizer.h>
#include <cvt/vision/rgbdvo/LMOptimizer.h>
#include <cvt/vision/rgbdvo/TROptimizer.h>

#include <cvt/util/ConfigFile.h>

using namespace cvt;

void run( ConfigFile& cfg )
{
    typedef StandardWarp<float> Warp;
    typedef Huber<float>        LossFunc;

    Optimizer<Warp, LossFunc>   optimizer;
    optimizer.setMaxIterations( cfg.valueForName<int>( "maxiters", 10 ) );
    optimizer.setMinUpdate( cfg.valueForName<float>( "minupdate", 0.000001f ) );

    Matrix3f K( 520.9f,   0.0f, 325.1f,
                  0.0f, 521.0f, 249.7f,
                  0.0f,   0.0f,   1.0f );

    size_t nOctaves = cfg.valueForName<int>( "octaves", 3 );
    float  scale    = cfg.valueForName<float>( "scale", 0.5f );
    float  dScale   = cfg.valueForName<float>( "depthScale", 1000.0f );
    float  dFactor  = cfg.valueForName<float>( "depthFactor", 1000.0f );
    float  gThresh  = cfg.valueForName<float>( "gradientThreshold", 0.02f );

    typedef IntensityKeyframe<Warp> KeyframeType;
    KeyframeType keyframe( K, nOctaves, scale );
    keyframe.setDepthMapScaleFactor( dScale * dFactor );
    keyframe.setGradientThreshold( gThresh );

    String referenceRGB = cfg.valueForName<String>( "referenceRGB", String( "bla" ) );
    String referenceD   = cfg.valueForName<String>( "referenceDepth", String( "bla" ) );
    String currentRGB   = cfg.valueForName<String>( "currentRGB", String( "bla" ) );
    String currentD     = cfg.valueForName<String>( "currentDepth", String( "bla" ) );

    Image rr, rd, cr, cd;
    std::cout << "Loading " << referenceRGB << std::endl;
    rr.load( referenceRGB );
    std::cout << "Loading " << referenceD << std::endl;
    rd.load( referenceD );
    std::cout << "Loading " << currentRGB << std::endl;
    cr.load( currentRGB );
    std::cout << "Loading " << currentD << std::endl;
    cd.load( currentD );

    Image refgray, refdepth, curgray, curdepth;
    rr.convert( refgray, IFormat::GRAY_FLOAT );
    rd.convert( refdepth, IFormat::GRAY_FLOAT );
    cr.convert( curgray, IFormat::GRAY_FLOAT );
    cd.convert( curdepth, IFormat::GRAY_FLOAT );

    ImagePyramid pyr( nOctaves, scale );
    pyr.update( refgray );

    Matrix4f start;
    start.setIdentity();

    keyframe.updateOfflineData( start, pyr, refdepth );

    pyr.update( curgray );


    start = Matrix4f(   0.99803,     -0.00527,      0.06215,      0.03010,
                        0.00458,      0.99992,      0.01120,      0.00440,
                       -0.06223,     -0.01088,      0.99799,     -0.03222,
                        0.00000,      0.00000,      0.00000,      1.00000 );


    KeyframeType::Result result;
    optimizer.optimize( result, start, keyframe, pyr, curdepth );

    std::cout << result.warp.poseMatrix() << std::endl;

}

int main( int argc, char* argv[] )
{
    if( argc < 2 ){
        std::cout << "Usage: " << argv[ 0 ] << " <cfgfile>" << std::endl;
        return 0;
    }

    ConfigFile cfg( argv[ 1 ] );

    run( cfg );

    return 0;
}
