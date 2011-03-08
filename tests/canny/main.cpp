#include <cvt/gfx/Image.h>
#include <cvt/io/ImageIO.h>
#include <cvt/io/Resources.h>

using namespace cvt;

void canny( Image& out, const Image& in )
{
	Image dx( in.width(), in.height(), IFormat::GRAY_FLOAT );
	Image dy( in.width(), in.height(), IFormat::GRAY_FLOAT );
	Image dir( in.width(), in.height(), IFormat::GRAY_UINT8 );

	in.convolve( dx, IKernel::FIVEPOINT_DERIVATIVE_HORIZONTAL, IKernel::GAUSS_VERTICAL_5 );
	in.convolve( dy, IKernel::GAUSS_HORIZONTAL_5, IKernel::FIVEPOINT_DERIVATIVE_VERTICAL );

	out.reallocate( in.width(), in.height(), IFormat::GRAY_FLOAT );

	uint8_t* pdx;
	uint8_t* pdy;
	uint8_t* pdst;
	uint8_t* pdir;
	size_t stridedx, stridedy, stridedst, stridedir;
	pdx = dx.map<uint8_t>( &stridedx );
	pdy = dy.map<uint8_t>( &stridedy );
	pdst = out.map<uint8_t>( &stridedst );
	pdir = dir.map<uint8_t>( &stridedir );

	size_t h = out.height();
	uint8_t* cdx = pdx;
	uint8_t* cdy = pdy;
	uint8_t* cdst = pdst;
	uint8_t* cdir = pdir;
	while( h-- ) {
		size_t w = out.width();
		float* fdx =  ( float* ) cdx;
		float* fdy =  ( float* ) cdy;
		float* fdst =  ( float* ) cdst;
		uint8_t* udir = cdir;
		while( w-- ) {
			*fdst = Math::sqrt( Math::sqr( *fdx ) + Math::sqr( *fdy ) );
			float m = Math::abs( *fdy ) / Math::abs( *fdx );
			if( m >= 4.5107f ) {
				*udir++ = 1;
			} else if( m >= 0.41421f ) {
				*udir++ = *fdx * *fdy > 0 ? 2 : 3;
			} else {
				*udir++ = 0;
			}
			fdst++;
			fdx++;
			fdy++;
		}
		cdx += stridedx;
		cdy += stridedy;
		cdst += stridedst;
		cdir += stridedir;
	}

	h = out.height() - 2;
	cdst = pdst + stridedst;
	cdir = pdir + stridedir;
	while( h-- ) {
		size_t w = out.width() - 2;
		float* fdst =  ( ( float* ) cdst ) + 1;
		uint8_t* udir = cdir + 1;
		while( w-- ) {
			if( *udir == 0 ) {
				if( *(fdst - 1 ) >= *fdst ||
				   *( fdst + 1 ) >= *fdst )
					*fdst = 0;
			} else if( *udir == 1 ) {
				if( *( ( float* )( ( uint8_t* ) fdst - stridedst  ) ) >= *fdst ||
				   *( ( float* )( ( uint8_t* ) fdst + stridedst  ) ) >= *fdst )
					*fdst = 0;
			} else if( *udir == 3 ) {
				if( *( ( float* )( ( uint8_t* ) ( fdst + 1 ) - stridedst  ) ) >= *fdst ||
				   *( ( float* )( ( uint8_t* ) ( fdst - 1 ) + stridedst  ) ) >= *fdst )
					*fdst = 0;

			} else if( *udir == 2 ) {
				if( *( ( float* )( ( uint8_t* ) ( fdst - 1 ) - stridedst  ) ) >= *fdst ||
				   *( ( float* )( ( uint8_t* ) ( fdst + 1 ) + stridedst  ) ) >= *fdst )
					*fdst = 0;
			}
			*fdst *= 20.0f;
			fdst++;
			udir++;
		}
		cdst += stridedst;
		cdir += stridedir;
	}

	dx.unmap( pdx );
	dy.unmap( pdy );
	out.unmap( pdst );
}


int main()
{
	Image img, out;

	Resources r;
	ImageIO::loadPNG( img, r.find( "boss.png" ) );
	Image imgf( img.width(), img.height(), IFormat::GRAY_FLOAT );
	img.convert( imgf );

	canny( out, imgf );

	ImageIO::savePNG( out, "canny.png" );

}
