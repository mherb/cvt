#include <cvt/gfx/ifilter/StereoGCVFilter.h>
#include <cvt/cl/kernel/stereogcv/costdepth.h>
#include <cvt/cl/kernel/stereogcv/costmin.h>
#include <cvt/cl/kernel/stereogcv/costdepthconv.h>
#include <cvt/cl/kernel/fill.h>

namespace cvt {
	static ParamInfoTyped<Image*> pin0( "Input 0", true );
	static ParamInfoTyped<Image*> pin1( "Input 1", true );
	static ParamInfoTyped<Image*> pout( "Output", false );
	static ParamInfoTyped<float>  pdmin( "Depth min.", true );
	static ParamInfoTyped<float>  pdmax( "Depth max.", true );
	static ParamInfoTyped<float>  pdt( "Depth stepsize", true );

	static ParamInfo * _params[ 6 ] = {
		&pin0,
		&pin1,
		&pout,
		&pdmin,
		&pdmax,
		&pdt
	};

	StereoGCVFilter::StereoGCVFilter() :
		IFilter( "StereoGCVFilter", _params, 6, IFILTER_CPU | IFILTER_OPENCL ),
		_cldepthcost( _costdepth_source, "stereogcv_costdepth" ),
		_cldepthmin( _costmin_source, "stereogcv_costmin" ),
		_clfill( _fill_source, "fill" ),
		_clcdconv( _costdepthconv_source, "stereogcv_costdepthconv" )
	{
	}


	void StereoGCVFilter::apply( Image& dst, const Image& cam0, const Image& cam1, const Matrix4f& T, float dmin, float dmax, float dt ) const
	{
		// FIXME: use buffer to perform calculation instead of CLImage
//		CLBuffer buf( sizeof( cl_float ) * cam0.width() * cam0.height() * zsize );
//		size_t zsize = ( dmax - dmin ) / dt;

		Image cost( cam0.width(), cam0.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL ); //FIXME: just use GRAYALPHA
		Image costgf( cam0.width(), cam0.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL ); //FIXME: just use GRAYALPHA
		Image c0( cam0.width(), cam0.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL ); //FIXME: just use GRAYALPHA
		Image c1( cam0.width(), cam0.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL ); //FIXME: just use GRAYALPHA
		Image* c[ 2 ] = { &c0, &c1 };
		int index = 0;
		float fill[ 4 ] = { 5.0f, 0.0f, 0.0f, 0.0f };

		CLBuffer clproj( ( void* ) T.ptr(), sizeof( float ) * 12 );

		CLNDRange global( Math::pad16( cam0.width() ), Math::pad16( cam0.height() ) );

		_clfill.setArg( 0, *c[ !index ] );
		_clfill.setArg( 1, sizeof( cl_float ) * 4, fill );
		_clfill.run( global, CLNDRange( 16, 16 ) );

		for( float d = dmin; d <= dmax; d += dt ) {
			_cldepthcost.setArg( 0, cost );
			_cldepthcost.setArg( 1, cam0 );
			_cldepthcost.setArg( 2, cam1 );
			_cldepthcost.setArg( 3, clproj );
			_cldepthcost.setArg( 4, d );
			_cldepthcost.run( global, CLNDRange( 16, 16 ) );

			_gf.apply( costgf, cost, cam0, 20.0f, 0.01f );

			_cldepthmin.setArg( 0, *c[ index ] );
			_cldepthmin.setArg( 1, costgf );
			_cldepthmin.setArg( 2, *c[ !index ] );
			_cldepthmin.setArg( 3, d );
			_cldepthmin.run( global, CLNDRange( 16, 16 ) );

			index = !index;
		}

		dst.reallocate( cam0.width(), cam0.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL );

		_clcdconv.setArg( 0, dst );
		_clcdconv.setArg( 1, *c[ !index ] );
		_clcdconv.setArg( 2, 1.0f / ( dmax - dmin ) );
		_clcdconv.setArg( 3, -dmin / ( dmax - dmin ));
		_clcdconv.run( global, CLNDRange( 16, 16 ) );


	}

}


