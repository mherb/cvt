/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/gfx/ifilter/Threshold.h>
#include <cvt/gfx/IMapScoped.h>


namespace cvt {
	static ParamInfoTyped<Image*> pout( "Output", false );
	static ParamInfoTyped<Image*> pin( "Input", true );
	static ParamInfoTyped<float>  pthreshold( "Threshold", 0.0f, 1.0f, 0.05f, true );

	static ParamInfo * _params[ 3 ] = {
		&pout,
		&pin,
		&pthreshold
	};

	Threshold::Threshold() :
		IFilter( "Threshold", _params, 3, IFILTER_CPU )
	{
	}

	void Threshold::apply( Image& dst, const Image& src, const float threshold, IFilterType type ) const
	{
		if( type != IFILTER_CPU || !( src.format().formatID == IFORMAT_GRAY_FLOAT || src.format().formatID == IFORMAT_GRAY_UINT8 )  )
			throw CVTException( "Not implemented" );

		if( dst.width() != src.width()
		   || dst.height() != src.height()
		   || !( dst.format().formatID == IFORMAT_GRAY_FLOAT
		   || dst.format().formatID == IFORMAT_GRAY_UINT8 ) ) {
			dst.reallocate( src.width(), src.height(), IFormat::GRAY_UINT8 );
		}

		size_t w = src.width();
		size_t h = src.height();
		SIMD* simd = SIMD::instance();

		if( src.format().formatID == IFORMAT_GRAY_FLOAT ) {
			if( dst.format().formatID == IFORMAT_GRAY_UINT8 ) {
				cvt::IMapScoped<uint8_t> mapdst( dst );
				cvt::IMapScoped<const float> mapsrc( src );

				while( h-- ) {
					simd->threshold1_f_to_u8( mapdst.ptr(), mapsrc.ptr(), w, threshold );
					mapdst++;
					mapsrc++;
				}
			} else {
				cvt::IMapScoped<float> mapdst( dst );
				cvt::IMapScoped<const float> mapsrc( src );

				while( h-- ) {
					simd->threshold1_f_to_f( mapdst.ptr(), mapsrc.ptr(), w, threshold );
					mapdst++;
					mapsrc++;
				}

			}
		} else {
			uint8_t t = Math::round( threshold * 0xff );
			if( dst.format().formatID == IFORMAT_GRAY_UINT8 ) {
				cvt::IMapScoped<uint8_t> mapdst( dst );
				cvt::IMapScoped<const uint8_t> mapsrc( src );

				while( h-- ) {
					simd->threshold1_u8_to_u8( mapdst.ptr(), mapsrc.ptr(), w, t );
					mapdst++;
					mapsrc++;
				}
			} else {
				cvt::IMapScoped<float> mapdst( dst );
				cvt::IMapScoped<const uint8_t> mapsrc( src );

				while( h-- ) {
					simd->threshold1_u8_to_f( mapdst.ptr(), mapsrc.ptr(), w, t );
					mapdst++;
					mapsrc++;
				}

			}
		}
	}

	void Threshold::apply( const ParamSet* set, IFilterType t ) const
	{
		Image * out = set->arg<Image*>( 0 );
		Image * in = set->arg<Image*>( 1 );
		float threshold = set->arg<float>( 2 );

		apply( *out, *in, threshold, t );
	}

}
