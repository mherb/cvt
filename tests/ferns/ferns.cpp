#include <iostream>

#include <cvt/vision/PatchGenerator.h>
#include <cvt/math/Math.h>
#include <cvt/gfx/Image.h>

#include <cvt/io/Resources.h>
#include <cvt/io/ImageIO.h>
#include <Eigen/Core>
#include <Eigen/Geometry>

#include <cvt/util/RNG.h>
#include <cvt/vision/Ferns.h>
#include <cvt/vision/FAST.h>

#include <cvt/vision/DLT.h>

#include <cvt/gfx/IFilterVector.h>
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
	
	uint32_t patchSize = 31;
	PatchGenerator gen( rotRange, sRange, patchSize );
	
	Resources res;	
	Image img;
	std::string fileName = res.find( "lena.png" );
	ImageIO::loadPNG( img, fileName );
	Image gray( img.width(), img.height(), IFormat::GRAY_UINT8 );
	img.convert( gray );
	
	Image patch( patchSize, patchSize, gray.format() );
	
	uint32_t numPatches = 10;
	char buf[ 255 ];
	Eigen::Vector2i p;
	p[ 0 ] = gray.width() / 2;
	p[ 1 ] = gray.height() / 2;
	
	for( size_t i = 0; i < numPatches; i++ ){
		gen.next( patch, gray, p );
		
		sprintf( buf, "patch_%03zu.png", i );
		ImageIO::savePNG( patch, buf );
	}	
}

static IFilterVector8 calc_homography( float theta, float phi, float sx, float sy, float tx, float ty, float v1, float v2 )
{
	Eigen::Matrix3f h, ih;
	Eigen::Transform<float,2> affine;
	IFilterVector8 ret;
	
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
	
	std::cout << h << std::endl;
	ih = h.inverse();
	std::cout << "DET " <<ih.determinant() << std::endl;
	ih /= powf( ih.determinant(), 1.0f / 3.0f );
	std::cout << "DET " <<ih.determinant() << std::endl;
	std::cout << ih << std::endl;
	
	ret[ 0 ] = ih( 0, 0 );
	ret[ 1 ] = ih( 0, 1 );
	ret[ 2 ] = ih( 0, 2 );
	ret[ 3 ] = ih( 1, 0 );
	ret[ 4 ] = ih( 1, 1 );
	ret[ 5 ] = ih( 1, 2 );
	ret[ 6 ] = ih( 2, 0 );
	ret[ 7 ] = ih( 2, 1 );
	return ret;
}

void testFerns()
{
	Resources res;	
	Image img;
	std::string fileName = res.find( "lena.png" );
	ImageIO::loadPNG( img, fileName );
	
	Image gray( img.width(), img.height(), IFormat::GRAY_UINT8 );
	Image grayf( img.width(), img.height(), IFormat::GRAY_FLOAT );
	
	img.convert( gray );
	gray.convert( grayf );
	
	/* patchSize, numOverallTests, testsPerClass */	
	/*Ferns ferns( 51, 550, 50 );	
	ferns.train( gray );
	ferns.save( "lena.ferns" );*/
	
	std::cout << "Loading ferns ... ";
	Ferns ferns( "lena.ferns" );
	std::cout << "done" << std::endl;
	
	Image warpedf( img.width(), img.height(), IFormat::GRAY_FLOAT );
	Image warped( img.width(), img.height(), IFormat::GRAY_UINT8 );
	
	Homography hfilter;
	IFilterVector8 H = calc_homography( 0.0f, 0.0f, 0.5f, 0.5f, 100.0f, 100.0f, 0.001f, 0.0f );
	Color black( 0.0f, 0.0f, 0.0f, 1.0f );
	hfilter.apply( warpedf, grayf, H, black );
	warpedf.convert( warped );
	
	std::vector<Feature2D> features;		
	FeatureExtractor<int32_t> * fe = new FAST( SEGMENT_10 );
	static_cast<FAST*>(fe)->setMinScore( 50 );
	static_cast<FAST*>(fe)->setThreshold( 30 );
	fe->extractMultiScale( warped, features, 3 );
	
	std::cout << "NUMBER OF FEATURES: " << features.size() << std::endl;
	
	Eigen::Vector2i testPoint, bestPoint;
	
	Eigen::Matrix3d homMat = Eigen::Matrix3d::Identity();
	/*homMat( 0, 0 ) = H[ 0 ];	homMat( 0, 1 ) = H[ 1 ];	homMat( 0, 2 ) = H[ 2 ];
	homMat( 1, 0 ) = H[ 3 ];	homMat( 1, 1 ) = H[ 4 ];	homMat( 1, 2 ) = H[ 5 ];
	homMat( 2, 0 ) = H[ 6 ];	homMat( 2, 1 ) = H[ 7 ];	homMat( 2, 2 ) = 1.0;*/
	
	Eigen::Vector3d p, pp;
	
	float prob;
	
	std::vector<Eigen::Vector2d> reference, imageFeatures;
	Eigen::Matrix3d H_calculated;
	Eigen::Vector2d tmpP;
	
	for( size_t i = 0; i < features.size(); i++ ){
		testPoint[ 0 ] = features[ i ][ 0 ];
		testPoint[ 1 ] = features[ i ][ 1 ];
				
		prob = ferns.classify( bestPoint, warped, testPoint );
		
		p[ 0 ] = bestPoint[ 0 ];
		p[ 1 ] = bestPoint[ 1 ];
		p[ 2 ] = 1.0;
		
		pp = homMat * p;
		
		if( prob < 0.0 ) {
			tmpP[ 0 ] = testPoint[ 0 ];
			tmpP[ 1 ] = testPoint[ 1 ];
			imageFeatures.push_back( tmpP );
			
			tmpP[ 0 ] = bestPoint[ 0 ];
			tmpP[ 1 ] = bestPoint[ 1 ];
			reference.push_back( tmpP );
			
			std::cout << "\nPOINTS:" << std::endl;
			std::cout << "P: " << testPoint[ 0 ] << ", " << testPoint[ 1 ] << std::endl;
			std::cout << "M: " << pp[ 0 ] / pp[ 2 ] << ", " << pp[ 1 ] / pp[ 2 ] << std::endl;
			std::cout << "Prob: " << prob << std::endl;			
		}		
	}
	
	DLT::ndlt( reference, imageFeatures, H_calculated );
	std::cout << "Reference homography:\n" << homMat << std::endl; 
	std::cout << "Computed homography: \n" << H_calculated << std::endl;
}

int main()
{
	//testRNG();
	//testPatchGen();
	
	testFerns();
	
	return 0;	
}