#include "gfx/ifilter/Homography.h"
#include "gfx/IFilterParameterInfo.h"
#include "gfx/IFilterParameterSet.h"
#include "util/Exception.h"
#include "math/Math.h"

#include <iostream>

namespace cvt {

	static const IFilterParameterInfo _params[ 4 ] = {
		IFilterParameterInfo( "Input", IFILTERPARAMETER_IMAGE ),
		IFilterParameterInfo( "Output", IFILTERPARAMETER_IMAGE, IFILTERPARAMETER_OUT ),
		IFilterParameterInfo( "H", IFILTERPARAMETER_VECTOR8 ),
		IFilterParameterInfo( "Color", IFILTERPARAMETER_COLOR )
	};

	Homography::Homography() : IFilter( "Homography", _params, 4, IFILTER_CPU )
	{
	}

	void Homography::applyFloat( Image& idst, const Image& isrc, const IFilterVector8& H, const Color& color ) const
	{
		float h33;
		const uint8_t* src;
		uint8_t* dst;
		uint8_t* dst2;
		size_t sstride;
		size_t dstride;
		float* pdst;
		size_t w, h, i;
		ssize_t sw, sh;
		float x, y;
		float hx, hy, hz;
		float data[ 4 ];
		float col[ 4 ] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float alpha, beta;
		size_t C;

		h33 = ( ( H[ 6 ] * ( H[ 1 ] * H[ 5 ] - H[ 4 ] * H[ 2 ] ) +
			   H[ 7 ] * ( H[ 3 ] * H[ 2 ] - H[ 0 ] * H[ 5 ] ) ) - 1.0f ) / ( H[ 3 ] * H[ 1 ] - H[ 0 ] * H[ 4 ] );

		if( h33 == 0.0f || std::isinf( h33 ) || std::isnan( h33 ) )
			return;

		dst = idst.map( &dstride );
		src = isrc.map( &sstride );
		w = idst.width();
		h = idst.height();
		sw = ( ssize_t ) isrc.width();
		sh = ( ssize_t ) isrc.height();
		C = idst.channels();
		dst2 = dst;
		y = 0.0f;

		while( h-- ) {
			pdst = ( float * ) dst2;
			i = w;
			x = 0.0f;
			while( i-- ) {
				float alpha, beta;
				ssize_t ix[ 2 ], iy[ 2 ];

				// Evaluate homography
				hx = x * H[ 0 ] + y * H[ 1 ] + H[ 2 ];
				hy = x * H[ 3 ] + y * H[ 4 ] + H[ 5 ];
				hz = x * H[ 6 ] + y * H[ 7 ] + h33;
				hx /= hz;
				hy /= hz;

				if( Math::abs( hz ) > Math::EPSILONF ) {
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
				x += 1.0f;
			}
			y += 1.0f;
			dst2 += dstride;
		}
		idst.unmap( dst );
		isrc.unmap( src );
	}

	void Homography::apply( Image& dst, const Image& src, const IFilterVector8& h, const Color& c ) const
	{

		if( dst.type() != src.type() ||
		    dst.order() != src.order() ||
			dst.type() != IType::FLOAT )
			throw CVTException( "Invalid image formats/types");

		applyFloat( dst, src, h, c );
	}

	void Homography::apply( const IFilterParameterSet* set, IFilterType t ) const
	{
		if( !(getIFilterType() & t) )
			throw CVTException("Invalid filter type (CPU/GPU)!");

		Image* dst;
		Image* src;
		IFilterVector8* h;
		Color* c;

		if( !set->isValid())
			throw CVTException("Invalid argument(s)");

		if( !( dst = dynamic_cast<Image*>( set->getParameter( 1 ) ) ) )
			throw CVTException("Invalid argument");
		if( !( src = dynamic_cast<Image*>( set->getParameter( 0 ) ) ) )
			throw CVTException("Invalid argument");
		if( !( h = dynamic_cast<IFilterVector8*>( set->getParameter( 2 ) ) ) )
			throw CVTException("Invalid argument");
		if( !( c = dynamic_cast<Color*>( set->getParameter( 3 ) ) ) )
			throw CVTException("Invalid argument");
		apply( *dst, *src, *h, *c );
	}

}
