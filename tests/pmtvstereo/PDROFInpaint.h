#ifndef CVT_PDROFINPAINT
#define CVT_PDROFINPAINT

#include <cvt/cl/kernel/PDHuberWeightedInpaint.h>
#include <cvt/cl/kernel/fill.h>

namespace cvt {

	class PDROFInpaint
	{
		public:
			PDROFInpaint();

			void apply( Image& output, const Image& input, const Image& weight, const Image& mask, float lambda, int iter );
		private:
			CLKernel _clfill;
			CLKernel _clrof;
	};

	inline PDROFInpaint::PDROFInpaint() :
		_clfill( _fill_source, "fill" ),
		_clrof(_PDHuberWeightedInpaint_source, "PDHuberWeightedInpaint" )
	{
	}

	inline void PDROFInpaint::apply( Image& output, const Image& input, const Image& weight, const Image& mask, float lambda, int iter )
	{
		//TODO: check image for CL and size
		Image cltmp( input, IALLOCATOR_CL );
		Image clp1( input.width()*2, input.height(), input.format(), IALLOCATOR_CL );
		Image clp2( input.width()*2, input.height(), input.format(), IALLOCATOR_CL );
		Image* climg[ 2 ] = { &cltmp, &output };
		Image* clp[ 2 ] = { &clp1, &clp2 };

		cl_float4 fillvalue = {{ 0.0f, 0.0f, 0.0f, 0.0f }};
//		cl_float4 lambda4 = {{ lambda, lambda, 10.0f * lambda, 0.0f }};

		_clfill.setArg( 0, *clp[ 1 ] );
		_clfill.setArg( 1, fillvalue );
		_clfill.run( CLNDRange( Math::pad( clp2.width(), 16 ), Math::pad( clp2.height(), 16 ) ), CLNDRange( 16, 16 ) );

		int swap = 0;
		for( int i = 0; i < iter; i++ ) {
			swap = i & 1;

			_clrof.setArg( 0, *climg[ swap ] );
			_clrof.setArg( 1, *clp[ swap ] );
			_clrof.setArg( 2, *climg[ 1 - swap ] );
			_clrof.setArg( 3, *clp[ 1 - swap ] );
			_clrof.setArg( 4, input );
			_clrof.setArg( 5, weight );
			_clrof.setArg( 6, mask );
			_clrof.setArg( 7, lambda );
			_clrof.setArg( 8, CLLocalSpace( sizeof( cl_float4 ) * 18 * 18 ) );
			_clrof.setArg( 9, CLLocalSpace( sizeof( cl_float8 ) * 18 * 18 ) );
			_clrof.setArg( 10, CLLocalSpace( sizeof( cl_float ) * 18 * 18 ) );
			_clrof.run( CLNDRange( Math::pad( input.width(), 16 ), Math::pad( input.height(), 16 ) ), CLNDRange( 16, 16 ) );
		}

		if( !swap ) {
			_clrof.setArg( 0, *climg[ 1 ] );
			_clrof.setArg( 1, *clp[ 1 ] );
			_clrof.setArg( 2, *climg[ 0 ] );
			_clrof.setArg( 3, *clp[ 0 ] );
			_clrof.setArg( 4, input );
			_clrof.setArg( 5, weight );
			_clrof.setArg( 6, mask );
			_clrof.setArg( 7, lambda );
			_clrof.setArg( 8, CLLocalSpace( sizeof( cl_float4 ) * 18 * 18 ) );
			_clrof.setArg( 9, CLLocalSpace( sizeof( cl_float8 ) * 18 * 18 ) );
			_clrof.setArg( 10, CLLocalSpace( sizeof( cl_float ) * 18 * 18 ) );
			_clrof.run( CLNDRange( Math::pad( input.width(), 16 ), Math::pad( input.height(), 16 ) ), CLNDRange( 16, 16 ) );
		}
	}
}

#endif
