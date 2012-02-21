/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
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
