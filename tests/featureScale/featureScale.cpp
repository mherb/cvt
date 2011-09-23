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


float centroidAngle( int x, int y, const float* iimgptr, size_t widthstep )
{
	static const int _circularoffset[ 31 ] = {  3,  6,  8,  9, 10, 11, 12, 13, 13, 14, 14, 14, 15, 15, 15, 15,
		15, 15, 15, 14, 14, 14, 13, 13, 12, 11, 10,  9,  8,  6,  3 };
	float mx = 0;
	float my = 0;

	int cury = ( int ) y - 15;
	int curx = ( int ) x;

	for( int i = 0; i < 15; i++ ) {
		mx +=( ( float ) i - 15.0f ) * ( IntegralImage::area( iimgptr, curx - _circularoffset[ i ], cury + i, 2 * _circularoffset[ i ] + 1, 1, widthstep )
										- IntegralImage::area( iimgptr, curx - _circularoffset[ i ], cury + 30 - i, 2 * _circularoffset[ i ] + 1, 1, widthstep ) );
	}

	cury = ( int ) y;
	curx = ( int ) x - 15;
	for( int i = 0; i < 15; i++ ) {
		my += ( ( float ) i - 15.0f ) * ( IntegralImage::area( iimgptr, curx + i, cury - _circularoffset[ i ], 1, 2 * _circularoffset[ i ] + 1, widthstep )
										 - IntegralImage::area( iimgptr, curx + 30 - i, cury - _circularoffset[ i ], 1, 2 * _circularoffset[ i ] + 1, widthstep ) );
	}

	return Math::rad2Deg( Math::atan2( my, mx ) );
}

void testRotations( const Image & gray )
{
	int x = gray.width() / 2;
	int y = gray.height() / 2;

	SIMD * simd = SIMD::instance();

	size_t stride;
	const uint8_t * p = gray.map( &stride );


	float xx, xy, yy, mx, my, angle;
	float score = simd->harrisResponseCircular1u8( xx, xy, yy, p + y * stride + x , stride, 0.04f );

	float c,s;
	Math::jacobi( c, s, xx, xy, yy );
	Matrix2f cov( xx, xy, xy, yy );
	Matrix2f u, d, vt;
	cov.svd( u, d, vt );
	//std::cout << u << "\n<->\n" << vt << std::endl;
	//std::cout << c << " " << s << std::endl;
	std::cout << mx << std::endl;
	std::cout << my << std::endl;
	std::cout << Math::sgn( xy ) << std::endl;
	std::cout << Math::rad2Deg( Math::atan2( vt[ 0 ][ 1 ], vt[ 0 ][ 0 ] ) ) << std::endl;
	//std::cout << Math::atan2( -vt[ 1 ][ 0 ], vt[ 0 ][ 0 ] ) << " <-> " << atan2( s, c ) << std::endl;
	//std::cout << Math::rad2Deg( Math::atan2( s, c ) )<< std::endl;
	//std::cout << Math::rad2Deg( Math::atan2( 2*c*s, c*c-s*s ) )<< std::endl;
	//std::cout << Math::rad2Deg( Math::atan2( yy, xx ) )<< std::endl;

	//vt = u * vt;
	//std::cout << Math::rad2Deg( Math::atan2( vt[ 1 ][ 0 ], vt[ 0 ][ 0 ] ) )<< std::endl;
	
	//it->svdangle = Math::TWO_PI - it->svdangle + Math::HALF_PI;
	//while( it->svdangle > Math::TWO_PI )
	//		it->svdangle -= Math::TWO_PI;

	gray.unmap( p );
    
	Image integral;
    gray.integralImage( integral );
	size_t istride;
	float * iip = integral.map<float>( &istride );
	std::cout << centroidAngle( x, y, iip, istride ) << std::endl;
	integral.unmap( iip );

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
	testRotations( warped );

	return 0;
}
