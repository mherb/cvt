#include "TVL1Flow.h"

#include <cvt/cl/kernel/clear.h>
#include <cvt/cl/kernel/median3.h>
#include <cvt/cl/kernel/pyr/pyrupmul.h>
#include <cvt/cl/kernel/pyr/pyrdown_binom3.h>
#include <cvt/cl/kernel/pyr/pyrdown_binom5.h>
#include <cvt/cl/kernel/pyr/pyrdown.h>
#include <cvt/cl/kernel/tvl1flow/flowthreshold.h>

#include <cvt/vision/Flow.h>

namespace cvt {
		static ParamInfoTyped<Image*> pin( "Input", true );
		static ParamInfoTyped<Image*> pout( "Output", false );

		static ParamInfo * _params[ 2 ] = {
			&pin,
			&pout,
		};

		TVL1Flow::TVL1Flow( float scalefactor, size_t levels ) : IFilter( "TVL1Flow", _params, 2, IFILTER_OPENCL ),
			_toggle( false ),
			_scalefactor( scalefactor ),
			_levels( levels ),
			_pyrup( _pyrupmul_source, "pyrup_mul" ),
			_pyrdown( _pyrdown_source, "pyrdown" ),
			_flowthreshold( _flowthreshold_source, "flow_threshold" ),
			_clear( _clear_source, "clear" ),
			_median3( _median3_source, "median3" ),
			_lambda( 70.0f )
		{
			_pyr[ 0 ] = new Image[ levels ];
			_pyr[ 1 ] = new Image[ levels ];
		}

		TVL1Flow::~TVL1Flow()
		{
			delete[ ] _pyr[ 0 ];
			delete[ ] _pyr[ 1 ];
		}

