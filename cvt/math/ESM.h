/*
 *  ESM.h
 *
 *  Created by Sebastian Klose on 10.10.10.
 *  Copyright 2010. All rights reserved.
 *
 */
#ifndef CVT_ESM_H
#define CVT_ESM_H

#include <cvt/gfx/Image.h>
#include <cvt/io/ImageIO.h>
#include <cvt/math/SL3.h>
#include <cvt/gfx/ifilter/Homography.h>
#include <cvt/math/Matrix.h>
#include <Eigen/Cholesky>

namespace cvt {
	template < typename T >
	class ESM
	{
	public:
		ESM( size_t maxIter = 20, double ssdEpsilon = 1e-8 );		
		~ESM(){}
		
		void updateTemplate( const Image & img );
		void optimize( SL3<T> & H, const Image & currI );
		
		double error() const { return ssd; }
		size_t iterations() const { return iteration; }
		
	private:	
		/* stopping citerions */
		size_t maxIter;
		T ssdEpsilon;
		
		/* the template & warped */
		Image temp;
		Image warped;
		
		/* derivatives wrt the pose parameters */
		Eigen::Matrix<T, 8, 8> jPose;
		
		/* combined jacobians: ( grad( temp ) + grad( warped ) )* jPose */
		Eigen::Matrix<T, 8, 8> jEsm;
		
		/* intensity difference multiplied by JesmT */
		Eigen::Matrix<T, 8, 1> deltaI;
		
		T ssd;
		size_t iteration;
		
		/* gradients of template & current warped */
		Image tmpDx;
		Image tmpDy;
		Image warpedDx;
		Image warpedDy;
		
		IKernel kdx, kdy;
		
		/* calc jacobians of current warped and deltaI */
		void updateData( const SL3<T> & pose );
		
		void calcTemplateGradients();
		
	};
	
	template <typename T>
	inline ESM<T>::ESM( size_t maxIter, double ssdEpsilon ):
		maxIter( maxIter ),	ssdEpsilon( ssdEpsilon ), ssd( 0.0 ), iteration( 0 ),
		kdx( IKernel::HAAR_HORIZONTAL_3 ), kdy( IKernel::HAAR_VERTICAL_3 )
	{		
	}
	
	template < typename T >
	inline void ESM<T>::updateTemplate( const Image & img )
	{
		temp.reallocate( img );
		temp = img;
		
		warped.reallocate( temp );				
		tmpDx.reallocate( temp );
		tmpDy.reallocate( temp );
		
		warpedDx.reallocate( temp );
		warpedDy.reallocate( temp );
		
		// compute gradients of the template image
		temp.convolve( tmpDx, kdx );
		temp.convolve( tmpDy, kdy );		
	}	
	
	template <typename T>
	inline void ESM<T>::optimize( SL3<T> & H, const Image & currI)
	{
		Homography hFilter;		
		cvt::Matrix3f warpMat;
		Color black( 0.0f, 0.0f, 0.0f, 1.0f );
		
		Eigen::Matrix<double, 8, 1> deltaH;					
		iteration = 0;		
		
		while ( true ) {			
			const Eigen::Matrix<T, 3, 3> & hMat = H.transformation();		
			
			warpMat[ 0 ].x = ( float )hMat( 0, 0 ); warpMat[ 0 ].y = ( float )hMat( 0, 1 ); warpMat[ 0 ].z = ( float )hMat( 0, 2 );
			warpMat[ 1 ].x = ( float )hMat( 1, 0 ); warpMat[ 1 ].y = ( float )hMat( 1, 1 ); warpMat[ 1 ].z = ( float )hMat( 1, 2 );
			warpMat[ 2 ].x = ( float )hMat( 2, 0 ); warpMat[ 2 ].y = ( float )hMat( 2, 1 ); warpMat[ 2 ].z = ( float )hMat( 2, 2 );
			
			// get the pixels using the current warp
			hFilter.apply( warped, currI, warpMat, black );			
									
			// calc combined jacobians and deltaI
			updateData( H );
			
			if( ssd < ssdEpsilon ){
				break;
			}
											
			deltaH = jEsm.inverse() * deltaI;									
			H.apply( deltaH );
			
			iteration++;
			if( iteration > maxIter ){
				break;
			}			
			
		}		
	}
	
	template <typename T>
	inline void ESM<T>::updateData( const SL3<T> & pose )
	{
		Eigen::Vector2d p( Eigen::Vector2d::Zero() );
		
		warped.convolve( warpedDx, kdx );
		warped.convolve( warpedDy, kdy );		
		
		float* timg;
		float* tptr;
		size_t tstride;
		float* tdximg;
		float* tdxptr;
		size_t tdxstride;
		float* tdyimg;
		float* tdyptr;
		size_t tdystride;
		float* wimg;
		float* wptr;
		size_t wstride;
		float* wdximg;
		float* wdxptr;
		size_t wdxstride;
		float* wdyimg;
		float* wdyptr;
		size_t wdystride;
		
		timg = temp.map<float>( &tstride );
		tptr = timg;
		
		tdximg = tmpDx.map<float>( &tdxstride );
		tdxptr = tdximg;
		
		tdyimg = tmpDy.map<float>( &tdystride );
		tdyptr = tdyimg;
		
		wimg = warped.map<float>( &wstride );
		wptr = wimg;
		
		wdximg = warpedDx.map<float>( &wdxstride );
		wdxptr = wdximg;
		
		wdyimg = warpedDy.map<float>( &wdystride );
		wdyptr = wdyimg;
		
		Eigen::Matrix<T, 1, 2> grad;
		T intensityDelta;
		Eigen::Matrix<T, 2, 8> jScreen;
		
		jEsm.setZero();
		deltaI.setZero();
		Eigen::Matrix<T, 1, 8> j;		
		
		ssd = 0.0;		
		size_t pointIdx = 0;
		for( size_t h = 0; h < temp.height(); h++ ) {
			p[ 1 ] = ( T ) h;
			
			for( size_t x = 0; x < temp.width(); x++ ){
				p[ 0 ] = ( T ) x;
				
				grad( 0, 0 ) = ( wdxptr[ x ] + tdxptr[ x ] );
				grad( 0, 1 ) = ( wdyptr[ x ] + tdyptr[ x ] );
								
				pose.screenJacobian( jScreen, p );				
				j = grad * jScreen;
				jEsm += j.transpose() * j;				
				
				// compute delta between intensities + ssd				
				intensityDelta = ( wptr[ x ] - tptr[ x ] );
				
				deltaI += intensityDelta * j.transpose();
				
				ssd += Math::sqr( intensityDelta );
				pointIdx++;
			}
			
			tptr += tstride;
			tdxptr += tdxstride;
			tdyptr += tdystride;
			wptr += wstride;
			wdxptr += wdxstride;
			wdyptr += wdystride;
		}
		
		ssd /= ( float )( temp.width() * temp.height() );
		
		temp.unmap<float>( timg );
		tmpDx.unmap<float>( tdximg );
		tmpDy.unmap<float>( tdyimg );
		warped.unmap<float>( wimg );
		warpedDx.unmap<float>( wdximg );
		warpedDy.unmap<float>( wdyimg );
	}
}


#endif

