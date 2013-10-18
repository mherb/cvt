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

#include <cvt/gfx/ifilter/ColorCode.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/math/Math.h>

namespace cvt {
	static ParamInfoTyped<Image*> pin( "Input", true );
	static ParamInfoTyped<Image*> pout( "Output", false );

	static ParamInfo * _params[ 2 ] = {
		&pin,
		&pout,
	};

#define NUMCOLORS 5
	static float _colorgradient[ NUMCOLORS * 4 ] = {
		1.000000f, 0.000000f, 0.000000f, 1.0f,
		1.000000f, 1.000000f, 0.000000f, 1.0f,
		0.000000f, 1.000000f, 0.000000f, 1.0f,
		0.000000f, 0.000000f, 1.000000f, 1.0f,
		0.000000f, 0.000000f, 0.000000f, 1.0f,
	};


	ColorCode::ColorCode() :
		IFilter( "ColorCode", _params, 2, IFILTER_CPU | IFILTER_OPENCL )
	{
	}

	void ColorCode::apply( Image& idst, const Image& isrc, float min, float max )
	{
		if( isrc.format() != IFormat::GRAY_FLOAT )
			throw CVTException( "Illegal data for color-coding" );

		float* pdst;
		const float* psrc;
		size_t w, h;
		float val;
		float alpha;
		size_t i1, i2;
		float low = Math::min( min, max );
		float high = Math::max( min, max );

		idst.reallocate( isrc.width(), isrc.height(), IFormat::RGBA_FLOAT );

		IMapScoped<float> dstmap( idst );
		IMapScoped<const float> srcmap( isrc );


		h = idst.height();
		while( h-- ) {
			pdst = dstmap.ptr();
			psrc = srcmap.ptr();

			w = idst.width();
			while( w-- ) {
				val = ( ( Math::clamp( *psrc++, low, high ) - min ) / ( max - min ) ) *  ( float ) ( NUMCOLORS - 1 );
				alpha = val - Math::floor( val );
				i1 = ( size_t ) val;
				i2 = Math::min<size_t>( i1 + 1, NUMCOLORS - 1 );
				*pdst++ = Math::mix( _colorgradient[ i1 * 4 ], _colorgradient[ i2 * 4 ], alpha  );
				*pdst++ = Math::mix( _colorgradient[ i1 * 4 + 1 ], _colorgradient[ i2 * 4 + 1 ], alpha  );
				*pdst++ = Math::mix( _colorgradient[ i1 * 4 + 2 ], _colorgradient[ i2 * 4 + 2 ], alpha  );
				*pdst++ = 1.0f;	/* alpha is one */
			}
			dstmap++;
			srcmap++;
		}
	}



	void ColorCode::apply( const ParamSet* set, IFilterType t ) const
	{
		Image * in = set->arg<Image*>( 0 );
		Image * out = set->arg<Image*>( 1 );

		switch ( t ) {
			case IFILTER_CPU:
				this->apply( *out, *in );
				break;
			default:
				throw CVTException( "Not implemented" );
		}
	}

}