		void TVL1Flow::apply( Image& output, const Image& src1, const Image& src2 )
		{
			if( src1.width() != src2.width() ||
			    src1.height() != src2.height() )
				throw CVTException( "Image do not match in size!" );

			fillPyramidCL( src1, 0 );
			fillPyramidCL( src2, 1 );

#define THETA 0.20f
#define PYRUPWGSIZE 16
#define FLOWTHWGSIZE 16
#define MED3WGSIZE 16

			Image* flow = NULL;
			Image* flowtmp = NULL;
			Image* flowold = NULL;
			for( int l = _levels - 1; l >= 0; l-- ) {
				if( flowold )
					delete flowold;
				if( flow )
					flowold = flow;
				if( flowtmp )
					delete flowtmp;

				flow = new Image( _pyr[ 0 ][ l ].width(), _pyr[ 0 ][ l ].height(), IFormat::GRAYALPHA_FLOAT, IALLOCATOR_CL );
				flowtmp = new Image( _pyr[ 0 ][ l ].width(), _pyr[ 0 ][ l ].height(), IFormat::GRAYALPHA_FLOAT, IALLOCATOR_CL );

				if( flowold ) {
					_pyrup.setArg( 0, *flowtmp );
					_pyrup.setArg( 1, *flowold );
					_pyrup.setArg( 2, 1.0f / _scalefactor );
					_pyrup.run( CLNDRange( Math::pad( flow->width(), PYRUPWGSIZE ), Math::pad( flow->height(), PYRUPWGSIZE ) ), CLNDRange( PYRUPWGSIZE, PYRUPWGSIZE ) );

					_median3.setArg( 0, *flow );
					_median3.setArg( 1, *flowtmp );
					_median3.setArg( 2, CLLocalSpace( sizeof( cl_float4 ) * ( MED3WGSIZE + 2 ) * ( MED3WGSIZE + 2 ) ) );
					_median3.run( CLNDRange( Math::pad( flowtmp->width(), MED3WGSIZE ), Math::pad( flowtmp->height(), MED3WGSIZE ) ), CLNDRange( MED3WGSIZE, MED3WGSIZE ) );
//					_gf.apply( *flowtmp, *flow, _pyr[ 0 ][ l ], 20, 0.01f );

//					*flow = *flowtmp;
				} else {
					_clear.setArg( 0, *flow );
					_clear.run( CLNDRange(Math::pad( flow->width(), 16 ), Math::pad( flow->height(), 16 ) ), CLNDRange( 16, 16 ));
				}

				for( int i = 0; i < 15; i++  ) {
					_flowthreshold.setArg( 0, *flowtmp );
					_flowthreshold.setArg( 1, *flow );
					_flowthreshold.setArg( 2, _pyr[ 0 ][ l ] );
					_flowthreshold.setArg( 3, _pyr[ 1 ][ l ] );
					_flowthreshold.setArg( 4, _lambda * THETA );
					_flowthreshold.setArg( 5, 5 );
					_flowthreshold.setArg( 6, CLLocalSpace( sizeof( cl_float4 ) * ( FLOWTHWGSIZE + 2 ) * ( FLOWTHWGSIZE + 2 ) ) );
					_flowthreshold.run( CLNDRange( Math::pad( flow->width(), FLOWTHWGSIZE ), Math::pad( flow->height(), FLOWTHWGSIZE ) ), CLNDRange( FLOWTHWGSIZE, FLOWTHWGSIZE ) );
					_median3.setArg( 0, *flow );
					_median3.setArg( 1, *flowtmp );
					_median3.setArg( 2, CLLocalSpace( sizeof( cl_float4 ) * ( MED3WGSIZE + 2 ) * ( MED3WGSIZE + 2 ) ) );
					_median3.run( CLNDRange( Math::pad( flowtmp->width(), MED3WGSIZE ), Math::pad( flowtmp->height(), MED3WGSIZE ) ), CLNDRange( MED3WGSIZE, MED3WGSIZE ) );
					*flowtmp = *flow;
					_rof.apply( *flow, *flowtmp, THETA, 10 );

//					_gf.apply( *flowtmp, *flow, _pyr[ 0 ][ l ], 10, 0.0001f );
//					*flow = *flowtmp;
			/*		{
						Image ccode( output.width(), output.height(), IFormat::BGRA_FLOAT );
						Flow::colorCode( ccode, *flow, 2.0f );
						String fname;
						ccode.save( "curflow.png" );
					}*/

				}

			}
			if( flowold )
				delete flowold;

			_median3.setArg( 0, *flowtmp );
			_median3.setArg( 1, *flow );
			_median3.setArg( 2, CLLocalSpace( sizeof( cl_float4 ) * ( MED3WGSIZE + 2 ) * ( MED3WGSIZE + 2 ) ) );
			_median3.run( CLNDRange( Math::pad( flowtmp->width(), MED3WGSIZE ), Math::pad( flowtmp->height(), MED3WGSIZE ) ), CLNDRange( MED3WGSIZE, MED3WGSIZE ) );

//			_gf.apply( *flow, *flowtmp, _pyr[ 0 ][ 0 ], 10, 0.01f );
			*flow = *flowtmp;
			if( flowtmp )
				delete flowtmp;

			output.reallocate( *flow );
			output = *flow;
			delete flow;
		}

		void TVL1Flow::fillPyramidCL( const Image& img, size_t index )
		{
			Image* pyr = _pyr[ index ];

#define PYRWGSIZE 16

			pyr[ 0 ].reallocate( img.width(), img.height(), IFormat::RGBA_UINT8, IALLOCATOR_CL );
			img.convert( pyr[ 0 ] );
			for( size_t l = 1; l < _levels; l++ ) {
				pyr[ l ].reallocate( pyr[ l - 1 ].width() * _scalefactor, pyr[ l - 1 ].height() * _scalefactor, IFormat::GRAY_FLOAT, IALLOCATOR_CL );
				_pyrdown.setArg( 0, pyr[ l ] );
				_pyrdown.setArg( 1, pyr[ l - 1 ] );
//				_pyrdown.setArg( 2, CLLocalSpace( sizeof( cl_float4 ) * ( PYRWGSIZE + 4 ) * ( PYRWGSIZE + 4 ) ) );
//				_pyrdown.setArg( 3, CLLocalSpace( sizeof( cl_float4 ) * ( PYRWGSIZE + 4 ) * PYRWGSIZE ) );
				_pyrdown.run( CLNDRange( Math::pad( pyr[ l ].width(), PYRWGSIZE ), Math::pad( pyr[ l ].height(), PYRWGSIZE ) ), CLNDRange( PYRWGSIZE, PYRWGSIZE ) );
			}
		}
}
