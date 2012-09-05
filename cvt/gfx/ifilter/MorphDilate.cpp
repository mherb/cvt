/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/gfx/ifilter/MorphDilate.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/util/ScopedBuffer.h>
#include <cvt/math/Math.h>


namespace cvt {
	static ParamInfoTyped<Image*> pout( "Output", false );
	static ParamInfoTyped<Image*> pin( "Input", true );
	static ParamInfoTyped<size_t> pradius( "Radius", true );

	static ParamInfo * _params[ 3 ] = {
		&pout,
		&pin,
		&pradius
	};

	MorphDilate::MorphDilate() :
		IFilter( "MorphDilate", _params, 3, IFILTER_CPU )
	{
	}

	void MorphDilate::apply( Image& dst, const Image& src, size_t radius, IFilterType type ) const
	{
		if( src.channels() != 1 || type != IFILTER_CPU )
			throw CVTException( "Not implemented" );

		dst.reallocate( src.width(), src.height(), src.format() );

		switch( src.format().formatID ) {
			case IFORMAT_GRAY_UINT8:
				dilateU8( dst, src, radius );
				return;
			case IFORMAT_GRAY_UINT16:
				dilateU16( dst, src, radius );
				break;
			case IFORMAT_GRAY_FLOAT:
				dilateF( dst, src, radius );
				break;
			default:
				throw CVTException( "Not implemented" );
		}

	}

	void MorphDilate::dilateU8( Image& dst, const Image& src, size_t radius ) const
	{
		size_t step = radius * 2 + 1;
		size_t curbuf;
		size_t w = src.width();
		size_t h = src.height();
		SIMD* simd = SIMD::instance();
		size_t bstride = Math::pad16( w );
		uint8_t** buf;
		size_t i;

		IMapScoped<uint8_t> mapdst( dst );
		IMapScoped<const uint8_t> mapsrc( src );

		/* allocate and fill buffer */
		ScopedBuffer<uint8_t,true> bufmem( bstride * step );
		ScopedBuffer<uint8_t*,true> bufptr( step );

		buf = bufptr.ptr();
		buf[ 0 ] = bufmem.ptr();
		for( i = 0; i < step; i++ ) {
			if( i != 0 )
				buf[ i ] = buf[ i - 1 ] + bstride;
			simd->dilateSpanU8( buf[ i ], mapsrc.ptr(), w, radius );
			mapsrc++;
		}

		/* upper border */
		simd->MaxValueVertU8( mapdst.ptr(), ( const uint8_t** ) buf, radius + 1, w );
		for( i = 1; i < radius; i++ ) {
			uint8_t* prev = mapdst.ptr();
			mapdst++;
			simd->MaxValueU8( mapdst.ptr(), ( const uint8_t* ) prev, ( const uint8_t* ) buf[ radius + 1 + i ], w );
		}
		mapdst++;

		/* center */
		curbuf = 0;
		i = h - 2 * radius - 1;
		simd->MaxValueVertU8( mapdst.ptr(), ( const uint8_t** ) buf, step, w );
		mapdst++;
		while( i-- ) {
			simd->dilateSpanU8( buf[ curbuf ], mapsrc.ptr(), w, radius );
			curbuf = ( curbuf + 1 ) % step;
			simd->MaxValueVertU8( mapdst.ptr(), ( const uint8_t** ) buf, step, w );
			mapdst++;
			mapsrc++;
		}

		/* lower border */
		/* reorder buffer */
		ScopedBuffer<uint8_t*,true> bufptr2( step );
		uint8_t** buf2 = bufptr2.ptr();
		for( i = 0; i < step; i++ )
			buf2[ i ] = buf[ ( curbuf + i ) % step ];

		for( i = 1; i <= radius; i++ ) {
			simd->MaxValueVertU8( mapdst.ptr(), ( const uint8_t** ) ( buf2 + i ), step - i, w );
			mapdst++;
		}
	}

