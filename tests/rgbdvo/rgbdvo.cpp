#include <cvt/util/String.h>
#include <cvt/util/EigenBridge.h>
#include <cvt/math/Matrix.h>
#include <cvt/gui/Application.h>

#include <RGBDVOApp.h>
#include <EvalRun.h>
#include <ConvergenceEval.h>
#include <cvt/util/ConfigFile.h>
#include <cvt/vision/CameraCalibration.h>
#include <cvt/vision/rgbdvo/Optimizer.h>
#include <cvt/vision/rgbdvo/GNOptimizer.h>
#include <cvt/vision/rgbdvo/LMOptimizer.h>
#include <cvt/vision/rgbdvo/DLOptimizer.h>
#include <cvt/vision/rgbdvo/RGBDKeyframe.h>
#include <cvt/vision/rgbdvo/ICKeyframe.h>
#include <cvt/vision/rgbdvo/FCKeyframe.h>
#include <cvt/vision/rgbdvo/GlobalFCKeyframe.h>
#include <cvt/vision/rgbdvo/ESMKeyframe.h>

using namespace cvt;

template <class Warp, class LossFunc>
inline Optimizer<Warp, LossFunc>* createOptimizer( const String& optimizerName )
{
    if( optimizerName == "GaussNewton" ){
        std::cout << "Creating GaussNewton Optimizer" << std::endl;
        return new GNOptimizer<Warp, LossFunc>();
    } else if( optimizerName == "LevenbergMarquardt" ){
        std::cout << "Creating LevenbergMarquardt Optimizer" << std::endl;
        return new LMOptimizer<Warp, LossFunc>();
    } else if( optimizerName == "DogLeg" ){
        std::cout << "Creating TrustRegion Optimizer" << std::endl;
        return new DLOptimizer<Warp, LossFunc>();
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
        String parName;
        for( int i = 0; i < reg.cols(); i++ ){
            parName.sprintf( "reg_dim_%02d", i );
            reg.coeffRef( i, i ) = cfg.valueForName<float>( parName, 1.0f );
        }
        optimizer->setUseRegularization( true );
        optimizer->setRegularizationMatrix( reg );
        optimizer->setRegularizationAlpha( cfg.valueForName<float>( "regularization_alpha", 0.5f ) );
    }

    optimizer->setCostStopThreshold( cfg.valueForName<float>( "optimizer_cost_stop_thresh", 0.0001f ) );

    String runMode = cfg.valueForName<String>( "runMode", "BATCH" );
    if( runMode == "CONV_EVAL" ){
        ConvergenceEval<KF, LF> eval( optimizer, K, folder, cfg );
        eval.evaluate();
    } else {
        RGBDVisualOdometry<KF, LF> vo( optimizer, K,  cfg );
        if( runMode == "GUI" ){
            RGBDVOApp<KF, LF> app( &vo, folder );
            Application::run();
        } else if( runMode == "BATCH" ){
            std::cout << "Starting batch mode" << std::endl;
            EvalRun<KF, LF> eval( vo, folder, cfg );
            eval.evaluateDataSetPerformance( cfg );
        }
    }
    std::cout << "BYE" << std::endl;
    delete optimizer;
    cfg.save( "rgbdvo.cfg" );
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
    } else if( keyframeType == "FC" ){
        std::cout << "Using Fwd Compositional Algorithm" << std::endl;
        createAppDefineLF<FCKeyframe<WarpType>, WarpType>( K, folder, cfg );
    } else if( keyframeType == "GFC" ){
        std::cout << "Using Global Fwd Compositional Algorithm" << std::endl;
        createAppDefineLF<GlobalFCKeyframe<WarpType>, WarpType>( K, folder, cfg );
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

    if( argc < 2 ){
        std::cout << "Usage: " << argv[ 0 ] << " <rgbd_dataset_folder>" << std::endl;
        return 1;
    }

    String folder( argv[ 1 ] );
    if( folder[ folder.length() - 1 ] != '/' )
        folder += "/";

    String calibFile( folder + "calib.xml" );
    CameraCalibration calib;
    calib.load( calibFile );

    runApplication( calib.intrinsics(), folder, cfg );
    return 0;
}
