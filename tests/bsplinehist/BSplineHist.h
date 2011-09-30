#ifndef CVT_BSPLINEHIST_H
#define CVT_BSPLINEHIST_H

#include <cvt/gfx/Image.h>

namespace cvt {
	class BSplineHist {
		public:
			BSplineHist( size_t bins ) {
				_hist = new float[ bins + 4 ];
				_bins = bins;
				//for( float x = -2.5f; x <= 2.5f; x+= 0.01f )
				//	std::cout << x << " " << cbspline( x ) << std::endl;
			}

			~BSplineHist() {
				delete[] _hist;
			}

			void update( const Image& i );
			void dump();
		private:
			inline float cbspline( float t ) const;
			inline void normalize();

			size_t _bins;
			float* _hist;
	};


	inline float BSplineHist::cbspline( float t ) const
	{
		t = Math::abs( t );
		if( t > 2.0f ) return 0.0f;
		if( t > 1.0f ) {
			t = ( 2.0f - t );
			return t * t * t;
		}
		return ( 4.0f - 6.0f * t * t + 3.0f * t * t *t );
	}

	inline void BSplineHist::update( const Image& img )
	{
		// Assume i is float gray
		for( size_t i = 0; i < _bins + 4; i++ )
			_hist[ i ] = 0.0f;

		size_t stride;
		const uint8_t* ptr;
		const uint8_t* map;
		size_t w, h;

		w = img.width();
		h = img.height();
		map = img.map( &stride );
		ptr = map;
		while( h-- ) {
			const float* p = ( const float * ) ptr;
			size_t n = w;
			while( n-- ) {
				float fidx =  *p++ * ( float ) ( _bins - 1 ) + 2.0f;
				//std::cout << fidx << std::endl;
				int idx = ( int ) fidx;
				_hist[ idx ] += cbspline( fidx - ( float ) idx );
				_hist[ idx + 1 ] += cbspline( fidx - ( float ) ( idx + 1 ) );
				_hist[ idx - 1 ] += cbspline( fidx - ( float ) ( idx - 1 ) );
				_hist[ idx + 2 ] += cbspline( fidx - ( float ) ( idx + 2 ) );
				_hist[ idx - 2 ] += cbspline( fidx - ( float ) ( idx - 2 ) );
			}
			ptr += stride;
		}

		img.unmap( map );
		normalize();
	}

	inline void BSplineHist::normalize()
	{
		float sum = 0;
		for( size_t i = 0; i < _bins + 4; i++ )
			sum += _hist[ i ];
		for( size_t i = 0; i < _bins + 4; i++ )
			_hist[ i ] /= sum;
	}

	inline void BSplineHist::dump()
	{
		for( size_t i = 0; i < _bins + 4; i++ ) {
			std::cout << i << " " << _hist[ i ] << std::endl;
		}
	}
}

#endif
