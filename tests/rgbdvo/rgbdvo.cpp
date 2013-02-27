#include <cvt/util/String.h>
#include <cvt/util/EigenBridge.h>
#include <cvt/util/Time.h>
#include <cvt/math/Matrix.h>
#include <cvt/gui/Application.h>

#include <RGBDVOApp.h>
#include <cvt/util/ConfigFile.h>
#include <cvt/vision/rgbdvo/Optimizer.h>
#include <cvt/vision/rgbdvo/GNOptimizer.h>
#include <cvt/vision/rgbdvo/LMOptimizer.h>
#include <cvt/vision/rgbdvo/TROptimizer.h>
#include <cvt/vision/rgbdvo/RGBDKeyframe.h>
#include <cvt/vision/rgbdvo/ICKeyframe.h>
#include <cvt/vision/rgbdvo/ESMKeyframe.h>

#include <Eigen/Geometry>

using namespace cvt;

void writePoseToFile( std::ofstream& file, const Matrix4f& pose, double stamp )
{
    Quaternionf q( pose.toMatrix3() );

    file.precision( 15 );
    file << std::fixed << stamp << " "
             << pose[ 0 ][ 3 ] << " "
             << pose[ 1 ][ 3 ] << " "
             << pose[ 2 ][ 3 ] << " "
             << q.x << " "
             << q.y << " "
             << q.z << " "
             << q.w << std::endl;
}

template <class KFType, class LossFunc>
void runVOWithKFType( const Matrix3f& K, const String& folder, ConfigFile& cfg )
{
    std::cout << "Press enter to start ...";
    std::flush( std::cout );
    getchar();

    RGBDParser parser( folder, 0.05f );
    RGBDVisualOdometry<KFType, LossFunc> vo( K, cfg );
    vo.setMaxRotationDistance( cfg.valueForName( "maxRotationDist", 3.0f ) );
    vo.setMaxTranslationDistance( cfg.valueForName( "maxTranslationDist", 3.0f ) );
    vo.setMaxSSD( cfg.valueForName( "maxSSD", 0.2f ) );

    parser.setIdx( cfg.valueForName( "dataStartIdx", 0 ) );
    parser.loadNext();
    const RGBDParser::RGBDSample& sample = parser.data();

    Image gray( sample.rgb.width(), sample.rgb.height(), IFormat::GRAY_FLOAT );
    Image depth( sample.depth.width(), sample.depth.height(), IFormat::GRAY_FLOAT );
    sample.rgb.convert( gray );
    sample.depth.convert( depth );

    vo.addNewKeyframe( gray, depth, sample.pose<float>() ); // add initial

    std::ofstream file;
    file.open( "trajectory.txt" );
    Matrix4f pose; pose.setIdentity();

    Time time;
    size_t iters = 0;
    float timeSum = 0;

    float translationError = 0.0f;

    pose = vo.pose();
    while( parser.hasNext() ){
        parser.loadNext();
        const RGBDParser::RGBDSample& d = parser.data();

        time.reset();
        d.rgb.convert( gray );
        d.depth.convert( depth );
        pose = vo.pose();
        vo.updatePose( pose, gray, depth );

        timeSum += time.elapsedMilliSeconds();
        iters++;

        if( d.poseValid ){
            float currError = ( d.pose<float>() - pose ).col( 3 ).length();

            float errorChange = currError - translationError;
            std::cout << "ErrorChange: " << errorChange << std::endl;
            translationError = currError;
        }


        writePoseToFile( file, pose, d.stamp );

        std::cout << "\r" << parser.iter() << " / " << parser.size();
        std::flush( std::cout );
        //getchar();
    }
    std::cout << std::endl;
    file.close();

    std::cout << "Average Proc. Time per frame:\t " << timeSum / iters << "ms" << std::endl;
    std::cout << "Number of created Keyframes:\t "	<< vo.numOverallKeyframes() << std::endl;
}

template <class Warp, class LossFunc>
inline Optimizer<Warp, LossFunc>* createOptimizer( const String& optimizerName )
{
    if( optimizerName == "GaussNewton" ){
        std::cout << "Creating GaussNewton Optimizer" << std::endl;
        return new GNOptimizer<Warp, LossFunc>();
    }
    if( optimizerName == "LevenbergMarquardt" ){
        std::cout << "Creating LevenbergMarquardt Optimizer" << std::endl;
        return new LMOptimizer<Warp, LossFunc>();
    }
    if( optimizerName == "TrustRegion" ){
        std::cout << "Creating TrustRegion Optimizer" << std::endl;
        return new TROptimizer<Warp, LossFunc>();
    }

    throw CVTException( "unknown OptimizerType requested" );
}

