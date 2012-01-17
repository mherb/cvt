#include <cvt/gfx/ifilter/GuidedFilter.h>

#include <cvt/cl/kernel/guidedfilter/guidedfilter_calcab.h>
#include <cvt/cl/kernel/guidedfilter/guidedfilter_applyab.h>

namespace cvt {
	static ParamInfoTyped<Image*> pin( "Input", true );
	static ParamInfoTyped<Image*> pinguide( "Guide", true );
	static ParamInfoTyped<Image*> pout( "Output", false );
	static ParamInfoTyped<int>	  pradius( "Radius", true );
	static ParamInfoTyped<float>  pepsilon( "Epsilon", true );

	static ParamInfo * _params[ 5 ] = {
		&pin,
		&pinguide,
		&pout,
		&pradius,
		&pepsilon,
	};

	GuidedFilter::GuidedFilter() :
		IFilter( "GuidedFilter", _params, 5, IFILTER_CPU | IFILTER_OPENCL ),
		_clguidedfilter_calcab( _guidedfilter_calcab_source, "guidedfilter_calcab" ),
		_clguidedfilter_applyab( _guidedfilter_applyab_source, "guidedfilter_applyab" )
	{
	}

	void GuidedFilter::apply( Image& dst, const Image& src, const Image& guide, const int radius, const float epsilon ) const
	{
		// G guidance image, S source image
		Image iint( src.width(), src.height(), IFormat::floatEquivalent( src.format() ), IALLOCATOR_CL );
		Image imeanG( src.width(), src.height(), src.format(), IALLOCATOR_CL );
		Image imeanS( src.width(), src.height(), src.format(), IALLOCATOR_CL );
		Image imeanGS( src.width(), src.height(), src.format(), IALLOCATOR_CL );
		Image imeanGG( src.width(), src.height(), src.format(), IALLOCATOR_CL );

		Image ia( src.width(), src.height(), IFormat::floatEquivalent( src.format() ), IALLOCATOR_CL );
		Image ib( src.width(), src.height(), IFormat::floatEquivalent( src.format() ), IALLOCATOR_CL );

		_intfilter.apply( iint, guide );
		_boxfilter.apply( imeanG, iint, radius );
		_intfilter.apply( iint, src );
		_boxfilter.apply( imeanS, iint, radius );
		_intfilter.apply( iint, guide, &guide );
		_boxfilter.apply( imeanGG, iint, radius );
		_intfilter.apply( iint, guide, &src );
		_boxfilter.apply( imeanGS, iint, radius );

		CLNDRange global( Math::pad16( src.width() ), Math::pad16( src.height() ) );

		_clguidedfilter_calcab.setArg( 0, ia );
		_clguidedfilter_calcab.setArg( 1, ib );
		_clguidedfilter_calcab.setArg( 2, imeanG );
		_clguidedfilter_calcab.setArg( 3, imeanS );
		_clguidedfilter_calcab.setArg( 4, imeanGS );
		_clguidedfilter_calcab.setArg( 5, imeanGG );
		_clguidedfilter_calcab.setArg( 6, epsilon );

		_clguidedfilter_calcab.run( global, _clguidedfilter_calcab.bestLocalRange2d( global ) );

		_intfilter.apply( iint, ia );
		_boxfilter.apply( ia, iint, radius );
		_intfilter.apply( iint, ib );
		_boxfilter.apply( ib, iint, radius );

		_clguidedfilter_applyab.setArg( 0, dst );
		_clguidedfilter_applyab.setArg( 1, guide );
		_clguidedfilter_applyab.setArg( 2, ia );
		_clguidedfilter_applyab.setArg( 3, ib );
		_clguidedfilter_applyab.run( global, _clguidedfilter_applyab.bestLocalRange2d( global ) );
	}

	void GuidedFilter::apply( const ParamSet* set, IFilterType t ) const
	{
		Image * in = set->arg<Image*>( 0 );
		Image * guide = set->arg<Image*>( 1 );
		Image * out = set->arg<Image*>( 2 );
		int radius = set->arg<int>( 3 );
		float epsilon = set->arg<float>( 4 );


		switch ( t ) {
			case IFILTER_OPENCL:
				this->apply( *out, *in, guide?*guide:*in, radius, epsilon );
				break;
			default:
				throw CVTException( "Not implemented" );
		}
	}

}