	void MorphDilate::dilateU16( Image& dst, const Image& src, size_t radius ) const
	{
		size_t step = radius * 2 + 1;
		size_t curbuf;
		size_t w = src.width();
		size_t h = src.height();
		SIMD* simd = SIMD::instance();
		size_t bstride = Math::pad( w, 8 );
		uint16_t** buf;
		size_t i;

		IMapScoped<uint16_t> mapdst( dst );
		IMapScoped<const uint16_t> mapsrc( src );

		/* allocate and fill buffer */
		ScopedBuffer<uint16_t,true> bufmem( bstride * step );
		ScopedBuffer<uint16_t*,true> bufptr( step );

		buf = bufptr.ptr();
		buf[ 0 ] = bufmem.ptr();
		for( i = 0; i < step; i++ ) {
			if( i != 0 )
				buf[ i ] = buf[ i - 1 ] + bstride;
			simd->dilateSpanU16( buf[ i ], mapsrc.ptr(), w, radius );
			mapsrc++;
		}

		/* upper border */
		simd->MaxValueVertU16( mapdst.ptr(), ( const uint16_t** ) buf, radius + 1, w );
		for( i = 1; i < radius; i++ ) {
			uint16_t* prev = mapdst.ptr();
			mapdst++;
			simd->MaxValueU16( mapdst.ptr(), ( const uint16_t* ) prev, ( const uint16_t* ) buf[ radius + 1 + i ], w );
		}
		mapdst++;

		/* center */
		curbuf = 0;
		i = h - 2 * radius - 1;
		simd->MaxValueVertU16( mapdst.ptr(), ( const uint16_t** ) buf, step, w );
		mapdst++;
		while( i-- ) {
			simd->dilateSpanU16( buf[ curbuf ], mapsrc.ptr(), w, radius );
			curbuf = ( curbuf + 1 ) % step;
			simd->MaxValueVertU16( mapdst.ptr(), ( const uint16_t** ) buf, step, w );
			mapdst++;
			mapsrc++;
		}

		/* lower border */
		/* reorder buffer */
		ScopedBuffer<uint16_t*,true> bufptr2( step );
		uint16_t** buf2 = bufptr2.ptr();
		for( i = 0; i < step; i++ )
			buf2[ i ] = buf[ ( curbuf + i ) % step ];

		for( i = 1; i <= radius; i++ ) {
			simd->MaxValueVertU16( mapdst.ptr(), ( const uint16_t** ) ( buf2 + i ), step - i, w );
			mapdst++;
		}
	}

	void MorphDilate::dilateF( Image& dst, const Image& src, size_t radius ) const
	{
		size_t step = radius * 2 + 1;
		size_t curbuf;
		size_t w = src.width();
		size_t h = src.height();
		SIMD* simd = SIMD::instance();
		size_t bstride = Math::pad( w, 4 );
		float** buf;
		size_t i;

		IMapScoped<float> mapdst( dst );
		IMapScoped<const float> mapsrc( src );

		/* allocate and fill buffer */
		ScopedBuffer<float,true> bufmem( bstride * step );
		ScopedBuffer<float*,true> bufptr( step );

		buf = bufptr.ptr();
		buf[ 0 ] = bufmem.ptr();
		for( i = 0; i < step; i++ ) {
			if( i != 0 )
				buf[ i ] = buf[ i - 1 ] + bstride;
			simd->dilateSpan1f( buf[ i ], mapsrc.ptr(), w, radius );
			mapsrc++;
		}

		/* upper border */
		simd->MaxValueVert1f( mapdst.ptr(), ( const float** ) buf, radius + 1, w );
		for( i = 1; i < radius; i++ ) {
			float* prev = mapdst.ptr();
			mapdst++;
			simd->MaxValue1f( mapdst.ptr(), ( const float* ) prev, ( const float* ) buf[ radius + 1 + i ], w );
		}
		mapdst++;

		/* center */
		curbuf = 0;
		i = h - 2 * radius - 1;
		simd->MaxValueVert1f( mapdst.ptr(), ( const float** ) buf, step, w );
		mapdst++;
		while( i-- ) {
			simd->dilateSpan1f( buf[ curbuf ], mapsrc.ptr(), w, radius );
			curbuf = ( curbuf + 1 ) % step;
			simd->MaxValueVert1f( mapdst.ptr(), ( const float** ) buf, step, w );
			mapdst++;
			mapsrc++;
		}

		/* lower border */
		/* reorder buffer */
		ScopedBuffer<float*,true> bufptr2( step );
		float** buf2 = bufptr2.ptr();
		for( i = 0; i < step; i++ )
			buf2[ i ] = buf[ ( curbuf + i ) % step ];

		for( i = 1; i <= radius; i++ ) {
			simd->MaxValueVert1f( mapdst.ptr(), ( const float** ) ( buf2 + i ), step - i, w );
			mapdst++;
		}
	}


	void MorphDilate::apply( const ParamSet* set, IFilterType t ) const
	{
		Image * out = set->arg<Image*>( 0 );
		Image * in = set->arg<Image*>( 1 );
		size_t radius = set->arg<size_t>( 2 );

		apply( *out, *in, radius, t );
	}

}
