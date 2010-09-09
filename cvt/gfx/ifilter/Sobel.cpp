/*
 *  Sobel.cpp
 *  CVTools
 *
 *  Created by Sebastian Klose on 08.09.10.
 *  Copyright 2010 sebik.org. All rights reserved.
 *
 */

#include "Sobel.h"

#include "gfx/IFilterScalar.h"
#include "gfx/IFilterParameterInfo.h"
#include "gfx/IFilterParameterSet.h"
#include "math/Math.h"
#include "util/Exception.h"

namespace cvt {
	
	static const IFilterParameterInfo _params[ 4 ] = {
		IFilterParameterInfo( "Input", IFILTERPARAMETER_IMAGE ),
		IFilterParameterInfo( "SobelX", IFILTERPARAMETER_IMAGE, IFILTERPARAMETER_OUT ),
		IFilterParameterInfo( "SobelY", IFILTERPARAMETER_IMAGE, IFILTERPARAMETER_OUT ),
		IFilterParameterInfo( "Normalize", IFILTERPARAMETER_SCALAR )
	};
	
	void Sobel::nonMaximalSuppression(Image & nonMaxSuppressed, Image const& dx, 
									  Image const& dy, Image const& magnitude )
	{
		const float * magData;
		const float * mag0;
		const float * mag1;
		const float * dxData;
		const float * dyData;
		float * suppr;
		
		float angle;
		for(size_t row = 1; row < magnitude.height(); row++){
			magData = (const float *) magnitude.scanline( row );
			dxData = (const float *) dx.scanline( row );
			dyData = (const float *) dy.scanline( row );
			suppr = (float *) nonMaxSuppressed.scanline( row );
			
			for(size_t w = 1; w < dx.width(); w++){
				angle = atan2(*dyData++, *dxData++);
				
				if( ( angle < 0.125f * M_PI && angle >= 0.0f ) ||
				    ( angle < -0.125f * M_PI && angle <= 0.0f * M_PI ) ||
				    ( angle > -0.875f * M_PI && angle > -M_PI ) ||
				    ( angle > 0.875f * M_PI && angle < M_PI ) ){
					// angle 0 or PI
					// need row before and after:
					mag0 = (const float *) magnitude.scanline( row - 1 );
					mag1 = (const float *) magnitude.scanline( row + 1 );
					
					if( magData[ w ] < mag0[ w ] ||
					    magData[ w ] < mag1[ w ] ) {
						suppr[ w ] = 0.0f;
					} else {
						suppr[ w ] = magData[ w ];
					}
					
					continue;
					
				}
				
				if( ( ( angle > 0.125f * M_PI ) && ( angle < 0.375f * M_PI ) ) ||
				    ( ( angle < -0.625f * M_PI ) && ( angle > -0.875f * M_PI ) ) ){
					// angle PI/4 or -3*PI/4					
					mag0 = (const float *) magnitude.scanline( row - 1 );
					mag1 = (const float *) magnitude.scanline( row + 1 );
					
					if( magData[ w ] < mag0[ w-1 ] ||
					    magData[ w ] < mag1[ w+1 ] ) {
						suppr[ w ] = 0.0f;
					} else {
						suppr[ w ] = magData[ w ];
					}
					continue;
				}
				
				if( ( ( angle > 0.625f * M_PI ) && ( angle > 0.375f * M_PI ) ) ||
				   ( ( angle > -0.625f * M_PI ) && ( angle < -0.375f * M_PI ) ) ){
					// angle PI/4 or -3*PI/4	
					
					if( magData[ w ] < magData[ w-1 ] ||
					   magData[ w ] < magData[ w+1 ] ) {
						suppr[ w ] = 0.0f;
					} else {
						suppr[ w ] = magData[ w ];
					}
					continue;
				}
				
				// if we reach here -> angle 3/4PI or -PI/4
				mag0 = (const float *) magnitude.scanline( row - 1 );
				mag1 = (const float *) magnitude.scanline( row + 1 );
				
				if( magData[ w ] < mag0[ w+1 ] ||
				   magData[ w ] < mag1[ w-1 ] ) {
					suppr[ w ] = 0.0f;
				} else {
					suppr[ w ] = magData[ w ];
				}				
			}
		}
	}
	
	void Sobel::magnitude( Image & magnitude, Image const& dx, Image const & dy )
	{
		magnitude.reallocate( dx );
		
		float const * x; 
		float const * y;
		float * mag;
		
		size_t i = magnitude.height();
		while ( i-- ) {
			x = (float const *) dx.scanline( i );
			y = (float const *) dy.scanline( i );
			mag = (float *) magnitude.scanline( i );
			
			for(size_t w = 0; w < magnitude.width(); w++)
				*mag++ = Math::sqrt( Math::sqr( *x++ ) + Math::sqr( *y++ ) );
		}
	}
	
	Sobel::Sobel():
		IFilter("Sobel", _params, 4, IFILTER_CPU),
		kernelDx( 3, 3, CVT_GRAY, CVT_FLOAT ),
		kernelDy( 3, 3, CVT_GRAY, CVT_FLOAT )
	{
		float * kernData;
		
		kernData = ( float* ) kernelDx.scanline(0);
		*kernData++ = 1.0f;
		*kernData++ = 0.0f;
		*kernData = -1.0f;
		
		kernData = ( float* ) kernelDx.scanline(1);
		*kernData++ = 2.0f;
		*kernData++ = 0.0f;
		*kernData = -2.0f;
		
		kernData = ( float* ) kernelDx.scanline(2);
		*kernData++ = 1.0f;
		*kernData++ = 0.0f;
		*kernData = -1.0f;
		
		kernData = ( float* ) kernelDy.scanline(0);
		*kernData++ = 1.0f;
		*kernData++ = 2.0f;
		*kernData = 1.0f;
		
		kernData = ( float* ) kernelDy.scanline(1);
		*kernData++ = 0.0f;
		*kernData++ = 0.0f;
		*kernData = 0.0f;
		
		kernData = ( float* ) kernelDy.scanline(2);
		*kernData++ = -1.0f;
		*kernData++ = -2.0f;
		*kernData = -1.0f;
	}
	
	void Sobel::apply( Image & dx, Image & dy, const Image & srcGray, bool normalize ) const
	{	
		srcGray.convolve( dx, kernelDx, normalize );
		srcGray.convolve( dy, kernelDy, normalize );
	}
	
	void Sobel::apply( const IFilterParameterSet* set, IFilterType t ) const
	{	
		if( !(getIFilterType() & t) )
			throw CVTException("Invalid filter type (CPU/GPU)!");

		Image* dstDx;
		Image* dstDy;
		Image* src;
		IFilterScalar* normalize;
		
		if( !set->isValid())
			throw CVTException("Invalid argument(s)");
		
		if( !( src = dynamic_cast<Image*>( set->getParameter( 0 ) ) ) )
			throw CVTException("Invalid argument");
		
		if( !( dstDx = dynamic_cast<Image*>( set->getParameter( 1 ) ) ) )
			throw CVTException("Invalid argument");
		
		if( !( dstDy = dynamic_cast<Image*>( set->getParameter( 2 ) ) ) )
			throw CVTException("Invalid argument");

		if( !( normalize = dynamic_cast<IFilterScalar*>( set->getParameter( 3 ) ) ) )
			throw CVTException("Invalid argument");
	
		apply( *dstDx, *dstDy, *src, (bool)normalize->get() );
	}
	
}