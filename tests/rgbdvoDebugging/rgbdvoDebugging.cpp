#include <cvt/vision/rgbdvo/RGBDKeyframe.h>
#include <cvt/vision/rgbdvo/ICKeyframe.h>
#include <cvt/vision/rgbdvo/ESMKeyframe.h>
#include <cvt/vision/rgbdvo/RGBDWarp.h>
#include <cvt/vision/rgbdvo/Optimizer.h>
#include <cvt/vision/rgbdvo/GNOptimizer.h>
#include <cvt/vision/rgbdvo/LMOptimizer.h>
#include <cvt/vision/rgbdvo/TROptimizer.h>

#include <cvt/util/ConfigFile.h>

using namespace cvt;

Matrix4f poseFromVals( ConfigFile& cfg, const String& base )
{
    String query;

    query.sprintf( "%s_q_w", base.c_str() );
    float qw = cfg.valueForName<float>( query, 0.0f );
    query.sprintf( "%s_q_x", base.c_str() );
    float qx = cfg.valueForName<float>( query, 0.0f );
    query.sprintf( "%s_q_y", base.c_str() );
    float qy = cfg.valueForName<float>( query, 0.0f );
    query.sprintf( "%s_q_z", base.c_str() );
    float qz = cfg.valueForName<float>( query, 0.0f );
    query.sprintf( "%s_t_x", base.c_str() );
    float tx = cfg.valueForName<float>( query, 0.0f );
    query.sprintf( "%s_t_y", base.c_str() );
    float ty = cfg.valueForName<float>( query, 0.0f );
    query.sprintf( "%s_t_z", base.c_str() );
    float tz = cfg.valueForName<float>( query, 0.0f );


    Matrix4f mat( cvt::Quaternionf( qx, qy, qz, qw ).toMatrix4() );
    mat[ 0 ][ 3 ] = tx;
    mat[ 1 ][ 3 ] = ty;
    mat[ 2 ][ 3 ] = tz;

    return mat;
}

void run( ConfigFile& cfg )
{
    typedef StandardWarp Warp;
    typedef Huber<float> LossFunc;
    typedef GNOptimizer<Warp, LossFunc> OptimizerType;

    GNOptimizer<Warp, LossFunc>   optimizer;
    optimizer.setMaxIterations( cfg.valueForName<int>( "maxiters", 10 ) );
    optimizer.setMinUpdate( cfg.valueForName<float>( "minupdate", 0.000001f ) );

    Matrix3f K( 520.9f,   0.0f, 325.1f,
                  0.0f, 521.0f, 249.7f,
                  0.0f,   0.0f,   1.0f );

    size_t nOctaves = cfg.valueForName<int>( "octaves", 3 );
    float  scale    = cfg.valueForName<float>( "scale", 0.5f );
    float  dScale   = cfg.valueForName<float>( "depthScale", 1.0f );
    float  dFactor  = cfg.valueForName<float>( "depthFactor", 1000.0f );
    float  gThresh  = cfg.valueForName<float>( "gradientThreshold", 0.02f );

    typedef ICKeyframe<Warp> KeyframeType;
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

    // update reference keyframe
    pyr.update( refgray );
    Matrix4f start;
    start.setIdentity();
    keyframe.updateOfflineData( start, pyr, refdepth );

    // compute relative pose using current image
    pyr.update( curgray );
    start = poseFromVals( cfg, "start" );
    OptimizerType::Result result;
    optimizer.optimize( result, start, keyframe, pyr, curdepth );

    Matrix4f gt = poseFromVals( cfg, "gt" );
    std::cout << "Estimated\n" << result.warp.pose() << std::endl;
    std::cout << "True\n" << gt << std::endl;
    std::cout << "Delta\n" << gt.inverse() * result.warp.pose() << std::endl;
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
