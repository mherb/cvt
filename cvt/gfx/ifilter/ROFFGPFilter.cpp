#include <cvt/gfx/ifilter/ROFFGPFilter.h>

#include <cvt/cl/kernel/clear.h>
#include <cvt/cl/kernel/fgp/fgp.h>
#include <cvt/cl/kernel/fgp/fgp_data.h>

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
		_clfgp( _fgp_source, "fgp" ),
		_clfgpdata( _fgp_data_source, "fgp_data" ),
		_clfgpclear( _clear_source, "clear" )
	{
	}

	void ROFFGPFilter::apply( Image& dst, const Image& src, float lambda, size_t iter ) const
	{
		float t = 1.0f, told = 1.0f;
		Image* e[ 3 ];

		dst.reallocate( src, IALLOCATOR_CL );
		_imge0.reallocate( src.width() * 2, src.height(), IFormat::floatEquivalent( src.format() ), IALLOCATOR_CL );
		_imge1.reallocate( src.width() * 2, src.height(), IFormat::floatEquivalent( src.format() ), IALLOCATOR_CL );
		_imge2.reallocate( src.width() * 2, src.height(), IFormat::floatEquivalent( src.format() ), IALLOCATOR_CL );

		e[ 0 ] = &_imge0;
		e[ 1 ] = &_imge1;
		e[ 2 ] = &_imge2;

		CLNDRange ndglobal( Math::pad16( dst.width() ), Math::pad16( dst.height() ) );
		CLNDRange ndglobal2( Math::pad16( dst.width() * 2 ), Math::pad16( dst.height() ) );

		CLNDRange ndlocalfgp( /*_clfgp.bestLocalRange2d( ndglobal )*/ 16, 16 );
		CLNDRange ndlocalclear( /*_clfgpclear.bestLocalRange2d( ndglobal2 )*/ 16, 16 );


		_clfgpclear.setArg( 0, *e[ 1 ] );
		_clfgpclear.run( ndglobal2, ndlocalclear );

		_clfgpclear.setArg( 0, *e[ 2 ] );
		_clfgpclear.run( ndglobal2, ndlocalclear );


		while( iter-- ) {
			_clfgp.setArg( 0, *e[ 0 ] );
			_clfgp.setArg( 1, src );
			_clfgp.setArg( 2, *e[ 1 ] );
			_clfgp.setArg( 3, *e[ 2 ] );
			_clfgp.setArg( 4, lambda );
			_clfgp.setArg( 5, ( told - 1.0f ) / t );
			_clfgp.setArg( 6, CLLocalSpace( sizeof( cl_float4 ) * ( ndlocalfgp.x() + 2 ) * ( ndlocalfgp.y() + 2 ) * 2 ) );
			_clfgp.setArg( 7, CLLocalSpace( sizeof( cl_float4 ) * ( ndlocalfgp.x() + 1 ) * ( ndlocalfgp.y() + 1 ) ) );
			_clfgp.run( ndglobal, ndlocalfgp );

			Image* tmp = e[ 2 ];
			e[ 2 ] = e[ 1 ];
			e[ 1 ] = e[ 0 ];
			e[ 0 ] = tmp;

			told = t;
			t = 0.5f * ( 1.0f + Math::sqrt( 1.0f + 4.0f * told * told ) );
		}

		_clfgpdata.setArg( 0, dst );
		_clfgpdata.setArg( 1, src );
		_clfgpdata.setArg( 2, *e[ 1 ] );
		_clfgpdata.setArg( 3, *e[ 2 ] );
		_clfgpdata.setArg( 4, lambda );
		_clfgpdata.setArg( 5, ( told - 1.0f ) / t );
		_clfgpdata.setArg( 6, CLLocalSpace( sizeof( cl_float4 ) * ( ndlocalfgp.x() + 2 ) * ( ndlocalfgp.y() + 2 ) * 2 ) );
		_clfgpdata.run( ndglobal, ndlocalfgp );
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
