#include <cvt/util/String.h>
#include <cvt/util/EigenBridge.h>
#include <cvt/util/Time.h>
#include <cvt/math/Matrix.h>
#include <cvt/gui/Application.h>

#include <RGBDVOApp.h>
#include <ConfigFile.h>

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

void testQuaternionPrecision( const Matrix4f& pose )
{
    Matrix3f R = pose.toMatrix3();
    Quaternionf q( R );
    Matrix3f tmp = q.toMatrix3();

    std::cout << tmp-R << std::endl;

}

template <class KFType>
void runVOWithKFType( const VOParams& params, const Matrix3f& K, const String& folder, ConfigFile& cfg )
{
    RGBDParser parser( folder, 0.05f );
    RGBDVisualOdometry<KFType> vo( K, params );
    vo.setMaxRotationDistance( cfg.valueForName( "maxRotationDist", 3.0f ) );
    vo.setMaxTranslationDistance( cfg.valueForName( "maxTranslationDist", 3.0f ) );
    vo.setMaxSSD( cfg.valueForName( "maxSSD", 0.2f ) );

    parser.loadNext();
    const RGBDParser::RGBDSample& sample = parser.data();

    Image gray( sample.rgb.width(), sample.rgb.height(), IFormat::GRAY_FLOAT );
    //Image smoothed( sample.rgb.width(), sample.rgb.height(), IFormat::GRAY_FLOAT );
    sample.rgb.convert( gray );
    //gray.convolve( smoothed, IKernel::GAUSS_HORIZONTAL_3, IKernel::GAUSS_VERTICAL_3 );
    vo.addNewKeyframe( gray, sample.depth, sample.pose ); // add initial

    std::ofstream file;
    file.open( "trajectory.txt" );
    Matrix4f pose; pose.setIdentity();

    size_t stepIter = parser.size() / 20;

	Time time;
	size_t iters = 0;
	float timeSum;	

    while( parser.hasNext() ){
        parser.loadNext();
        const RGBDParser::RGBDSample& d = parser.data();

		time.reset();
        d.rgb.convert( gray );
        //gray.convolve( smoothed, IKernel::GAUSS_HORIZONTAL_3, IKernel::GAUSS_VERTICAL_3 );
        vo.updatePose( gray, d.depth );

		timeSum += time.elapsedMilliSeconds();
		iters++;

        vo.pose( pose );
        writePoseToFile( file, pose, d.stamp );

       //testQuaternionPrecision( d.pose );

        if( parser.iter() % stepIter == 0 )
            std::cout << "#"; std::flush( std::cout );
    }
    std::cout << std::endl;
    file.close();

	std::cout << "Average Proc. Time per frame:\t " << timeSum / iters << "ms" << std::endl;
	std::cout << "Number of Create Keyframes:\t "	<< vo.numOverallKeyframes() << std::endl;
}

void runBatch( VOParams& params, const Matrix3f& K, const String& folder, ConfigFile& cfg )
{
    String kftypeString = cfg.valueForName<String>( "keyframeType", "VO" );
    std::cout << "Keyframetype: " << kftypeString << std::endl;
    if( kftypeString.toUpper() == "VO" ){
        runVOWithKFType<VOKeyframe>( params, K, folder, cfg );
    } else if( kftypeString.toUpper() == "ESM" ){
        runVOWithKFType<ESMKeyframe>( params, K, folder, cfg );
    } else if( kftypeString.toUpper() == "AII" ) {
        runVOWithKFType<AIIKeyframe>( params, K, folder, cfg );
    } else if( kftypeString.toUpper() == "ROBUST_HUBER" ) {
		params.robustParam = cfg.valueForName( "huberThreshold", 0.1f );
    } else if( kftypeString.toUpper() == "ROBUST_TUKEY" ) {
		params.robustParam = cfg.valueForName( "tukeyThreshold", 0.3f );
        runVOWithKFType<RobustKeyframe<Tukey> >( params, K, folder, cfg );
    } else if( kftypeString.toUpper() == "MS_VO" ) {
        runVOWithKFType<MultiscaleKeyframe<VOKeyframe> >( params, K, folder, cfg );
    } else if( kftypeString.toUpper() == "MS_ESM" ) {
        runVOWithKFType<MultiscaleKeyframe<ESMKeyframe> >( params, K, folder, cfg );
    } else if( kftypeString.toUpper() == "MS_AII" ) {
        runVOWithKFType<MultiscaleKeyframe<AIIKeyframe> >( params, K, folder, cfg );
    } else if( kftypeString.toUpper() == "MS_ROBUST_HUBER" ) {
		params.robustParam = cfg.valueForName( "huberThreshold", 0.1f );
       runVOWithKFType<MultiscaleKeyframe<RobustKeyframe<Huber> > >( params, K, folder, cfg );
    } else if( kftypeString.toUpper() == "MS_ROBUST_TUKEY" ) {
		params.robustParam = cfg.valueForName( "tukeyThreshold", 0.3f );
       runVOWithKFType<MultiscaleKeyframe<RobustKeyframe<Tukey> > >( params, K, folder, cfg );
    } else {
		std::cout << "Unknown keyframe type" << std::endl;
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
    VOParams params;
    params.maxIters = cfg.valueForName( "maxIterations", 10 );
    params.gradientThreshold = cfg.valueForName( "gradientThreshold", 0.2f );
    params.depthScale = cfg.valueForName( "depthFactor", 5000.0f ) * cfg.valueForName( "depthScale", 1.0f );
    params.minParameterUpdate = cfg.valueForName( "minDeltaP", 0.0f );
    params.pyrScale = cfg.valueForName( "pyrScale", 0.5f );
    params.octaves = cfg.valueForName( "pyrOctaves", 3 );

    Matrix3f K;
    K.setIdentity();
    K[ 0 ][ 0 ] = 520.9f; K[ 0 ][ 2 ] = 325.1f;
    K[ 1 ][ 1 ] = 521.0f; K[ 1 ][ 2 ] = 249.7f;
  /*
  	runBatch( params, K, folder, cfg );
    cfg.save( "test.cfg" );
    return 0;
	*/

	std::cout << "Num Cams: " << OpenNICamera::count() << std::endl;
    RGBDVOApp app( folder, K, params );
    app.setMaxRotationDistance( cfg.valueForName( "maxRotationDist", 3.0f ) );
    app.setMaxTranslationDistance( cfg.valueForName( "maxTranslationDist", 0.3f ) );
    app.setMaxSSD( cfg.valueForName( "maxSSD", 0.2f ) );
    Application::run();


    return 0;
}
