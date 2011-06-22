#include <cvt/gfx/Image.h>
#include <cvt/io/Resources.h>
#include <cvt/util/Stack.h>
#include <cvt/util/Time.h>

using namespace cvt;

#define HORIZONTAL	0
#define VERTICAL	1
#define DIAGUP		2
#define DIAGDOWN	3
#define NOEDGE 1
#define PEDGE 0
#define EDGE 2

static inline void _canny_firstline( float* fdst, uint8_t* dir, size_t stridedst, size_t w, size_t wdir, float low, float high, Stack<uint8_t*>& stack )
{
		uint8_t* udir = dir + 1;
		/* set map outside image to NOEDGE */
		*dir = NOEDGE;
		*( dir + wdir - 1 ) = NOEDGE;
		dir++;

		/* border begin */
		if( *fdst >= low ) {
			if( *udir == HORIZONTAL && *( fdst + 1 ) > *fdst ) {
				*udir = NOEDGE;
			} else if( *udir == VERTICAL &&
					   *( ( float* )( ( uint8_t* ) fdst + stridedst  ) ) > *fdst ) {
				*udir = NOEDGE;
			} else if( *udir == DIAGUP &&
					  *( ( float* )( ( uint8_t* ) ( fdst + 1 ) + stridedst  ) ) > *fdst ) {
				*udir = NOEDGE;
			} else {
				if( *fdst >= high ) {
					*udir = EDGE;
					stack.push( udir );
				} else {
					*udir = PEDGE;
				}
			}
		} else
			*udir = NOEDGE;
		fdst++;
		udir++;

		/* main */
		w -= 2;
		while( w-- ) {
			if( *fdst >= low ) {
				if( *udir == HORIZONTAL &&
				    ( *(fdst - 1 ) > *fdst ||
					   *( fdst + 1 ) > *fdst ) ) {
						*udir = NOEDGE;
				} else if( *udir == VERTICAL &&
					   *( ( float* )( ( uint8_t* ) fdst + stridedst  ) ) > *fdst ) {
						*udir = NOEDGE;
				} else if( *udir == DIAGDOWN &&
					   *( ( float* )( ( uint8_t* ) ( fdst - 1 ) + stridedst  ) ) > *fdst ) {
						*udir = NOEDGE;
				} else if( *udir == DIAGUP &&
					   *( ( float* )( ( uint8_t* ) ( fdst + 1 ) + stridedst  ) ) > *fdst ) {
						*udir = NOEDGE;
				} else {
					if( *fdst >= high ) {
						*udir = EDGE;
						stack.push( udir );
					} else {
						*udir = PEDGE;
					}
				}
			} else
					*udir = NOEDGE;
			fdst++;
			udir++;
		}
		/* border end */
		if( *fdst >= low ) {
			if( *udir == HORIZONTAL && *(fdst - 1 ) > *fdst ) {
				*udir = NOEDGE;
			} else if( *udir == VERTICAL &&
					   *( ( float* )( ( uint8_t* ) fdst + stridedst  ) ) > *fdst ) {
				*udir = NOEDGE;
			} else if( *udir == DIAGDOWN &&
					   *( ( float* )( ( uint8_t* ) ( fdst - 1 ) + stridedst  ) ) > *fdst ) {
				*udir = NOEDGE;
			} else {
				if( *fdst >= high ) {
					*udir = EDGE;
					stack.push( udir );
				} else {
					*udir = PEDGE;
				}
			}
		} else
			*udir = NOEDGE;

}