template <class KF, class WF, class LF>
void runAppWithTypes( const Matrix3f& K, const String& folder, ConfigFile& cfg )
{
    Optimizer<WF, LF>* optimizer = createOptimizer<WF, LF>( cfg.valueForName<String>( "optimizer", "GaussNewton" ) );

    bool useRegularization = cfg.valueForName( "useRegularization", false );
    if( useRegularization ){
        typename Optimizer<WF, LF>::HessianType reg;
        reg.setIdentity();
        optimizer->setUseRegularization( true );
        optimizer->setRegularizationMatrix( reg );
    }

    RGBDVisualOdometry<KF, LF> vo( optimizer, K,  cfg );
    {
        RGBDVOApp<KF, LF> app( &vo, folder );
        cfg.save( "rgbdvo.cfg" );
        Application::run();
    }
    delete optimizer;
}

template <class KFType, class WarpType>
void createAppDefineLF( const Matrix3f& K, const String& folder, ConfigFile& cfg )
{
    String lfType = cfg.valueForName<String>( "lossFunction", "Squared" );
    if ( lfType == "Squared" ){
        std::cout << "Using Squared Loss" << std::endl;
        runAppWithTypes<KFType, WarpType, NoWeightingf>( K, folder, cfg );
    } else if( lfType == "Huber" ){
        std::cout << "Using Huber Loss" << std::endl;
        runAppWithTypes<KFType, WarpType, Huberf>( K, folder, cfg );
    } else if( lfType == "Tukey" ){
        std::cout << "Using Tukey Loss" << std::endl;
        runAppWithTypes<KFType, WarpType, Tukeyf>( K, folder, cfg );
    } else {
        throw CVTException( "unknown keyframeType" );
    }
}

template <class WarpType>
void createAppDefineKF( const Matrix3f& K, const String& folder, ConfigFile& cfg )
{
    String keyframeType = cfg.valueForName<String>( "keyframe", "IC" );
    if ( keyframeType == "IC" ){
        std::cout << "Using InverseCompositional Algorithm" << std::endl;
        createAppDefineLF<ICKeyframe<WarpType>, WarpType>( K, folder, cfg );
    } else if( keyframeType == "ESM" ){
        std::cout << "Using ESM Algorithm" << std::endl;
        createAppDefineLF<ESMKeyframe<WarpType>, WarpType>( K, folder, cfg );
    } else {
        throw CVTException( "unknown keyframeType" );
    }
}

void runApplication( const Matrix3f& K, const String& folder, ConfigFile& cfg )
{
    // define warp:
    String warpType = cfg.valueForName<String>( "warp", "Standard" );
    if( warpType == "Standard" ){
        createAppDefineKF<StandardWarp>( K, folder, cfg );
    } else if( warpType == "AI" ){
        createAppDefineKF<AffineLightingWarp>( K, folder, cfg );
    } else {
        throw CVTException( "unkown warp" );
    }
}

int main( int argc, char* argv[] )
{
    ConfigFile cfg( "rgbdvo.cfg" );
    //ConfigFile cfg( "rgbdvo_freiburg1.cfg" );

    if( argc < 2 ){
        std::cout << "Usage: " << argv[ 0 ] << " <rgbd_dataset_folder>" << std::endl;
        return 1;
    }

    String folder( argv[ 1 ] );

    Matrix3f K;
    K.setIdentity();
    // freiburg 1
    // K[ 0 ][ 0 ] = 517.306408;
    // K[ 0 ][ 2 ] = 318.643040;
    // K[ 1 ][ 1 ] = 516.469215;
    // K[ 1 ][ 2 ] = 255.313989;
    // freiburg 2
    K[ 0 ][ 0 ] = 520.9f;
    K[ 0 ][ 2 ] = 325.1f;
    K[ 1 ][ 1 ] = 521.0f;
    K[ 1 ][ 2 ] = 249.7f;

    String runMode = cfg.valueForName<String>( "runMode", "BATCH" );

    //cfg.save( "rgbdvo.cfg" );
    //std::cout << "Saving config" << std::endl;

    if( runMode == "BATCH" ){
        std::cout << "Starting batch mode" << std::endl;
        //runBatch( K, folder, cfg );
    } else {
        runApplication( K, folder, cfg );
    }


    return 0;
}
