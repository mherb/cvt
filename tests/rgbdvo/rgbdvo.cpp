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

void writeConvergenceTestSampleToFile( std::ofstream& file,
                                       float initialROffset, float initialTOffset,
                                       float finalROffset, float finalTOffset,
                                       size_t iterations, float finalSSD, size_t numPixels )
{
    file.precision( 15 );
    file << std::fixed << initialROffset << " "
                       << initialTOffset << " "
                       << finalROffset << " "
                       << finalTOffset << " "
                       << iterations << " "
                       << finalSSD << " "
                       << numPixels << " "
                       << std::endl;
}

template <class KFType>
void convergenceTest( const VOParams& params, const Matrix3f& K, const String& folder, ConfigFile& cfg )
{
    RGBDParser parser( folder, 0.05f );

    // go to an image somewhere inside the dataset
    size_t iter = 0;
    while( iter++ < 100 )
        parser.loadNext();

    const RGBDParser::RGBDSample& d = parser.data();

    Image gray;
    d.rgb.convert( gray, IFormat::GRAY_FLOAT );

    Image dFloat;
    d.depth.convert( dFloat, IFormat::GRAY_FLOAT );

    Matrix4f poseMat;
    poseMat.setIdentity();

    KFType* keyframe = KFType::create( gray, dFloat, poseMat, K, params );
    PoseRepresentation relPose;
    relPose.bias = 0.0f;
    relPose.gain = 0.0f;

    const float angleRange = cfg.valueForName( "convTestRotoRangeDeg", 20.0f ) * Math::PI / 180.0f;
    const float transRange = cfg.valueForName( "convTestTransRangeM", 1.5f );
    Vector3f randVec3, v3;


    std::ofstream rotfile, transfile, rottransfile;
    rotfile.open( "rotation_convergence.txt" );
    transfile.open( "translation_convergence.txt" );
    rottransfile.open( "rotation_translation_convergence.txt" );

    // rotation test
    int numSamples = cfg.valueForName( "convTestSamples", 10000 );
    size_t dumpIter = numSamples / 100;

    std::cout << "Rotation convergence:\t\t";
    for( int i = 0; i < numSamples; i++ ){
        // generate random rotation offset:
        randVec3[ 0 ] = Math::rand( -angleRange, angleRange );
        randVec3[ 1 ] = Math::rand( -angleRange, angleRange );
        randVec3[ 2 ] = Math::rand( -angleRange, angleRange );
        relPose.pose.set( randVec3.x, randVec3.y, randVec3.z, 0.0f, 0.0f, 0.0f );

        VOResult result = keyframe->computeRelativePose( relPose, gray, K, params );
        EigenBridge::toCVT( poseMat, relPose.pose.transformation() );
        Vector3f vt( poseMat[ 0 ][ 3 ], poseMat[ 1 ][ 3 ], poseMat[ 2 ][ 3 ] );
        Quaternionf q( poseMat.toMatrix3() );
        Vector3f v( q.toEuler() );

        writeConvergenceTestSampleToFile( rotfile, randVec3.length(), 0.0f, v.length(), vt.length(), result.iterations, result.SSD, result.numPixels );
        if( i % dumpIter == 0 ){
            std::cout << "#"; std::flush( std::cout );
        }
    }
    std::cout << std::endl;

    std::cout << "Translation convergence:\t";
    for( int i = 0; i < numSamples; i++ ){
        // generate random rotation offset:
        randVec3[ 0 ] = Math::rand( -transRange, transRange );
        randVec3[ 1 ] = Math::rand( -transRange, transRange );
        randVec3[ 2 ] = Math::rand( -transRange, transRange );
        relPose.pose.set( 0.0f, 0.0f, 0.0f, randVec3.x, randVec3.y, randVec3.z );

        VOResult result = keyframe->computeRelativePose( relPose, gray, K, params );
        EigenBridge::toCVT( poseMat, relPose.pose.transformation() );
        Vector3f vt( poseMat[ 0 ][ 3 ], poseMat[ 1 ][ 3 ], poseMat[ 2 ][ 3 ] );
        Quaternionf q( poseMat.toMatrix3() );
        Vector3f v( q.toEuler() );

        writeConvergenceTestSampleToFile( transfile, 0.0f, randVec3.length(), v.length(), vt.length(), result.iterations, result.SSD, result.numPixels );

        if( i % dumpIter == 0 ){
            std::cout << "#"; std::flush( std::cout );
        }
    }
    std::cout << std::endl;

    std::cout << "RotoTranslation convergence:\t";
    for( int i = 0; i < numSamples; i++ ){
        // generate random rotation offset:
        v3[ 0 ] = Math::rand( -angleRange, angleRange );
        v3[ 1 ] = Math::rand( -angleRange, angleRange );
        v3[ 2 ] = Math::rand( -angleRange, angleRange );
        randVec3[ 0 ] = Math::rand( -transRange, transRange );
        randVec3[ 1 ] = Math::rand( -transRange, transRange );
        randVec3[ 2 ] = Math::rand( -transRange, transRange );
        relPose.pose.set( v3.x, v3.y, v3.z, randVec3.x, randVec3.y, randVec3.z );

        VOResult result = keyframe->computeRelativePose( relPose, gray, K, params );
        EigenBridge::toCVT( poseMat, relPose.pose.transformation() );
        Vector3f vt( poseMat[ 0 ][ 3 ], poseMat[ 1 ][ 3 ], poseMat[ 2 ][ 3 ] );
        Quaternionf q( poseMat.toMatrix3() );
        Vector3f v( q.toEuler() );

        writeConvergenceTestSampleToFile( rottransfile, v3.length(), randVec3.length(), v.length(), vt.length(), result.iterations, result.SSD, result.numPixels );
        if( i % dumpIter == 0 ){
            std::cout << "#"; std::flush( std::cout );
        }
    }
    std::cout << std::endl;
    delete keyframe;
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
    float timeSum = 0;

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

void convergenceAnalysis( VOParams& params, const Matrix3f& K, const String& folder, ConfigFile& cfg )
{
    String kftypeString = cfg.valueForName<String>( "keyframeType", "VO" );
    std::cout << "Keyframetype: " << kftypeString << std::endl;
    if( kftypeString.toUpper() == "VO" ){
        convergenceTest<VOKeyframe>( params, K, folder, cfg );
    } else if( kftypeString.toUpper() == "ESM" ){
        convergenceTest<ESMKeyframe>( params, K, folder, cfg );
    } else if( kftypeString.toUpper() == "AII" ) {
        convergenceTest<AIIKeyframe>( params, K, folder, cfg );
    } else if( kftypeString.toUpper() == "ROBUST_HUBER" ) {
        params.robustParam = cfg.valueForName( "huberThreshold", 0.1f );
        convergenceTest<RobustKeyframe<Huber> >( params, K, folder, cfg );
    } else if( kftypeString.toUpper() == "ROBUST_TUKEY" ) {
        params.robustParam = cfg.valueForName( "tukeyThreshold", 0.2 );
        convergenceTest<RobustKeyframe<Tukey> >( params, K, folder, cfg );
    } else if( kftypeString.toUpper() == "MS_VO" ) {
        convergenceTest<MultiscaleKeyframe<VOKeyframe> >( params, K, folder, cfg );
    } else if( kftypeString.toUpper() == "MS_ESM" ) {
        convergenceTest<MultiscaleKeyframe<ESMKeyframe> >( params, K, folder, cfg );
    } else if( kftypeString.toUpper() == "MS_AII" ) {
        convergenceTest<MultiscaleKeyframe<AIIKeyframe> >( params, K, folder, cfg );
    } else if( kftypeString.toUpper() == "MS_ROBUST_HUBER" ) {
        params.robustParam = cfg.valueForName( "huberThreshold", 0.1f );
       convergenceTest<MultiscaleKeyframe<RobustKeyframe<Huber> > >( params, K, folder, cfg );
    } else if( kftypeString.toUpper() == "MS_ROBUST_TUKEY" ) {
        params.robustParam = cfg.valueForName( "tukeyThreshold", 0.2f );
       convergenceTest<MultiscaleKeyframe<RobustKeyframe<Tukey> > >( params, K, folder, cfg );
    } else {
        std::cout << "Unknown keyframe type" << std::endl;
    }
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
        runVOWithKFType<RobustKeyframe<Huber> >( params, K, folder, cfg );
    } else if( kftypeString.toUpper() == "ROBUST_AII_HUBER" ) {
		params.robustParam = cfg.valueForName( "huberThreshold", 0.1f );
        runVOWithKFType<RobustAIIKeyframe<Huber> >( params, K, folder, cfg );
    } else if( kftypeString.toUpper() == "ROBUST_TUKEY" ) {
		params.robustParam = cfg.valueForName( "tukeyThreshold", 0.2f );
        runVOWithKFType<RobustKeyframe<Tukey> >( params, K, folder, cfg );
    } else if( kftypeString.toUpper() == "ROBUST_AII_TUKEY" ) {
		params.robustParam = cfg.valueForName( "tukeyThreshold", 0.2f );
        runVOWithKFType<RobustAIIKeyframe<Tukey> >( params, K, folder, cfg );
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
    } else if( kftypeString.toUpper() == "MS_ROBUST_AII_HUBER" ) {
		params.robustParam = cfg.valueForName( "huberThreshold", 0.1f );
       runVOWithKFType<MultiscaleKeyframe<RobustAIIKeyframe<Huber> > >( params, K, folder, cfg );
    } else if( kftypeString.toUpper() == "MS_ROBUST_AII_TUKEY" ) {
		params.robustParam = cfg.valueForName( "tukeyThreshold", 0.2f );
       runVOWithKFType<MultiscaleKeyframe<RobustAIIKeyframe<Tukey> > >( params, K, folder, cfg );
    } else {
		std::cout << "Unknown keyframe type" << std::endl;
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
    VOParams params;
    params.maxIters = cfg.valueForName( "maxIterations", 10 );
    params.gradientThreshold = cfg.valueForName( "gradientThreshold", 0.2f );
    params.depthScale = cfg.valueForName( "depthFactor", 5000.0f ) * cfg.valueForName( "depthScale", 1.0f );
    params.minParameterUpdate = cfg.valueForName( "minDeltaP", 0.0f );
    params.pyrScale = cfg.valueForName( "pyrScale", 0.5f );
    params.octaves = cfg.valueForName( "pyrOctaves", 3 );

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

  	runBatch( params, K, folder, cfg );
//    convergenceAnalysis( params, K, folder, cfg );
    cfg.save( "rgbdvo.cfg" );
    return 0;

/*
	std::cout << "Num Cams: " << OpenNICamera::count() << std::endl;
    RGBDVOApp app( folder, K, params );
    app.setMaxRotationDistance( cfg.valueForName( "maxRotationDist", 3.0f ) );
    app.setMaxTranslationDistance( cfg.valueForName( "maxTranslationDist", 0.3f ) );
    app.setMaxSSD( cfg.valueForName( "maxSSD", 0.2f ) );
    Application::run();
*/


    return 0;
}
