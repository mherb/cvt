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

#ifndef CVT_ICOLORCODE_H
#define CVT_ICOLORCODE_H

#include <cvt/gfx/IColorCodeMap.h>

namespace cvt {

	class IColorCode {
		public:
			static void colorCode( Image& dst, const Image& src, IColorCodeMap palette = ICOLORCODE_PM3D, float min = 0.0f, float max = 1.0f );

		private:
			IColorCode();
			IColorCode( const IColorCode& );

			static float zero( float ) { return 0.0f; }
			static float half( float ) { return 0.5f; }
			static float one( float ) { return 1.0f; }
			static float id( float x ) { return x; }
			static float sqr( float x ) { return x * x; }
			static float cube( float x ) { return x * x * x; }
			static float pow4( float x ) { return x * x * x * x; }
			static float sqrt( float x ) { return Math::sqrt( x ); }
			static float sqrt2( float x ) { return Math::sqrt( Math::sqrt( x ) ); }
			static float sin90( float x ) { return Math::sin( Math::deg2Rad( 90.0f * x ) ); }
			static float sin180( float x ) { return Math::sin( Math::deg2Rad( 180.0f * x ) ); }
			static float sin360( float x ) { return Math::sin( Math::deg2Rad( 360.0f * x ) ); }
			static float cos90( float x ) { return Math::cos( Math::deg2Rad( 90.0f * x ) ); }
			static float cos180( float x ) { return Math::cos( Math::deg2Rad( 180.0f * x ) ); }
			static float cos360( float x ) { return Math::cos( Math::deg2Rad( 360.0f * x ) ); }
			static float hot1( float x ) { return 3.0f * x; }
			static float hot2( float x ) { return 3.0f * x - 1.0f; }
			static float hot3( float x ) { return 3.0f * x - 2.0f; }
			static float f28( float x ) { return Math::abs( 1.5f * x - 0.5f ); }
			static float f33( float x ) { return Math::abs( 2.0f * x - 0.5f ); }

			static float sRGB2Linear( float x )
			{
				if( x < 0.04045f )
					return x / 12.92f;
				return Math::pow( ( x + 0.055f ) / 1.055f, 2.4f );
			}

			static float cgprint1( float x )
			{
				if( x <= 0.25 )
					return 0.0f;
				if( x >= 0.57 )
					return 1.0f;
				return x / 0.32f - 0.78125f;
			}

			static float cgprint2( float x )
			{
				if( x <= 0.42f )
					return 0.0f;
				if( x >= 0.92f )
					return 1.0f;
				return 2.0f * x - 0.84f;
			}

			static float cgprint3( float x )
			{
				if (x <= 0.42f )
					return x * 4.0f;
				else
					return (x <= 0.92) ? -2.0f * x + 1.84f : x / 0.08f - 11.5f;
			}


			template<typename T1, typename T2, typename T3>
			static void applyRGBFormula( Image& idst, const Image& isrc, float min, float max, T1 rop, T2 gop, T3 bop );
	};

	void IColorCode::colorCode( Image& dst, const Image& src, IColorCodeMap map, float min, float max )
	{
		switch( map ) {
			case ICOLORCODE_PM3D: return applyRGBFormula( dst, src, min, max, IColorCode::sqrt, IColorCode::cube, IColorCode::sin360 );
			case ICOLORCODE_HOT: return applyRGBFormula( dst, src, min, max, IColorCode::hot1, IColorCode::hot2, IColorCode::hot3 );
			case ICOLORCODE_GRAY: return applyRGBFormula( dst, src, min, max, IColorCode::id, IColorCode::id, IColorCode::id );
			case ICOLORCODE_OCEAN: return applyRGBFormula( dst, src, min, max, IColorCode::hot3, IColorCode::f28, IColorCode::id );
			case ICOLORCODE_COLORGRAYPRINT: return applyRGBFormula( dst, src, min, max, IColorCode::cgprint1, IColorCode::cgprint2, IColorCode::cgprint3 );
			case ICOLORCODE_RAINBOW: return applyRGBFormula( dst, src, min, max, IColorCode::f33, IColorCode::sin180, IColorCode::cos90 );
			case ICOLORCODE_AUTUMN: return applyRGBFormula( dst, src, min, max, IColorCode::one, IColorCode::id, IColorCode::zero );
		}
	}

	template<typename T1, typename T2, typename T3>
	void IColorCode::applyRGBFormula( Image& idst, const Image& isrc, float min, float max, T1 rop, T2 gop, T3 bop )
	{
		if( isrc.format() != IFormat::GRAY_FLOAT )
			throw CVTException( "Illegal data for color-coding" );

		float* pdst;
		const float* psrc;
		size_t w, h;
		float val;
		float low = Math::min( min, max );
		float high = Math::max( min, max );

		idst.reallocate( isrc.width(), isrc.height(), IFormat::RGBA_FLOAT );

		IMapScoped<float> dstmap( idst );
		IMapScoped<const float> srcmap( isrc );


		h = idst.height();
		while( h-- ) {
			pdst = dstmap.ptr();
			psrc = srcmap.ptr();

			w = idst.width();
			while( w-- ) {
				val = ( ( Math::clamp( *psrc++, low, high ) - min ) / ( max - min ) );
				*pdst++ = sRGB2Linear( Math::clamp( rop( val ), 0.0f, 1.0f ) );
				*pdst++ = sRGB2Linear( Math::clamp( gop( val ), 0.0f, 1.0f ) );
				*pdst++ = sRGB2Linear( Math::clamp( bop( val ), 0.0f, 1.0f ) );
				*pdst++ = 1.0f;	/* alpha is one */
			}
			dstmap++;
			srcmap++;
		}
	}

}

#endif
