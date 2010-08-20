#include "calcflow.h"
#include <cvt/gfx/ifilter/ROFDenoise.h>
#include <cvt/math/Math.h>

#include <cvt/vision/Flow.h>

#include <iostream>
#include <highgui.h>

using namespace cvt;

#define I( img, x, y ) (*( ( float* ) ( img.data() + Math::clamp(y, (size_t)0, img.height()-1) * img.stride() + Math::clamp(x, (size_t)0,img.width()-1) * sizeof( float ) ) ) )
#define IFX( img, x, y ) (*( ( float* ) ( img.data() + Math::clamp(y, (size_t)0, img.height()-1) * img.stride() + ( Math::clamp(x, (size_t)0,img.width()-1) * 2 ) * sizeof( float ) ) ) )
#define IFY( img, x, y ) (*( ( float* ) ( img.data() + Math::clamp(y, (size_t)0, img.height()-1) * img.stride() + ( Math::clamp(x,(size_t)0,img.width()-1) * 2 + 1 ) * sizeof( float ) ) ) )

#define WS 2

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
			float i1;

			fout[ 0 ] = fin[ 0 ] = IFX( flowin, x, y );
			fout[ 1 ] = fin[ 1 ] = IFY( flowin, x, y );
			fin2[ 0 ] = IFX( flowin2, x, y );
			fin2[ 1 ] = IFY( flowin2, x, y );

			i1 = I( img1, x, y );

			for( ly = -WS; ly <= WS; ly++ ) {
				for( lx = -WS; lx <= WS; lx++ ) {
					float c;
					c = ( Math::sqr( fin[ 0 ] + ( float ) lx - fin2[ 0 ] ) + Math::sqr( fin[ 1 ] + ( float ) ly - fin2[ 1 ] ) ) / ( 2.0f * theta );
					c += Math::abs( i1 - I( img2, ( x + ( size_t ) fin[ 0 ] + lx ), ( y + ( size_t ) fin[ 1 ] + ly ) ) );
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

void calcflow( Image& flow, Image& img1, Image& img2 )
{
	size_t iter;
	float theta = 20.0f;
	ROFDenoise rof;
	Image tmpflow;
	Image tmpflow2;

	flow.reallocate( img1.width(), img1.height(), CVT_GRAYALPHA, CVT_FLOAT );
	tmpflow2.reallocate( img1.width(), img1.height(), CVT_GRAYALPHA, CVT_FLOAT );

	iter = 50;
	flow.fill( Color( 0.0f, 0.0f ) );
	tmpflow2.fill( Color( 0.0f, 0.0f ) );
	while( iter-- ) {
		flowsearch( tmpflow, flow, tmpflow2, img1, img2, theta );
		rof.apply( tmpflow2, tmpflow, 0.5f, 10 );
		flow.copy( tmpflow );
		{
			Image x;
			Flow::colorCode( x, flow );
			cvShowImage( "Iter", x.iplimage() );
			Flow::colorCode( x, tmpflow );
			cvShowImage( "Iter2", x.iplimage() );
			cvWaitKey( 25 );
			std::cout << "Iter: " << iter << " Theta: " << theta << std::endl;
		}
		theta *= 0.9f; //20.0f - 10.0f * ( ( 50 - iter ) / 50.0f );
	}
}
