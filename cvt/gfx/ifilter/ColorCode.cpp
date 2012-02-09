#include <cvt/gfx/ifilter/ColorCode.h>

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
		1.000000f, 0.000000f, 1.000000f, 1.0f,
		0.000000f, 0.000000f, 1.000000f, 1.0f,
		0.000000f, 0.000000f, 0.000000f, 1.0f,
	};


	ColorCode::ColorCode() :
		IFilter( "ColorCode", _params, 2, IFILTER_CPU | IFILTER_OPENCL )
	{
	}

	void ColorCode::apply( Image& idst, const Image& isrc, float min, float max ) const
	{
		if( isrc.format() != IFormat::GRAY_FLOAT )
			throw CVTException( "Illegal data for color-coding" );

		uint8_t* dst;
		uint8_t* src;
		size_t stridedst;
		size_t stridesrc;
		float* pdst;
		float* psrc;
		size_t w, h;
		float d;
		float color[ 4 ];
		float radius;
		float angle;
		float val;
		float alpha;
		size_t i1, i2;
		uint8_t const* optr1;
		uint8_t const* optr2;

		idst.reallocate( isrc.width(), isrc.height(), IFormat::RGBA_FLOAT );

		optr1 = dst = idst.map( &stridedst );
		optr2 = src = isrc.map( &stridesrc );

		h = idst.height();
		while( h-- ) {
			pdst = ( float* ) dst;
			psrc = ( float* ) src;

			w = idst.width();
			while( w-- ) {
				d = *psrc++;
				val = ( Math::clamp( d - min, 0, max ) / max ) *  ( float ) ( NUMCOLORS - 1 );
				alpha = val - Math::floor( val );
				i1 = ( size_t ) val;
				i2 = Math::max( i1 + 1, NUMCOLORS - 1 );
				color[ 0 ] = Math::mix( _colorgradient[ i1 * 4 ], _colorgradient[ i2 * 4 ], alpha  );
				color[ 1 ] = Math::mix( _colorgradient[ i1 * 4 + 1 ], _colorgradient[ i2 * 4 + 1 ], alpha  );
				color[ 2 ] = Math::mix( _colorgradient[ i1 * 4 + 2 ], _colorgradient[ i2 * 4 + 2 ], alpha  );
				color[ 3 ] = 1.0f;
				/* alpha remains */
				*pdst++ = color[ 0 ];
				*pdst++ = color[ 1 ];
				*pdst++ = color[ 2 ];
				*pdst++ = color[ 3 ];
			}

			dst += stridedst;
			src += stridesrc;
		}
		idst.unmap( optr1 );
		isrc.unmap( optr2 );
	}



	void ColorCode::apply( const ParamSet* set, IFilterType t ) const
	{
		Image * in = set->arg<Image*>( 0 );
		Image * out = set->arg<Image*>( 1 );

		switch ( t ) {
			case IFILTER_OPENCL:
				this->apply( *out, *in );
				break;
			default:
				throw CVTException( "Not implemented" );
		}
	}

}
