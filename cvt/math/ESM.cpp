/*
 *  ESM.cpp
 *
 *  Created by Sebastian Klose on 10.10.10.
 *  Copyright 2010 sebik.org. All rights reserved.
 *
 */

#include "ESM.h"

#include <cvt/gfx/ifilter/Homography.h>

#include <stdio.h>
#include <cvt/math/Matrix.h>

namespace cvt {
	
	ESM::ESM( const Image & img, size_t maxIter, double ssdEpsilon ):
		maxIter( maxIter ),
		ssdEpsilon( ssdEpsilon ),
		dx( 5, 1, IFormat::GRAY_FLOAT ),
		dy( 1, 5, IFormat::GRAY_FLOAT ),
		temp( img ),
		ssd( 0.0 ),
		iteration( 0 )
	{
		size_t stride;
		float * p = ( float* )dx.map( &stride );
		p[ 0 ] =  -0.1f;
		p[ 1 ] =   0.8f;
		p[ 2 ] =   0.0f;
		p[ 3 ] =  -0.8f;
		p[ 4 ] =   0.1f;
		dx.unmap( ( uint8_t* )p );
		
		p = ( float* )dy.map( &stride );
		stride /= sizeof( float );
		p[ 0 * stride ] =  -0.1f; 
		p[ 1 * stride ] =   0.8f; 
		p[ 2 * stride ] =   0.0f;
		p[ 3 * stride ] =  -0.8f;
		p[ 4 * stride ] =   0.1f;
		dy.unmap( ( uint8_t* ) p );
		
		warped.reallocate( temp );
		
		jPose.resize( 2 * temp.width() * temp.height(), 8 );
		jCombined.resize( temp.width() * temp.height(), 8 );
		deltaI.resize( temp.width() * temp.height() );
		
		tmpDx.reallocate( temp );
		tmpDy.reallocate( temp );
		
		warpedDx.reallocate( temp );
		warpedDy.reallocate( temp );

		// compute gradients of the template image
		temp.convolve( tmpDx, dx );
		temp.convolve( tmpDy, dy );
		
		Eigen::Vector2d P;
		size_t pointIdx = 0;
		for( size_t y = 0; y < temp.height(); y++ ){
			P[ 1 ] = y;
			for( size_t x = 0; x < temp.width(); x++ ){
				P[ 0 ] = x;
				SL3Transform::jacobiansAtIdentity( P, jPose, pointIdx );
				pointIdx+=2;
			}
		}
	}
	
	ESM::~ESM()
	{		
	}
	
	void ESM::optimize( SL3Transform & H, const Image & currI)
	{
		Homography hFilter;

		Eigen::Matrix3d hInverse;
		
		Color black( 0.0f, 0.0f, 0.0f, 1.0f );
		
		Eigen::Matrix<double, 8, 1> deltaH;
		Eigen::MatrixXd jT( jCombined.cols(), jCombined.rows() );
		
		iteration = 0;
		
		cvt::Matrix3f warpMat;
				
		while ( true ) {			
			const Eigen::Matrix3d & hMat = H.matrix();			
			warpMat[ 0 ].x = ( float )hMat( 0, 0 ); warpMat[ 0 ].y = ( float )hMat( 0, 1 ); warpMat[ 0 ].z = ( float )hMat( 0, 2 );
			warpMat[ 1 ].x = ( float )hMat( 0, 0 ); warpMat[ 1 ].y = ( float )hMat( 1, 1 ); warpMat[ 1 ].z = ( float )hMat( 1, 2 );
			warpMat[ 2 ].x = ( float )hMat( 0, 0 ); warpMat[ 2 ].y = ( float )hMat( 2, 1 ); warpMat[ 2 ].z = ( float )hMat( 2, 2 );
			
			// get the pixels using the current warp
			hFilter.apply( warped, currI, warpMat, black );
			
			// calc combined jacobians and deltaI
			updateData( H );
			
			if( ssd < ssdEpsilon ){
//				std::cout << "Epsilon threshold reached -> done" << std::endl;
				break;
			}
			
			// calc pseudo inverse: (J^T*J)^-1 * J^T
			jT = jCombined.transpose();			
			deltaH = -2.0 * ((jT * jCombined).inverse() * jT * deltaI);
					
//			std::cout << "Delta H: " << deltaH << std::endl;
			H.update( deltaH );
			
//			std::cout << "ESM: iter = " << iteration << ", " ;
			std::cout << " ssd = " << ssd << std::endl;
			//std::cout << H.matrix() << std::endl;  
			
			iteration++;
			
			if( iteration > maxIter ){
//				std::cout << "Maxiter threshold reached -> done" << std::endl;
				break;
			}			
				
		}		
	}
	
	void ESM::updateData( SL3Transform & H )
	{
		Eigen::Vector2d p( Eigen::Vector2d::Zero() );
		
		warped.convolve( warpedDx, dx );
		warped.convolve( warpedDy, dy );

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

		timg = temp.map<float>( & tstride );
		tptr = timg;
		tdximg = tmpDx.map<float>( & tdxstride );
		tdxptr = tdximg;
		tdyimg = tmpDy.map<float>( & tdystride );
		tdyptr = tdyimg;
		wimg = warped.map<float>( & wstride );
		wptr = wimg;
		wdximg = warpedDx.map<float>( & wdxstride );
		wdxptr = wdximg;
		wdyimg = warpedDy.map<float>( & wdystride );
		wdyptr = wdyimg;

		Eigen::Matrix<double, 1, 2> grad;

		ssd = 0.0;

		size_t pointIdx = 0;
		for( size_t h = 0; h < temp.height(); h++ ) {
			p[ 1 ] = ( double ) h;

			for( size_t x = 0; x < temp.width(); x++ ){
				p[ 0 ] = ( double ) x;
				
				grad( 0, 0 ) = wdxptr[ x ];
				grad( 0, 1 ) = wdyptr[ x ];
				
				grad( 0, 0 ) += tdxptr[ x ];
				grad( 0, 1 ) += tdyptr[ x ];

//				std::cout << "GRAD: " << std::endl << grad( 0, 0 ) << std::endl;
//				std::cout << grad( 0, 1 ) << std::endl;
				
				// multiply by the gradients
				jCombined.block( pointIdx, 0, 1, 8 ) = grad * jPose.block( pointIdx << 1, 0, 2, 8 );
			
/*				if( jCombined.block( pointIdx, 0, 1, 8 ).maxCoeff() >= 1e5 )
					std::cout << "IDX " << pointIdx << "( " << temp.width() << " , " << temp.height() << " ) -- " << jCombined.block( pointIdx, 0, 1, 8 ) << " -- " << grad << " " << tempDyPtr[ x ] << tempDyPtr[ x ] << std::endl;*/
				// compute delta between intensities + ssd
				deltaI[ pointIdx ] = wptr[ x ] - tptr[ x ];

				ssd += Math::sqr( deltaI[ pointIdx ] );
				pointIdx++;
			}

			tptr += tstride;
			tdxptr += tdxstride;
			tdyptr += tdystride;
			wptr += wstride;
			wdxptr += wdxstride;
			wdyptr += wdystride;
		}
		ssd /= deltaI.rows();
		temp.unmap<float>( timg );
		tmpDx.unmap<float>( tdximg );
		tmpDy.unmap<float>( tdyimg );
		warped.unmap<float>( wimg );
		warpedDx.unmap<float>( wdximg );
		warpedDy.unmap<float>( wdyimg );
	}
}
