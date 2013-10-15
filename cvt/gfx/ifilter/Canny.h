/*
   The MIT License (MIT)

   Copyright (c) 2011 - 2013, Philipp Heise and Sebastian Klose

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/


#ifndef CVT_CANNY_H
#define CVT_CANNY_H

#include <cvt/gfx/IFilter.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/util/Plugin.h>
#include <cvt/util/Stack.h>
#include <cvt/util/PluginManager.h>

namespace cvt {
	static ParamInfoTyped<Image*> _pinput( "Input" );
	static ParamInfoTyped<bool>   _physt( "Hysteresis-Threshold", false, true, true, true );
	static ParamInfoTyped<float>  _plow( "Threshold low", 0.0f, 1.0f, 0.01f );
	static ParamInfoTyped<float>  _phigh( "Threshold high", 0.0f, 1.0f, 0.05f );
	static ParamInfoTyped<Image*> _poutput( "Output", NULL, false );
	static ParamInfoTyped<Image*> _psobelx( "Gradient X", NULL, false );
	static ParamInfoTyped<Image*> _psobely( "Gradient Y", NULL, false );

	static ParamInfo* _canny_params[] =
	{
		&_pinput,
		&_physt,
		&_plow,
		&_phigh,
		&_poutput,
		&_psobelx,
		&_psobely
	};

	class Canny : public IFilter
	{
		private:
                enum SearchDirections {
                    HORIZONTAL,
                    VERTICAL,
                    DIAGUP,
                    DIAGDOWN
                };

                enum EdgeType {
                    NOEDGE = 1,
                    PEDGE = 0,
                    EDGE = 2
                };

		public:
			Canny();
			~Canny();
			void apply( const ParamSet* attribs, IFilterType iftype ) const;
            void apply( Image& out, const Image& in, float low = 0.02f, float high = 0.025f ) const;

		private:
			Canny( const Canny& );
                        inline void _canny_firstline( float* fdst, uint8_t* dir, size_t stridedst, size_t w, size_t wdir, float low, float high, Stack<uint8_t*>& stack ) const;
                        inline void _canny_lastline( float* fdst, uint8_t* dir, size_t stridedst, size_t w, size_t wdir, float low, float high, Stack<uint8_t*>& stack ) const;

	};

	inline Canny::Canny() : IFilter( "Canny", _canny_params, 7, IFILTER_CPU )
	{
	}

	inline Canny::~Canny()
	{
	}


	inline void Canny::apply( Image& out, const Image& in, float low, float high ) const
	{
		Image dx( in.width(), in.height(), IFormat::GRAY_FLOAT );
		Image dy( in.width(), in.height(), IFormat::GRAY_FLOAT );
		Image dir( in.width(), in.height(), IFormat::GRAY_UINT8 );

		in.convolve( dx, IKernel::HAAR_HORIZONTAL_3, IKernel::GAUSS_VERTICAL_3 );
		in.convolve( dy, IKernel::GAUSS_HORIZONTAL_3, IKernel::HAAR_VERTICAL_3 );

		out.reallocate( in.width(), in.height(), IFormat::GRAY_FLOAT );

		IMapScoped<const float> mapdx( dx );
		IMapScoped<const float> mapdy( dy );
		IMapScoped<float> mapdst( out );
		IMapScoped<uint8_t> mapdir( dir );
		size_t stridedir = mapdir.stride();
		size_t stridedst = mapdst.stride();

		/* calculate edge direction and strength */
		size_t h = out.height();
		while( h-- ) {
			size_t w = out.width();
			const float* fdx =  mapdx.ptr();
			const float* fdy =  mapdy.ptr();
			float* fdst = mapdst.ptr();
			uint8_t* udir = mapdir.ptr();
			while( w-- ) {
				*fdst++ = Math::sqrt( Math::sqr( *fdx ) + Math::sqr( *fdy ) );
				float m = Math::abs( *fdy ) / Math::abs( *fdx );
				if( m >= 2.4142f ) {
					*udir++ = VERTICAL;
				} else if( m >= 0.41421f ) {
					*udir++ = ( *fdx > 0 ) ^ ( *fdy > 0 ) ? DIAGDOWN : DIAGUP;
				} else {
					*udir++ = HORIZONTAL;
				}
				fdx++;
				fdy++;
			}
			mapdx++;
			mapdy++;
			mapdir++;
			mapdst++;
		}

		Stack<uint8_t*> stack( 1024 );
		mapdst.reset();
		mapdir.reset();
		// first and last line are all NOEDGE
		memset( mapdir.ptr(), NOEDGE, dir.width() );
		memset( mapdir.ptr() + mapdir.stride() * ( dir.height() - 1 ), NOEDGE, dir.width() );

		h = out.height() - 2;
		mapdir++;
		mapdst++;
		while( h-- ) {
			size_t w = out.width() - 2;
			float* fdst =  mapdst.ptr();
			uint8_t* udir = mapdir.ptr();
			/* set map outside image to NOEDGE */
			*udir++ = NOEDGE;
			*fdst++ = 0.0f;
			*( ( float* )( ( uint8_t* ) fdst - stridedst - 1  ) ) = 0.0f;

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
			*udir = NOEDGE;
			mapdir++;
			mapdst++;
		}

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


		mapdst.reset();
		mapdir.reset();
		h = out.height();
		while( h-- ) {
			size_t w = out.width();
			float* fdst =  mapdst.ptr();
			uint8_t* udir = mapdir.ptr();
			while( w-- ) {
				if( *udir++ != EDGE )
					*fdst++ = 0;
				else
					*fdst++ = 1.0f;
			}
			mapdst++;
			mapdir++;
		}
	}

	inline void Canny::apply( const ParamSet* attribs, IFilterType iftype ) const
        {
            Image * in = attribs->arg<Image*>( 0 );
            Image * out = attribs->arg<Image*>( 4 );

            float tlow = attribs->arg<float>( 2 );
            float thigh = attribs->arg<float>( 3 );

            switch ( iftype ) {
                    case IFILTER_CPU:
                            this->apply( *out, *in, tlow, thigh );
                            break;
                    default:
                            throw CVTException( "Not implemented" );
            }
	}
}

#endif
