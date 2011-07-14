#include <iostream>

#include <cvt/vision/PatchGenerator.h>
#include <cvt/math/Math.h>
#include <cvt/gfx/Image.h>

#include <cvt/io/Resources.h>
#include <Eigen/Core>
#include <Eigen/Geometry>

#include <cvt/util/RNG.h>
#include <cvt/vision/Ferns.h>
#include <cvt/vision/FAST.h>

#include <cvt/vision/DLT.h>

#include <cvt/gfx/ifilter/Homography.h>

using namespace cvt;

void testRNG()
{
	RNG rng( time( NULL ) );

	float val;

	for( size_t i = 0; i < 1000; i++ ){
		val = rng.fl();
		if( val > 1.0 || val < 0.0 )
			std::cout << "ERROR: FLOAT: " << val << std::endl;

		val = rng.uniform( -300.0f, 300.0f );
		if( val > 300.0f || val < -300.0f )
			std::cout << "ERROR: uniform FLOAT: " << val << std::endl;
	}

	int intVal;
	for( size_t i = 0; i < 10000; i++ ){
		intVal = rng.uniform( 0, 72 );

		if( intVal < 0 || intVal > 200 )
			std::cout << "ERROR IN RNG: " << intVal << std::endl;
	}
}

void testPatchGen()
{
	Rangef sRange( 0.6f, 1.5f );
	Rangef rotRange( 0.0f, Math::TWO_PI );

	uint32_t patchSize = 81;
	PatchGenerator gen( rotRange, sRange, patchSize );

	Resources res;
	Image img;
    String fileName = res.find( "lena.png" );
	img.load( fileName.c_str() );
	Image gray( img.width(), img.height(), IFormat::GRAY_UINT8 );
	img.convert( gray );

	Image patch( patchSize, patchSize, gray.format() );

	uint32_t numPatches = 3;
	char buf[ 255 ];
	Vector2f p;
	p[ 0 ] = gray.width() / 2;
	p[ 1 ] = gray.height() / 2;

	for( size_t i = 0; i < numPatches; i++ ){
		gen.next( patch, gray, p );

		sprintf( buf, "patch_%03zu.png", i );
		patch.save( buf );
	}
}

static cvt::Matrix3f calc_homography( float theta, float phi, float sx, float sy, float tx, float ty, float v1, float v2 )
{
	Eigen::Matrix3f h, ih;
	Eigen::Transform<float,2> affine;

	affine = Eigen::Rotation2D<float>( Math::deg2Rad( phi ) );
	affine = Eigen::Scaling2f( sx, sy ) * affine;
	affine = Eigen::Rotation2D<float>( Math::deg2Rad( -phi ) ) * affine;
	affine = Eigen::Rotation2D<float>( Math::deg2Rad( theta ) ) * affine;

	h( 0, 0 ) = affine( 0, 0 );
	h( 0, 1 ) = affine( 0, 1 );
	h( 1, 0 ) = affine( 1, 0 );
	h( 1, 1 ) = affine( 1, 1 );
	h( 0, 2 ) = tx;
	h( 1, 2 ) = ty;
	h( 2, 0 ) = v1;
	h( 2, 1 ) = v2;
	h( 2, 2 ) = 1.0f;

	ih = h.inverse();
	ih /= powf( ih.determinant(), 1.0f / 3.0f );
	ih.transposeInPlace(); // hack: Eigen by default stores column major order

	return *( ( cvt::Matrix3f* )ih.data() );
}

void testFerns()
{
	Resources res;
	Image img;
	String fileName = res.find( "lena.png" );
	img.load( fileName.c_str() );

	Image gray( img.width(), img.height(), IFormat::GRAY_UINT8 );
	Image grayf( img.width(), img.height(), IFormat::GRAY_FLOAT );

	img.convert( gray );
	gray.convert( grayf );

	/* patchSize, numOverallTests, testsPerClass */
	/*
	Ferns ferns( 35, 440, 40 );
	ferns.train( gray );
	ferns.save( "lena.ferns" );
	 */

	std::cout << "Loading ferns ... ";
	std::flush( std::cout );
	Ferns ferns( "lena.ferns" );
	std::cout << "done" << std::endl;

	Image warpedf( img.width(), img.height(), IFormat::GRAY_FLOAT );
	Image warped( img.width(), img.height(), IFormat::GRAY_UINT8 );

	Homography hfilter;
	cvt::Matrix3f H = calc_homography( 25.0f, 0.0f, 1.3f, 1.3f, 100.0f, 100.0f, 0.0f, 0.0f );
	H *= 1.0f / H[ 2 ][ 2 ];
	Color black( 0.0f, 0.0f, 0.0f, 1.0f );

	hfilter.apply( warpedf, grayf, H, black );
	warpedf.convert( warped );
	warped.save( "test.png" );

	std::vector<Feature2Df> features;
    FAST * fe = new FAST( SEGMENT_9 );
	fe->setThreshold( 30 );

    VectorFeature2DInserter<float> inserter( features );
	fe->extract( warped, inserter );

	std::vector<Eigen::Vector2d> reference, imageFeatures;
	std::vector<Eigen::Vector2i> featurePoints;

	Eigen::Vector2i tmp;
	for( size_t i = 0; i < features.size(); i++ ){
		tmp[ 0 ] = features[ i ].pt.x;
		tmp[ 1 ] = features[ i ].pt.y;
		featurePoints.push_back( tmp );
	}

	ferns.match( featurePoints, warped, reference, imageFeatures );

	std::cout << "Matched points: \n";

	cvt::Vector3f pp, p;
	for( size_t i = 0; i < reference.size(); i++ ){
		pp[ 0 ] = imageFeatures[ i ][ 0 ];
		pp[ 1 ] = imageFeatures[ i ][ 1 ];
		pp[ 2 ] = 1.0;
		p = H * pp;
		double diff = fabs( reference[ i ][ 0 ] - p[ 0 ] / p[ 2 ] );
		diff+= fabs( reference[ i ][ 1 ] - p[ 1 ] / p[ 2 ] );
		std::cout << "Model: " << reference[ i ][ 0 ] << ", " << reference[ i ][ 1 ];
		std::cout << " Associated image point: " << imageFeatures[ i ][ 0 ] << ", " << imageFeatures[ i ][ 1 ];
		std::cout << " Image backproj. with GT: " << p[ 0 ] / p[ 2 ] << ", " << p[ 1 ] / p[ 2 ];
		if(  diff > 8.0 ){
			std::cout << ": bad " << std::endl;
		} else {
			std::cout << ": good " << std::endl;
		}
	}

	Eigen::Matrix3d H_calculated;
	DLT::ndlt( reference, imageFeatures, H_calculated );

	std::cout << "Reference homography:\n" << H.inverse() << std::endl;
	std::cout << "Computed homography: \n" << H_calculated << std::endl;

}

int main()
{
	//testRNG();
	//testPatchGen();

	testFerns();

	return 0;
}
