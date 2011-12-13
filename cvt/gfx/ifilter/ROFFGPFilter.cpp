#include <cvt/gfx/ifilter/ROFFGPFilter.h>

#include <cvt/cl/kernel/clear.h>
#include <cvt/cl/kernel/fgp/fgp_calcu.h>
#include <cvt/cl/kernel/fgp/fgp_calce.h>
#include <cvt/cl/kernel/fgp/fgp_calcun.h>

namespace cvt {
	static ParamInfoTyped<Image*> pin( "Input", true );
	static ParamInfoTyped<Image*> pout( "Output", false );
	static ParamInfoTyped<float> plambda( "Lambda", 0.05f, true );
	static ParamInfoTyped<int> pit( "Iterations", 10, true);

	static ParamInfo * _params[ 4 ] = {
		&pin,
		&pout,
		&plambda,
		&pit,
	};

	ROFFGPFilter::ROFFGPFilter() :
		IFilter( "ROFFGPFilter", _params, 4, IFILTER_OPENCL ),
		_clfgpclear( _clear_source, "clear" ),
		_clfgpcalcu( _fgp_calcu_source, "fgp_calcu" ),
		_clfgpcalcun( _fgp_calcun_source, "fgp_calcun" ),
		_clfgpcalce( _fgp_calce_source, "fgp_calce" )
	{
	}

	void ROFFGPFilter::apply( Image& dst, const Image& src, float lambda, size_t iter ) const
	{
		bool toggle = 0;
		float t = 1.0f, told;

		dst.reallocate( src, IALLOCATOR_CL );
		_imgn.reallocate( src.width() * 2, src.height(), IFormat::floatEquivalent( src.format() ), IALLOCATOR_CL );
		_imge0.reallocate( src.width() * 2, src.height(), IFormat::floatEquivalent( src.format() ), IALLOCATOR_CL );
		_imge1.reallocate( src.width() * 2, src.height(), IFormat::floatEquivalent( src.format() ), IALLOCATOR_CL );

		CLNDRange ndglobal( Math::pad16( dst.width() ), Math::pad16( dst.height() ) );
		CLNDRange ndglobal2( Math::pad16( dst.width() * 2 ), Math::pad16( dst.height() ) );

		CLNDRange ndlocalu( _clfgpcalcu.bestLocalRange2d( ndglobal ) );
		CLNDRange ndlocalun( _clfgpcalcu.bestLocalRange2d( ndglobal ) );
		CLNDRange ndlocale( _clfgpcalce.bestLocalRange2d( ndglobal ) );
		CLNDRange ndlocalclear( _clfgpclear.bestLocalRange2d( ndglobal2 ) );

		_clfgpclear.setArg( 0, _imge0 );
		_clfgpclear.run( ndglobal2, ndlocalclear );

		_clfgpclear.setArg( 0, _imge1 );
		_clfgpclear.run( ndglobal2, ndlocalclear );


		while( iter-- ) {
			told = t;
			t = 0.5f * ( 1.0f + Math::sqrt( 1.0f + 4.0f * told * told ) );

			/* update u and n */
			_clfgpcalcun.setArg( 0, dst );
			_clfgpcalcun.setArg( 1, _imgn );
			_clfgpcalcun.setArg( 2, src );
			_clfgpcalcun.setArg( 3, toggle?_imge0:_imge1 );
			_clfgpcalcun.setArg( 4, toggle?_imge1:_imge0 );
			_clfgpcalcun.setArg( 5, lambda );
			_clfgpcalcun.setArg( 6, ( told - 1.0f ) / t );
			_clfgpcalcun.setArg( 7, CLLocalSpace( sizeof( cl_float4 ) * ( ndlocalu.x() + 1 ) * ( ndlocalu.y() + 1 ) * 2 ) );
			_clfgpcalcun.run( ndglobal, ndlocalu );

			/* update e */
			_clfgpcalce.setArg( 0, toggle?_imge1:_imge0 );
			_clfgpcalce.setArg( 1, dst );
			_clfgpcalce.setArg( 2, _imgn );
			_clfgpcalce.setArg( 3, 1.0f / ( 8.0f * lambda ) );
			_clfgpcalce.setArg( 4, CLLocalSpace( sizeof( cl_float4 ) * ( ndlocale.x() + 1 ) * ( ndlocale.y() + 1 ) ) );
			_clfgpcalce.run( ndglobal, ndlocale );

			toggle = !toggle;
		}

		/* update u */
		_clfgpcalcu.setArg( 0, dst );
		_clfgpcalcu.setArg( 1, src );
		_clfgpcalcu.setArg( 2, _imgn );
		_clfgpcalcu.setArg( 3, lambda );
		_clfgpcalcu.setArg( 4, CLLocalSpace( sizeof( cl_float4 ) * ( ndlocalu.x() + 1 ) * ( ndlocalu.y() + 1 ) * 2 ) );
		_clfgpcalcu.runWait( ndglobal, ndlocalu );
	}

	void ROFFGPFilter::apply( const ParamSet* set, IFilterType t ) const
	{
		Image * in = set->arg<Image*>( 0 );
		Image * out = set->arg<Image*>( 1 );
		float lambda = set->arg<float>( 2 );
		int iter = set->arg<int>( 3 );

		switch ( t ) {
			case IFILTER_OPENCL:
				this->apply( *out, *in, lambda, iter );
				break;
			default:
				throw CVTException( "Not implemented" );
		}
	}

}
