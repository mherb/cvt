#include <cvt/gfx/Image.h>
#include <cvt/io/ImageIO.h>
#include <cvt/io/Resources.h>
#include <cvt/util/Stack.h>

using namespace cvt;

void canny( Image& out, const Image& in, float low = 0.05f, float high = 0.10f )
{
	Image dx( in.width(), in.height(), IFormat::GRAY_FLOAT );
	Image dy( in.width(), in.height(), IFormat::GRAY_FLOAT );
	Image dir( in.width(), in.height(), IFormat::GRAYALPHA_UINT8 );

	in.convolve( dx, IKernel::HAAR_HORIZONTAL_3, IKernel::GAUSS_VERTICAL_5 );
	in.convolve( dy, IKernel::GAUSS_HORIZONTAL_5, IKernel::HAAR_VERTICAL_3 );

	out.reallocate( in.width(), in.height(), IFormat::GRAY_FLOAT );

#define HORIZONTAL	0
#define VERTICAL	1
#define DIAGUP		2
#define DIAGDOWN	3
#define NOEDGE 1
#define PEDGE 0
#define EDGE 2

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
			if( m >= 2.4142f ) {
				*udir++ = VERTICAL;
			} else if( m >= 0.41421f ) {
				*udir++ = *fdx * *fdy > 0 ? DIAGUP : DIAGDOWN;
			} else {
				*udir++ = HORIZONTAL;
			}
			if( *fdst >= high )
				*udir++ = EDGE;
			else if( *fdst >= low )
				*udir++ = PEDGE;
			else
				*udir++ = NOEDGE;
			fdst++;
			fdx++;
			fdy++;
		}
		cdx += stridedx;
		cdy += stridedy;
		cdst += stridedst;
		cdir += stridedir;
	}


	Stack<uint8_t*> stack( 1024 );
	h = out.height() - 2;
	memset( pdir, NOEDGE, 2 * dir.width() );
	memset( pdir + ( dir.height() - 1 ) * stridedir, NOEDGE, 2 * dir.width() );
	cdst = pdst + stridedst;
	cdir = pdir + stridedir;
	while( h-- ) {
		size_t w = out.width() - 2;
		float* fdst =  ( ( float* ) cdst ) + 1;
		uint8_t* udir = cdir + 2;
		*( cdir + 1 ) = NOEDGE;
		*( cdir + 2 * ( dir.width() - 1 ) + 1 ) = NOEDGE;
		while( w-- ) {
			if( *udir == HORIZONTAL ) {
				if( *(fdst - 1 ) > *fdst ||
				   *( fdst + 1 ) > *fdst )
					*( udir + 1 ) = NOEDGE;
			} else if( *udir == VERTICAL ) {
				if( *( ( float* )( ( uint8_t* ) fdst - stridedst  ) ) > *fdst ||
				   *( ( float* )( ( uint8_t* ) fdst + stridedst  ) ) > *fdst )
					*( udir + 1 ) = NOEDGE;
			} else if( *udir == DIAGDOWN ) {
				if( *( ( float* )( ( uint8_t* ) ( fdst + 1 ) - stridedst  ) ) > *fdst ||
				   *( ( float* )( ( uint8_t* ) ( fdst - 1 ) + stridedst  ) ) > *fdst )
					*( udir + 1 ) = NOEDGE;
			} else if( *udir == DIAGUP ) {
				if( *( ( float* )( ( uint8_t* ) ( fdst - 1 ) - stridedst  ) ) > *fdst ||
				   *( ( float* )( ( uint8_t* ) ( fdst + 1 ) + stridedst  ) ) > *fdst )
					*( udir + 1 ) = NOEDGE;
			}
			if( *( udir + 1 ) == EDGE )
				stack.push( udir + 1 );
			fdst++;
			udir += 2;
		}
		cdst += stridedst;
		cdir += stridedir;
	}

	while( !stack.isEmpty() ) {
		uint8_t* m;
		m = stack.pop();
		*m = EDGE;
		if( !m[ - 2 ] )
			stack.push( m - 2 );
		if( !m[ + 2 ] )
			stack.push( m + 2 );
		if( !m[ - stridedir ] )
			stack.push( m - stridedir );
		if( !m[ - 2 - stridedir ] )
			stack.push( m - 2 - stridedir );
		if( !m[ + 2 - stridedir ] )
			stack.push( m + 2 - stridedir );
		if( !m[ + stridedir ] )
			stack.push( m + stridedir );
		if( !m[ - 2 + stridedir ] )
			stack.push( m - 2 + stridedir );
		if( !m[ + 2 + stridedir ] )
			stack.push( m + 2 + stridedir );
	}


	h = out.height();
	cdst = pdst + stridedst;
	cdir = pdir + stridedir;
	while( h-- ) {
		size_t w = out.width();
		float* fdst =  ( ( float* ) cdst );
		uint8_t* udir = cdir;
		while( w-- ) {
			udir++;
			if( *udir++ != EDGE )
				*fdst = 0;
			else
				*fdst = 1.0f;
			fdst++;
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
	ImageIO::loadPNG( img, r.find( "lena.png" )/* "/home/heise/Pictures/myface2.png"*/ );
	Image imgf( img.width(), img.height(), IFormat::GRAY_FLOAT );
	img.convert( imgf );

	canny( out, imgf );

	ImageIO::savePNG( out, "canny.png" );

}
