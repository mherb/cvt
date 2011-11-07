#include "ROFFGPFilter.h"

#include "fgp_clear.h"
#include "fgp_calcu.h"
#include "fgp_calce.h"
#include "fgp_calcn.h"

namespace cvt {
	static ParamInfoTyped<Image*> pin( "Input", true );
	static ParamInfoTyped<Image*> pout( "Output", false );
	static ParamInfoTyped<int> pit( "Iterations", true);
	static ParamInfoTyped<float> plambda( "Lambda", true );

	static ParamInfo * _params[ 4 ] = {
		&pin,
		&pout,
		&pit,
		&plambda
	};

	ROFFGPFilter::ROFFGPFilter() :
		IFilter( "ROFFGPFilter", _params, 4, IFILTER_OPENCL ),
		_clfgpclear( _fgp_clear_source, "fgp_clear" ),
		_clfgpcalcu( _fgp_calcu_source, "fgp_calcu" ),
		_clfgpcalce( _fgp_calce_source, "fgp_calce" ),
		_clfgpcalcn( _fgp_calcn_source, "fgp_calcn" )
	{
	}

	void ROFFGPFilter::apply( Image& dst, const Image& src, float lambda, size_t iter ) const
	{
		bool toggle = 0;
		float t = 1.0f, told;

		dst.reallocate( src, IALLOCATOR_CL );
		_imgn.reallocate( src, IALLOCATOR_CL );
		_imge0.reallocate( src, IALLOCATOR_CL );
		_imge1.reallocate( src, IALLOCATOR_CL );

		CLNDRange ndglobal( Math::pad16( dst.width() ), Math::pad16( dst.height() ) );

		CLNDRange ndlocalu( _clfgpcalcu.bestLocalRange2d( ndglobal ) );
		CLNDRange ndlocale( _clfgpcalce.bestLocalRange2d( ndglobal ) );
		CLNDRange ndlocaln( _clfgpcalcn.bestLocalRange2d( ndglobal ) );


		_clfgpclear.setArg( 0, _imgn );
		_clfgpclear.run( ndglobal, _clfgpclear.bestLocalRange2d( ndglobal ) );

		_clfgpclear.setArg( 0, _imge1 );
		_clfgpclear.run( ndglobal, _clfgpclear.bestLocalRange2d( ndglobal ) );

		while( iter-- ) {

			/* update u */
			_clfgpcalcu.setArg( 0, dst );
			_clfgpcalcu.setArg( 1, src );
			_clfgpcalcu.setArg( 2, _imgn );
			_clfgpcalcu.setArg( 3, lambda );
			_clfgpcalcu.setArg( 4, CLLocalSpace( sizeof( cl_float4 ) * ( ndlocalu.x() + 1 ) * ( ndlocalu.y() + 1 ) * 2 ) );
			_clfgpcalcu.run( ndglobal, ndlocalu );

			_clfgpcalce.setArg( 0, toggle?_imge1:_imge0 );
			_clfgpcalce.setArg( 1, dst );
			_clfgpcalce.setArg( 2, _imgn );
			_clfgpcalce.setArg( 3, 1.0f / ( 8.0f * lambda ) );
			_clfgpcalce.setArg( 4, CLLocalSpace( sizeof( cl_float4 ) * ( ndlocale.x() + 1 ) * ( ndlocale.y() + 1 ) ) );
			_clfgpcalce.run( ndglobal, ndlocale );

			told = t;
			t = 0.5f * ( 1.0f + sqrtf( 1.0f + 4.0f * told * told ) );

			_clfgpcalcn.setArg( 0, _imgn );
			_clfgpcalcn.setArg( 1, toggle?_imge1:_imge0 );
			_clfgpcalcn.setArg( 2, toggle?_imge0:_imge1 );
			_clfgpcalcn.setArg( 3, told / t );
			_clfgpcalcn.run( ndglobal, ndlocaln );
			toggle = !toggle;
		}

		/* update u */
		_clfgpcalcu.setArg( 0, dst );
		_clfgpcalcu.setArg( 1, src );
		_clfgpcalcu.setArg( 2, _imgn );
		_clfgpcalcu.setArg( 3, lambda );
		_clfgpcalcu.setArg( 4, CLLocalSpace( sizeof( cl_float4 ) * ( ndlocalu.x() + 1 ) * ( ndlocalu.y() + 1 ) * 2 ) );
		_clfgpcalcu.run( ndglobal, ndlocalu );
	}

	void ROFFGPFilter::apply( const ParamSet* set, IFilterType t ) const
	{
	}

}