static inline void _canny_lastline( float* fdst, uint8_t* dir, size_t stridedst, size_t w, size_t wdir, float low, float high, Stack<uint8_t*>& stack )
{
		uint8_t* udir = dir + 1;
		/* set map outside image to NOEDGE */
		*dir = NOEDGE;
		*( dir + wdir - 1 ) = NOEDGE;
		dir++;

		/* border begin */
		if( *fdst >= low ) {
			if( *udir == HORIZONTAL && *( fdst + 1 ) > *fdst ) {
				*udir = NOEDGE;
			} else if( *udir == VERTICAL &&
					   *( ( float* )( ( uint8_t* ) fdst - stridedst  ) ) > *fdst ) {
				*udir = NOEDGE;
			} else if( *udir == DIAGDOWN &&
					  *( ( float* )( ( uint8_t* ) ( fdst + 1 ) - stridedst  ) ) > *fdst ) {
				*udir = NOEDGE;
			} else {
				if( *fdst >= high ) {
					*udir = EDGE;
					stack.push( udir );
				} else {
					*udir = PEDGE;
				}
			}
		} else
			*udir = NOEDGE;
		fdst++;
		udir++;

		/* main */
		w -= 2;
		while( w-- ) {
			if( *fdst >= low ) {
				if( *udir == HORIZONTAL &&
				    ( *(fdst - 1 ) > *fdst ||
					   *( fdst + 1 ) > *fdst ) ) {
						*udir = NOEDGE;
				} else if( *udir == VERTICAL &&
						    *( ( float* )( ( uint8_t* ) fdst - stridedst  ) ) > *fdst ) {
						*udir = NOEDGE;
				} else if( *udir == DIAGDOWN &&
						   *( ( float* )( ( uint8_t* ) ( fdst + 1 ) - stridedst  ) ) > *fdst ) {
						*udir = NOEDGE;
				} else if( *udir == DIAGUP &&
						   *( ( float* )( ( uint8_t* ) ( fdst - 1 ) - stridedst  ) ) > *fdst ) {
						*udir = NOEDGE;
				} else {
					if( *fdst >= high ) {
						*udir = EDGE;
						stack.push( udir );
					} else {
						*udir = PEDGE;
					}
				}
			} else
					*udir = NOEDGE;
			fdst++;
			udir++;
		}
		/* border end */
		if( *fdst >= low ) {
			if( *udir == HORIZONTAL && *(fdst - 1 ) > *fdst ) {
				*udir = NOEDGE;
			} else if( *udir == VERTICAL &&
					   *( ( float* )( ( uint8_t* ) fdst - stridedst  ) ) > *fdst ) {
				*udir = NOEDGE;
			} else if( *udir == DIAGUP &&
					   *( ( float* )( ( uint8_t* ) ( fdst - 1 ) - stridedst  ) ) > *fdst ) {
				*udir = NOEDGE;
			} else {
				if( *fdst >= high ) {
					*udir = EDGE;
					stack.push( udir );
				} else {
					*udir = PEDGE;
				}
			}
		} else
			*udir = NOEDGE;

}

