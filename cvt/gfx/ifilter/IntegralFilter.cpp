#include <cvt/gfx/ifilter/IntegralFilter.h>

#include <cvt/cl/kernel/prefixsum/prefixsum_pblock.h>
#include <cvt/cl/kernel/prefixsum/prefixsum_pblock_mul2.h>
#include <cvt/cl/kernel/prefixsum/prefixsum_pblock_sqr.h>
#include <cvt/cl/kernel/prefixsum/prefixsum_horiz.h>
#include <cvt/cl/kernel/prefixsum/prefixsum_vert.h>
#include <cvt/cl/kernel/prefixsum/prefixsum_block2.h>

namespace cvt {
	static ParamInfoTyped<Image*> pin( "Input", true );
	static ParamInfoTyped<Image*> pin2( "Input2", true );
	static ParamInfoTyped<Image*> pout( "Output", false );

	static ParamInfo * _params[ 3 ] = {
		&pin,
		&pin2,
		&pout,
	};

	IntegralFilter::IntegralFilter() :
		IFilter( "IntegralFilter", _params, 3, IFILTER_CPU | IFILTER_OPENCL ),
		_clprefixsum_blockp( _prefixsum_pblock_source, "prefixsum_pblock" ),
		_clprefixsum_blockp_sqr( _prefixsum_pblock_sqr_source, "prefixsum_pblock_sqr" ),
		_clprefixsum_blockp_mul2( _prefixsum_pblock_mul2_source, "prefixsum_pblock_mul2" ),
		_clprefixsum_horiz( _prefixsum_horiz_source, "prefixsum_horiz" ),
		_clprefixsum_vert( _prefixsum_vert_source, "prefixsum_vert" ),
		_clprefixsum_block2( _prefixsum_block2_source, "prefixsum_block2" ),
		_blocksize( 16 )
	{
		size_t maxwg = Math::max( _clprefixsum_blockp.maxWorkGroupSize(), _clprefixsum_block2.maxWorkGroupSize() );
		while( _blocksize * _blocksize > maxwg  ) {
			_blocksize >>= 1;
		}
	}

	void IntegralFilter::apply( Image& dst, const Image& src, const Image* src2 ) const
	{
		// FIXME: hardcoded work-group size

		if( src2 ) {
			if( &src == src2 ) {
				_clprefixsum_blockp_sqr.setArg( 0, dst );
				_clprefixsum_blockp_sqr.setArg( 1, src );
				_clprefixsum_blockp_sqr.setArg( 2, CLLocalSpace( sizeof( cl_float4 ) * _blocksize * _blocksize ) );
				_clprefixsum_blockp_sqr.run( CLNDRange( Math::pad( src.width(), _blocksize ), Math::pad( src.height(), _blocksize ) ), CLNDRange( _blocksize, _blocksize ) );
			} else {
				_clprefixsum_blockp_mul2.setArg( 0, dst );
				_clprefixsum_blockp_mul2.setArg( 1, src );
				_clprefixsum_blockp_mul2.setArg( 2, *src2 );
				_clprefixsum_blockp_mul2.setArg( 3, CLLocalSpace( sizeof( cl_float4 ) * _blocksize * _blocksize ) );
				_clprefixsum_blockp_mul2.run( CLNDRange( Math::pad( src.width(), _blocksize ), Math::pad( src.height(), _blocksize ) ), CLNDRange( _blocksize, _blocksize ) );
			}
		} else {
			_clprefixsum_blockp.setArg( 0, dst );
			_clprefixsum_blockp.setArg( 1, src );
			_clprefixsum_blockp.setArg( 2, CLLocalSpace( sizeof( cl_float4 ) * _blocksize * _blocksize ) );
			_clprefixsum_blockp.run( CLNDRange( Math::pad( src.width(), _blocksize ), Math::pad( src.height(), _blocksize ) ), CLNDRange( _blocksize, _blocksize ) );

		}

		_clprefixsum_horiz.setArg( 0, dst );
		_clprefixsum_horiz.setArg( 1, dst );
		_clprefixsum_horiz.setArg<int>( 2, _blocksize );
		_clprefixsum_horiz.run( CLNDRange( Math::pad( dst.height(), _blocksize ) ), CLNDRange( _blocksize ) );

		_clprefixsum_vert.setArg( 0, dst );
		_clprefixsum_vert.setArg( 1, dst );
		_clprefixsum_vert.setArg<int>( 2, _blocksize );
		_clprefixsum_vert.run( CLNDRange( Math::pad( dst.width(), _blocksize ) ), CLNDRange( _blocksize ) );

		_clprefixsum_block2.setArg( 0, dst );
		_clprefixsum_block2.setArg( 1, dst );
		_clprefixsum_block2.setArg( 2, CLLocalSpace( sizeof( cl_float4 ) * _blocksize ) );
		_clprefixsum_block2.setArg( 3, CLLocalSpace( sizeof( cl_float4 ) * _blocksize ) );
		_clprefixsum_block2.run( CLNDRange( Math::pad( dst.width(), _blocksize ), Math::pad( dst.height(), _blocksize ) ), CLNDRange( _blocksize, _blocksize ) );
	}

	void IntegralFilter::apply( const ParamSet* set, IFilterType t ) const
	{
		Image * in = set->arg<Image*>( 0 );
		Image * in2 = set->arg<Image*>( 1 );
		Image * out = set->arg<Image*>( 2 );

		switch ( t ) {
			case IFILTER_OPENCL:
				this->apply( *out, *in, in2 );
				break;
			default:
				throw CVTException( "Not implemented" );
		}
	}

}
