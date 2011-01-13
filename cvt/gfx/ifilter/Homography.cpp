#include <cvt/gfx/ifilter/Homography.h>
#include <cvt/util/Exception.h>
#include <cvt/math/Math.h>

#include <iostream>

namespace cvt {

	ParamInfo* _params[ 4 ] = {
		new ParamInfoTyped<Image*>( "Input", true ),
		new ParamInfoTyped<Image*>( "Output", false ),
		new ParamInfoTyped<Matrix3f>( "H", true ),
		new ParamInfoTyped<Vector4f>( "Color", true )
	};

	Homography::Homography() : IFilter( "Homography", _params, 4, IFILTER_CPU )
	{
	}

	void Homography::applyFloat( Image& idst, const Image& isrc, const Matrix3f& H, const Color& ) const
	{
		const uint8_t* src;
		uint8_t* dst;
		uint8_t* dst2;
		size_t sstride;
		size_t dstride;
		float* pdst;
		size_t w, h, i;
		ssize_t sw, sh;

		float hx, hy;
		float data[ 4 ];
		float col[ 4 ] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float alpha, beta;
		size_t C;

		dst = idst.map( &dstride );
		src = isrc.map( &sstride );
		w = idst.width();
		h = idst.height();
		sw = ( ssize_t ) isrc.width();
		sh = ( ssize_t ) isrc.height();
		C = idst.channels();
		dst2 = dst;

		
		cvt::Vector3f point( 0.0f, 0.0f, 1.0f );
		cvt::Vector3f pprime;

		while( h-- ) {
			pdst = ( float * ) dst2;
			i = w;

			point[ 0 ] = 0.0f;
			while( i-- ) {
				ssize_t ix[ 2 ], iy[ 2 ];
				
				// Evaluate homography
				pprime = H * point;
				
				hx = pprime[ 0 ] / pprime[ 2 ];
				hy = pprime[ 1 ] / pprime[ 2 ];
				
				if( Math::abs( pprime[ 2 ] ) > Math::EPSILONF ) {
					alpha = hx - Math::floor( hx );
					beta  = hy - Math::floor( hy );
					
					// need to floor in order to get correct result for negative values
					ix[ 0 ] = ( ssize_t ) Math::floor( hx );
					iy[ 0 ] = ( ssize_t ) Math::floor( hy );
					ix[ 1 ] = ix[ 0 ] + 1;
					iy[ 1 ] = iy[ 0 ] + 1;

					for( size_t k = 0; k < C; k++ ) {
						if( iy[ 0 ] >= 0 && iy[ 0 ] < sh && ix[ 0 ] >= 0 && ix[ 0 ] < sw )
							data[ 0 ] = *( ( float* ) ( src + sstride * iy[ 0 ] + ( ix[ 0 ] * C + k ) * sizeof( float ) ) );
						else
							data[ 0 ] = col[ k ];

						if( iy[ 0 ] >= 0 && iy[ 0 ] < sh && ix[ 1 ] >= 0 && ix[ 1 ] < sw )
							data[ 1 ] = *( ( float* ) ( src + sstride * iy[ 0 ] + ( ix[ 1 ] * C + k ) * sizeof( float ) ) );
						else
							data[ 1 ] = col[ k ];

						if( iy[ 1 ] >= 0 && iy[ 1 ] < sh && ix[ 0 ] >= 0 && ix[ 0 ] < sw )
							data[ 2 ] = *( ( float* ) ( src + sstride * iy[ 1 ] + ( ix[ 0 ] * C + k ) * sizeof( float ) ) );
						else
							data[ 2 ] = col[ k ];

						if( iy[ 1 ] >= 0 && iy[ 1 ] < sh && ix[ 1 ] >= 0 && ix[ 1 ] < sw )
							data[ 3 ] = *( ( float* ) ( src + sstride * iy[ 1 ] + ( ix[ 1 ] * C + k ) * sizeof( float ) ) );
						else
							data[ 3 ] = col[ k ];

						data[ 0 ] = Math::mix( data[ 0 ], data[ 1 ], alpha );
						data[ 2 ] = Math::mix( data[ 2 ], data[ 3 ], alpha );
						*pdst++ = Math::mix( data[ 0 ], data[ 2 ], beta );
					}
				} else {
					for( size_t k = 0; k < C; k++ )
						*pdst++ = col[ k ];
				}
				point[ 0 ] += 1.0f;
			}
			point[ 1 ] += 1.0f;
			dst2 += dstride;
		}
		idst.unmap( dst );
		isrc.unmap( src );
	}

	void Homography::apply( Image& dst, const Image& src, const Matrix3f& H, const Color& c ) const
	{

		if( dst.format() != src.format() || 
			dst.format().type != IFORMAT_TYPE_FLOAT )
			throw CVTException( "Invalid image formats/types");

		applyFloat( dst, src, H, c );
	}

	void Homography::apply( const ParamSet* set, IFilterType t ) const
	{
		if( !(getIFilterType() & t) )
			throw CVTException("Invalid filter type (CPU/GPU)!");

		Image* dst;
		Image* src;
		Matrix3f H;
		Color c;
		
		src = set->arg<Image*>( 0 );
		dst = set->arg<Image*>( 1 );
		H = set->arg<Matrix3f>( 2 );
		c = set->arg<Color>( 3 );

		apply( *dst, *src, H, c );
	}

}
