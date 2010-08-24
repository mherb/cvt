#include "calcflow.h"
#include <cvt/gfx/ifilter/ROFDenoise.h>
#include <cvt/math/Math.h>

#include <cvt/vision/Flow.h>

#include <iostream>
#include <highgui.h>

using namespace cvt;

#define I( img, x, y ) (*( ( float* ) ( img.data() + Math::clamp(y, (size_t)0, img.height()-1) * img.stride() + Math::clamp(x, (size_t)0,(img.width()-1)*img.channels()) * sizeof( float ) ) ) )
#define IFX( img, x, y ) (*( ( float* ) ( img.data() + Math::clamp(y, (size_t)0, img.height()-1) * img.stride() + ( Math::clamp(x, (size_t)0,img.width()-1) * 2 ) * sizeof( float ) ) ) )
#define IFY( img, x, y ) (*( ( float* ) ( img.data() + Math::clamp(y, (size_t)0, img.height()-1) * img.stride() + ( Math::clamp(x,(size_t)0,img.width()-1) * 2 + 1 ) * sizeof( float ) ) ) )

#define WS 5

void flowsearch( Image& flowout, Image& flowin, Image& flowin2, Image& img1, Image& img2, float theta )
{
	size_t x, y;
	ssize_t lx, ly;
	flowout.reallocate( flowin.width(), flowin.height(), CVT_GRAYALPHA, CVT_FLOAT );

	for( y = 0; y < img1.height(); y++ ) {
		for( x = 0; x < img1.width(); x++ ) {
			float cost = 1e15f;
			float fin[ 2 ];
			float fin2[ 2 ];
			float fout[ 2 ];
			float i1[ 4 ];

			fout[ 0 ] = fin[ 0 ] = IFX( flowin, x, y );
			fout[ 1 ] = fin[ 1 ] = IFY( flowin, x, y );
			fin2[ 0 ] = IFX( flowin2, x, y );
			fin2[ 1 ] = IFY( flowin2, x, y );

			for( int i = 0; i < img1.channels(); i++ )
				i1[ i ] = I( img1, x * img1.channels() + i, y );

			for( ly = -WS; ly <= WS; ly++ ) {
				for( lx = -WS; lx <= WS; lx++ ) {
					float c;
					c = ( Math::sqr( fin[ 0 ] + ( float ) lx - fin2[ 0 ] ) + Math::sqr( fin[ 1 ] + ( float ) ly - fin2[ 1 ] ) ) / ( 2.0f * theta );

					for( int i = 0; i < img2.channels(); i++ )
						c += Math::abs( i1[ i ] - I( img2, ( x + ( size_t ) fin[ 0 ] + lx ) * img2.channels() + i, ( y + ( size_t ) fin[ 1 ] + ly ) ) );
					//std::cout << "Cost: " << c << std::endl;
					if( c < cost ) {
						cost = c;
						fout[ 0 ] = fin[ 0 ] + ( float ) lx;
						fout[ 1 ] = fin[ 1 ] + ( float ) ly;
						//std::cout << "Found: " << fout[ 0 ] << " " << fout[ 1 ] << std::endl;
					}
				}
			}
			IFX( flowout, x, y ) = fout[ 0 ];
			IFY( flowout, x, y ) = fout[ 1 ];
		}
	}
}

void flowsearch2( Image& flowout, Image& flowin, Image& flowin2, Image& img1, Image& img2, float theta )
{
	size_t x, y;
	float lx, ly;
	flowout.reallocate( flowin.width(), flowin.height(), CVT_GRAYALPHA, CVT_FLOAT );

	for( y = 0; y < img1.height(); y++ ) {
		for( x = 0; x < img1.width(); x++ ) {
			float cost = 1e15f;
			Color fin;
			Color fin2;
			float fout[ 2 ];
			Color i1;

			fin = flowin( ( int )x,(int) y );
			fin = flowin( ( int )x,(int) y );
			fin2 = flowin2( (int)x, (int)y );
			fin2 = flowin2( (int)x, (int)y );

			i1 = img1( (int)x, (int)y );

			for( ly = -WS; ly <= WS; ly += 0.5f ) {
				for( lx = -WS; lx <= WS; lx += 0.5f ) {
					float c;
					c = ( Math::sqr( fin.gray() + lx - fin2.gray() ) + Math::sqr( fin.alpha() + ly - fin2.alpha() ) ) / ( 2.0f * theta );

					Color i2 = img2( ( float ) x + fin.gray() + lx, ( float ) y + fin.alpha() + ly );
					Color diff = i1;
				    diff - i2;
					c += Math::abs( diff.red() );
					c += Math::abs( diff.green() );
					c += Math::abs( diff.blue() );
					//std::cout << "Cost: " << c << std::endl;
					if( c < cost ) {
						cost = c;
						fout[ 0 ] = fin.gray() + lx;
						fout[ 1 ] = fin.alpha() + ly;
						//std::cout << "Found: " << fout[ 0 ] << " " << fout[ 1 ] << std::endl;
					}
				}
			}
			IFX( flowout, x, y ) = fout[ 0 ];
			IFY( flowout, x, y ) = fout[ 1 ];
		}
	}
}

void calcflow( Image& flow, Image& img1, Image& img2 )
{
	size_t iter;
	float theta = 1.0f;
	float lambda = 50.0f;
	ROFDenoise rof;
	Image tmpflow;
	Image tmpflow2;

	flow.reallocate( img1.width(), img1.height(), CVT_GRAYALPHA, CVT_FLOAT );
	tmpflow2.reallocate( img1.width(), img1.height(), CVT_GRAYALPHA, CVT_FLOAT );

	iter = 40;
	flow.fill( Color( 0.0f, 0.0f ) );
	tmpflow2.fill( Color( 0.0f, 0.0f ) );

	while( iter-- ) {
		flowsearch2( tmpflow, flow, tmpflow2, img1, img2, theta * lambda );
		rof.apply( tmpflow2, tmpflow, 0.2f + theta * 0.6f, 30 );
		flow = Math::mix( tmpflow, tmpflow2, 0.2f );
		{
			Image x;
			Flow::colorCode( x, flow );
			cvShowImage( "Iter", x.iplimage() );
			Flow::colorCode( x, tmpflow2 );
			cvShowImage( "Iter2", x.iplimage() );
			cvWaitKey( 15 );
			std::cout << "Iter: " << iter << " Theta: " << theta << std::endl;
		}
		theta *= 0.8f; //20.0f - 10.0f * ( ( 50 - iter ) / 50.0f );
	}
}