void canny( Image& out, const Image& in, float low = 0.02f, float high = 0.025f )
{
	Image dx( in.width(), in.height(), IFormat::GRAY_FLOAT );
	Image dy( in.width(), in.height(), IFormat::GRAY_FLOAT );
	Image dir( in.width() + 2, in.height() + 2, IFormat::GRAY_UINT8 );

	in.convolve( dx, IKernel::HAAR_HORIZONTAL_3, IKernel::GAUSS_VERTICAL_3 );
	in.convolve( dy, IKernel::GAUSS_HORIZONTAL_3, IKernel::HAAR_VERTICAL_3 );

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
	uint8_t* cdir = pdir + stridedir + 1;
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
				*udir++ = ( *fdx > 0 ) ^ ( *fdy > 0 ) ? DIAGDOWN : DIAGUP;
			} else {
				*udir++ = HORIZONTAL;
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


	Stack<uint8_t*> stack( 1024 );
	h = out.height() - 2;
	memset( pdir, NOEDGE, dir.width() );
//	memset( pdir + stridedir, NOEDGE, dir.width() ); // remove
	memset( pdir + ( dir.height() - 1 ) * stridedir, NOEDGE, dir.width() );
	memset( pdir + ( dir.height() - 2 ) * stridedir, NOEDGE, dir.width() ); // remove
	cdst = pdst + stridedst;
	cdir = pdir + 2 * stridedir;

	_canny_firstline( ( float* ) pdst, pdir + stridedir, stridedst, out.width(), dir.width(), low, high, stack );
	while( h-- ) {
		size_t w = out.width() - 2;
		float* fdst =  ( float* ) cdst;
		uint8_t* udir = cdir + 1;
		/* set map outside image to NOEDGE */
		*cdir = NOEDGE;
		*( cdir + dir.width() - 1 ) = NOEDGE;

		/* border begin */
		if( *fdst >= low ) {
			if( *udir == HORIZONTAL && *( fdst + 1 ) > *fdst ) {
				*udir = NOEDGE;
			} else if( *udir == VERTICAL &&
					  ( *( ( float* )( ( uint8_t* ) fdst - stridedst  ) ) > *fdst ||
					   *( ( float* )( ( uint8_t* ) fdst + stridedst  ) ) > *fdst ) ) {
				*udir = NOEDGE;
			} else if( *udir == DIAGDOWN &&
					  *( ( float* )( ( uint8_t* ) ( fdst + 1 ) - stridedst  ) ) > *fdst ) {
				*udir = NOEDGE;
			} else if( *udir == DIAGUP &&
					  *( ( float* )( ( uint8_t* ) ( fdst + 1 ) + stridedst  ) ) > *fdst ) {
				*udir = NOEDGE;
			} else {
				if( *fdst >= high ) {
					*udir = EDGE;
					stack.push( udir );
				} else {
					*udir = PEDGE;
				}
			}
		} else
			*udir = NOEDGE;
		fdst++;
		udir++;

		/* main */
		while( w-- ) {
			if( *fdst >= low ) {
				if( *udir == HORIZONTAL &&
				    ( *(fdst - 1 ) > *fdst ||
					   *( fdst + 1 ) > *fdst ) ) {
						*udir = NOEDGE;
				} else if( *udir == VERTICAL &&
					( *( ( float* )( ( uint8_t* ) fdst - stridedst  ) ) > *fdst ||
					   *( ( float* )( ( uint8_t* ) fdst + stridedst  ) ) > *fdst ) ) {
						*udir = NOEDGE;
				} else if( *udir == DIAGDOWN &&
					( *( ( float* )( ( uint8_t* ) ( fdst + 1 ) - stridedst  ) ) > *fdst ||
					   *( ( float* )( ( uint8_t* ) ( fdst - 1 ) + stridedst  ) ) > *fdst ) ) {
						*udir = NOEDGE;
				} else if( *udir == DIAGUP &&
					( *( ( float* )( ( uint8_t* ) ( fdst - 1 ) - stridedst  ) ) > *fdst ||
					   *( ( float* )( ( uint8_t* ) ( fdst + 1 ) + stridedst  ) ) > *fdst ) ) {
						*udir = NOEDGE;
				} else {
					if( *fdst >= high ) {
						*udir = EDGE;
						stack.push( udir );
					} else {
						*udir = PEDGE;
					}
				}
			} else
					*udir = NOEDGE;
			fdst++;
			udir++;
		}
		/* border end */
		if( *fdst >= low ) {
			if( *udir == HORIZONTAL && *(fdst - 1 ) > *fdst ) {
				*udir = NOEDGE;
			} else if( *udir == VERTICAL &&
					  ( *( ( float* )( ( uint8_t* ) fdst - stridedst  ) ) > *fdst ||
					   *( ( float* )( ( uint8_t* ) fdst + stridedst  ) ) > *fdst ) ) {
				*udir = NOEDGE;
			} else if( *udir == DIAGDOWN &&
					   *( ( float* )( ( uint8_t* ) ( fdst - 1 ) + stridedst  ) ) > *fdst ) {
				*udir = NOEDGE;
			} else if( *udir == DIAGUP &&
					   *( ( float* )( ( uint8_t* ) ( fdst - 1 ) - stridedst  ) ) > *fdst ) {
				*udir = NOEDGE;
			} else {
				if( *fdst >= high ) {
					*udir = EDGE;
					stack.push( udir );
				} else {
					*udir = PEDGE;
				}
			}
		} else
			*udir = NOEDGE;

		cdst += stridedst;
		cdir += stridedir;
	}
	_canny_lastline( ( float* ) cdst, cdir, stridedst, out.width(), dir.width(), low, high, stack );

	while( !stack.isEmpty() ) {
		uint8_t* m;
		m = stack.pop();
		*m = EDGE;
		if( !m[ - 1 + stridedir ] )
			stack.push( m - 1 + stridedir );
		if( !m[ + stridedir ] )
			stack.push( m + stridedir );
		if( !m[ + 1 + stridedir ] )
			stack.push( m + 1 + stridedir );
		if( !m[ - 1 - stridedir ] )
			stack.push( m - 1 - stridedir );
		if( !m[ - stridedir ] )
			stack.push( m - stridedir );
		if( !m[ + 1 - stridedir ] )
			stack.push( m + 1 - stridedir );
		if( !m[ + 1 ] )
			stack.push( m + 1 );
		if( !m[ - 1 ] )
			stack.push( m - 1 );
	}


	h = out.height();
	cdst = pdst;
	cdir = pdir + stridedir + 1;
	while( h-- ) {
		size_t w = out.width();
		float* fdst =  ( ( float* ) cdst );
		uint8_t* udir = cdir;
		while( w-- ) {
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
	img.load( /*r.find( "boss.png" )*/ "/home/heise/Pictures/myface2.png" );
	Image imgf( img.width(), img.height(), IFormat::GRAY_FLOAT );
	img.convert( imgf );

	Time t;
	t.reset();
	canny( out, imgf );
	std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;

	out.save( "canny.png" );

}
