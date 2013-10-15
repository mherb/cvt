/*
   The MIT License (MIT)

   Copyright (c) 2011 - 2013, Philipp Heise and Sebastian Klose

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#include <cvt/gfx/ifilter/MorphErode.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/util/ScopedBuffer.h>
#include <cvt/util/ParamSet.h>
#include <cvt/math/Math.h>


namespace cvt {
	static ParamInfoTyped<Image*> pout( "Output", false );
	static ParamInfoTyped<Image*> pin( "Input", true );
	static ParamInfoTyped<uint32_t> pradius( "Radius", true );

	static ParamInfo * _params[ 3 ] = {
		&pout,
		&pin,
		&pradius
	};

	MorphErode::MorphErode() :
		IFilter( "MorphErode", _params, 3, IFILTER_CPU )
	{
	}

	void MorphErode::apply( Image& dst, const Image& src, size_t radius, IFilterType type ) const
	{
		if( src.channels() != 1 || type != IFILTER_CPU )
			throw CVTException( "Not implemented" );

		dst.reallocate( src.width(), src.height(), src.format() );

		switch( src.format().formatID ) {
			case IFORMAT_GRAY_UINT8:
				erodeU8( dst, src, radius );
				return;
			case IFORMAT_GRAY_UINT16:
				erodeU16( dst, src, radius );
				break;
			case IFORMAT_GRAY_FLOAT:
				erodeF( dst, src, radius );
				break;
			default:
				throw CVTException( "Not implemented" );
		}

	}

	void MorphErode::erodeU8( Image& dst, const Image& src, size_t radius ) const
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
			simd->erodeSpanU8( buf[ i ], mapsrc.ptr(), w, radius );
			mapsrc++;
		}

		/* upper border */
		simd->MinValueVertU8( mapdst.ptr(), ( const uint8_t** ) buf, radius + 1, w );
		for( i = 1; i < radius; i++ ) {
			uint8_t* prev = mapdst.ptr();
			mapdst++;
			simd->MinValueU8( mapdst.ptr(), ( const uint8_t* ) prev, ( const uint8_t* ) buf[ radius + 1 + i ], w );
		}
		mapdst++;

		/* center */
		curbuf = 0;
		i = h - 2 * radius - 1;
		simd->MinValueVertU8( mapdst.ptr(), ( const uint8_t** ) buf, step, w );
		mapdst++;
		while( i-- ) {
			simd->erodeSpanU8( buf[ curbuf ], mapsrc.ptr(), w, radius );
			curbuf = ( curbuf + 1 ) % step;
			simd->MinValueVertU8( mapdst.ptr(), ( const uint8_t** ) buf, step, w );
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
			simd->MinValueVertU8( mapdst.ptr(), ( const uint8_t** ) ( buf2 + i ), step - i, w );
			mapdst++;
		}
	}

	void MorphErode::erodeU16( Image& dst, const Image& src, size_t radius ) const
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
			simd->erodeSpanU16( buf[ i ], mapsrc.ptr(), w, radius );
			mapsrc++;
		}

		/* upper border */
		simd->MinValueVertU16( mapdst.ptr(), ( const uint16_t** ) buf, radius + 1, w );
		for( i = 1; i < radius; i++ ) {
			uint16_t* prev = mapdst.ptr();
			mapdst++;
			simd->MinValueU16( mapdst.ptr(), ( const uint16_t* ) prev, ( const uint16_t* ) buf[ radius + 1 + i ], w );
		}
		mapdst++;

		/* center */
		curbuf = 0;
		i = h - 2 * radius - 1;
		simd->MinValueVertU16( mapdst.ptr(), ( const uint16_t** ) buf, step, w );
		mapdst++;
		while( i-- ) {
			simd->erodeSpanU16( buf[ curbuf ], mapsrc.ptr(), w, radius );
			curbuf = ( curbuf + 1 ) % step;
			simd->MinValueVertU16( mapdst.ptr(), ( const uint16_t** ) buf, step, w );
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
			simd->MinValueVertU16( mapdst.ptr(), ( const uint16_t** ) ( buf2 + i ), step - i, w );
			mapdst++;
		}
	}

	void MorphErode::erodeF( Image& dst, const Image& src, size_t radius ) const
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
			simd->erodeSpan1f( buf[ i ], mapsrc.ptr(), w, radius );
			mapsrc++;
		}

		/* upper border */
		simd->MinValueVert1f( mapdst.ptr(), ( const float** ) buf, radius + 1, w );
		for( i = 1; i < radius; i++ ) {
			float* prev = mapdst.ptr();
			mapdst++;
			simd->MinValue1f( mapdst.ptr(), ( const float* ) prev, ( const float* ) buf[ radius + 1 + i ], w );
		}
		mapdst++;

		/* center */
		curbuf = 0;
		i = h - 2 * radius - 1;
		simd->MinValueVert1f( mapdst.ptr(), ( const float** ) buf, step, w );
		mapdst++;
		while( i-- ) {
			simd->erodeSpan1f( buf[ curbuf ], mapsrc.ptr(), w, radius );
			curbuf = ( curbuf + 1 ) % step;
			simd->MinValueVert1f( mapdst.ptr(), ( const float** ) buf, step, w );
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
			simd->MinValueVert1f( mapdst.ptr(), ( const float** ) ( buf2 + i ), step - i, w );
			mapdst++;
		}
	}


	void MorphErode::apply( const ParamSet* set, IFilterType t ) const
	{
		Image * out = set->arg<Image*>( 0 );
		Image * in = set->arg<Image*>( 1 );
		uint32_t radius = set->arg<uint32_t>( 2 );

		apply( *out, *in, radius, t );
	}

}
