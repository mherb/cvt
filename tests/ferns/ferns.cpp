#include <iostream>

#include <cvt/vision/PatchGenerator.h>
#include <cvt/math/Math.h>
#include <cvt/gfx/Image.h>

#include <cvt/io/Resources.h>
#include <cvt/io/ImageIO.h>
#include <Eigen/Core>

#include <cvt/util/RNG.h>
#include <cvt/vision/Ferns.h>
#include <cvt/vision/FAST.h>

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

void testFerns()
{
	Resources res;	
	Image img;
	std::string fileName = res.find( "lena.png" );
	ImageIO::loadPNG( img, fileName );
	Image gray( img.width(), img.height(), IFormat::GRAY_UINT8 );
	img.convert( gray );
	
	/* patchSize, numOverallTests, numClasses */	
	Ferns ferns( 31, 330, 30 );	
	ferns.train( gray );
	
	std::vector<Feature2D> features;		
	FeatureExtractor<int32_t> * fe = new FAST( SEGMENT_10 );
	static_cast<FAST*>(fe)->setMinScore( 80 );
	static_cast<FAST*>(fe)->setThreshold( 40 );
	fe->extractMultiScale( gray, features, 3 );
	
	std::cout << "NUMBER OF FEATURES: " << features.size() << std::endl;
	
	Eigen::Vector2i testPoint, bestPoint;
	
	float prob;
	
	for( size_t i = 0; i < features.size(); i++ ){
		testPoint[ 0 ] = features[ i ][ 0 ];
		testPoint[ 1 ] = features[ i ][ 1 ];
		
		prob = ferns.classify( bestPoint, gray, testPoint );
		
		std::cout << "Test: ( " << testPoint[ 0 ] << ", " << testPoint[ 1 ] << ", Match: ( " << bestPoint[ 0 ] << ", " << bestPoint[ 1 ] << " ) , Prob: " << prob << std::endl; 
	}
}

int main()
{
	//testRNG();
	//testPatchGen();
	
	testFerns();
	
	return 0;	
}