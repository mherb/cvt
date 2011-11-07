#include "ROFFGPFilter.h"

#include "cl/fgp_clear.h"

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
		_clfgpclear( _fgp_clear_source, "fgp_clear" )
	{
	}

	void ROFFGPFilter::apply( Image& dst, const Image& src, float lambda, uint64_t iter ) const
	{
		_clfgpclear.setArg( 0, dst );
		CLNDRange ndglobal( Math::pad16( dst.width() ), Math::pad16( dst.height() ) );
		_clfgpclear.run( ndglobal, _clfgpclear.bestLocalRange2d( ndglobal ) );
	}

	void ROFFGPFilter::apply( const ParamSet* set, IFilterType t ) const
	{
	}

}
