#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/vision/FAST.h>
#include <cvt/vision/Feature2D.h>
#include <cvt/vision/IntegralImage.h>
#include <cvt/io/Resources.h>
#include <cvt/gfx/ifilter/ITransform.h>

using namespace cvt;

void testFeatureScales( const Image & gray )
{
    FAST fast;
    fast.setThreshold( 35 );

    Image integral;
    gray.integralImage( integral );


    std::vector<Feature2Df> features;
    VectorFeature2DInserter<float> inserter( features );
    fast.extract( gray, inserter );

    Recti smallRect, bigRect;
    size_t offsetSmall = 3, offsetBig = 8;
    smallRect.height = 7;
    smallRect.width = 7;
    bigRect.width = 17;
    bigRect.height = 17;

    float avgScale = 0;
    size_t num = 0;
    for( size_t i = 0; i < features.size(); i++ ){
        const Feature2Df & f = features[ i ];
        if( f.pt.x > offsetBig && f.pt.x < gray.width() - offsetBig &&
            f.pt.y > offsetBig && f.pt.y < gray.height() - offsetBig ){
            smallRect.x = ( size_t )f.pt.x - offsetSmall;
            smallRect.y = ( size_t )f.pt.y - offsetSmall;
            bigRect.x = ( size_t )f.pt.x - offsetBig;
            bigRect.y = ( size_t )f.pt.y - offsetBig;
            float areaSmall = IntegralImage::area( integral, smallRect );
            float areaBig = IntegralImage::area( integral, bigRect );

            float scale = areaSmall / areaBig;
            avgScale += scale;
            num++;

            //std::cout << "Point " << i << ":\t" << scale << std::endl;
        }
    }
    std::cout << "Avg Scale: " << avgScale / num << std::endl;
}

void testRotations( const Image & gray )
{
}

void rotationHomography( Matrix3f & R, float angle, float tx, float ty )
{
	float s = Math::sin( angle );	
	float c = Math::cos( angle );

	R[ 0 ][ 0 ] = c; R[ 0 ][ 1 ] = -s; R[ 0 ][ 2 ] =  s * ty - c * tx + tx;	
	R[ 1 ][ 0 ] = s; R[ 1 ][ 1 ] =  c; R[ 1 ][ 2 ] = -c * ty - s * tx + ty;
	R[ 2 ][ 0 ] = R[ 2 ][ 1 ] = 0.0f;  R[ 2 ][ 2 ] =  1.0f;	
}

int main( int argc, char* argv[] )
{
    if( argc < 2 ){
        std::cout << "Usage: " << argv[ 0 ] << "<angle>" << std::endl;
        return 0;
    }

	Resources r;
    Image gray( r.find( "corner.png" ) );

	float angle = Math::deg2Rad( atof( argv[ 1 ] ) );

	Matrix3f H;
	rotationHomography( H, angle, gray.width() / 2.0f, gray.height() / 2.0f );
	Image warped( gray.width(), gray.height(), gray.format() );
	warped.fill( Color::WHITE );
	ITransform::apply( warped, gray, H );

	warped.save( "test.png" );

    //testFeatureScales( gray );
	//testRotations( gray );

	return 0;
}
