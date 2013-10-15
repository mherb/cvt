#include <cvt/gfx/ifilter/AdaptiveThreshold.h>
#include <cvt/gfx/IMapScoped.h>

#include <cvt/cl/kernel/boxfilter/boxfilter_prefixsum.h>
#include <cvt/cl/kernel/boxfilter/boxfilter.h>

namespace cvt {
	static ParamInfoTyped<Image*> pout( "Output", false );
	static ParamInfoTyped<Image*> pin( "Input", true );
	static ParamInfoTyped<Image*> pinmean( "Input Mean", true );
	static ParamInfoTyped<float>  pthreshold( "Threshold", 0.0f, 1.0f, 0.05f, true );

	static ParamInfo * _params[ 4 ] = {
		&pout,
		&pin,
		&pinmean,
		&pthreshold
	};

	AdaptiveThreshold::AdaptiveThreshold() :
		IFilter( "AdaptiveThreshold", _params, 4, IFILTER_CPU )
	{
	}

	void AdaptiveThreshold::apply( Image& dst, const Image& src, const Image& srcmean, const float threshold, IFilterType type ) const
	{
		if( type != IFILTER_CPU || src.format().formatID != IFORMAT_GRAY_FLOAT || srcmean.format().formatID != IFORMAT_GRAY_FLOAT  )
			throw CVTException( "Not implemented" );

		if( dst.width() != src.width()
		   || dst.height() != src.height()
		   || dst.format().formatID != IFORMAT_GRAY_FLOAT
		   || dst.format().formatID != IFORMAT_GRAY_UINT8 )
			dst.reallocate( src.width(), src.height(), IFormat::GRAY_UINT8 );

		size_t w = src.width();
		size_t h = src.height();
		SIMD* simd = SIMD::instance();

		if( dst.format().formatID == IFORMAT_GRAY_UINT8 ) {
			cvt::IMapScoped<uint8_t> mapdst( dst );
			cvt::IMapScoped<const float> mapsrc( src );
			cvt::IMapScoped<const float> mapsrcmean( srcmean );

			while( h-- ) {
				simd->adaptiveThreshold1_f_to_u8( mapdst.ptr(), mapsrc.ptr(), mapsrcmean.ptr(), w, threshold );
				mapdst++;
				mapsrc++;
				mapsrcmean++;
			}
		} else {
			cvt::IMapScoped<float> mapdst( dst );
			cvt::IMapScoped<const float> mapsrc( src );
			cvt::IMapScoped<const float> mapsrcmean( srcmean );

			while( h-- ) {
				simd->adaptiveThreshold1_f_to_f( mapdst.ptr(), mapsrc.ptr(), mapsrcmean.ptr(), w, threshold );
				mapdst++;
				mapsrc++;
				mapsrcmean++;
			}

		}
	}

	void AdaptiveThreshold::apply( const ParamSet* set, IFilterType t ) const
	{
		Image * out = set->arg<Image*>( 0 );
		Image * in = set->arg<Image*>( 1 );
		Image * inmean = set->arg<Image*>( 2 );
		float threshold = set->arg<float>( 3 );

		apply( *out, *in, *inmean, threshold, t );
	}

}
