/*
 *  ESM.cpp
 *
 *  Created by Sebastian Klose on 10.10.10.
 *  Copyright 2010 sebik.org. All rights reserved.
 *
 */

#include "ESM.h"

#include <cvt/gfx/ifilter/Homography.h>

#include <highgui.h>
#include <stdio.h>

namespace cvt {
	
	ESM::ESM( const Image & img, size_t maxIter, double ssdEpsilon ):
		maxIter( maxIter ),
		ssdEpsilon( ssdEpsilon ),
		dx( 3, 1, IOrder::GRAY, IType::FLOAT ),
		dy( 1, 3, IOrder::GRAY, IType::FLOAT ),
		temp( img ),
		ssd( 0.0 ),
		iteration( 0 )
	{
		size_t stride;
		float * p = ( float* )dx.map( &stride );
		p[ 0 ] =  0.0f; 
		p[ 1 ] =  1.0f; 
		p[ 2 ] =  -1.0f;
		dx.unmap( ( uint8_t* )p );
		
		p = ( float* )dy.map( &stride );
		stride /= sizeof( float );
		
		p[ 0 ] =  0.0f; 
		p[ stride ] = 1.0f; 
		p[ 2*stride ] = -1.0f;		
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
		
		cvShowImage( "dx", tmpDx.iplimage() );
		cvShowImage( "dy", tmpDy.iplimage() );
		cvWaitKey( 0 );
	}
	
	ESM::~ESM()
	{		
	}
	
	void ESM::optimize( SL3Transform & H, const Image & currI)
	{
		Homography hFilter;
		IFilterVector8 hVec;
		Eigen::Matrix3d hInverse;
		
		Color black( 0.0f, 0.0f, 0.0f, 1.0f );
		
		Eigen::Matrix<double, 8, 1> deltaH;
		Eigen::MatrixXd jT( jCombined.cols(), jCombined.rows() );
		
		iteration = 0;
				
		while ( true ) {			
			const Eigen::Matrix3d & hMat = H.matrix();			

			// get the matrix into vector form for the filter
			hVec[ 0 ] = hMat( 0, 0 );
			hVec[ 1 ] = hMat( 0, 1 );
			hVec[ 2 ] = hMat( 0, 2 );
			hVec[ 3 ] = hMat( 1, 0 );
			hVec[ 4 ] = hMat( 1, 1 );
			hVec[ 5 ] = hMat( 1, 2 );
			hVec[ 6 ] = hMat( 2, 0 );
			hVec[ 7 ] = hMat( 2, 1 );
//			hVec[ 8 ] = hMat( 2, 2 );
			
			// get the pixels using the current warp
			hFilter.apply( warped, currI, hVec, black );
			
			cvShowImage("Warped", warped.iplimage());
			//cvWaitKey( 0 );
			
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
//			std::cout << " ssd = " << ssd << "\n";
			//std::cout << H.matrix() << std::endl;  
			
			iteration++;
			
			if( iteration > maxIter ){
//				std::cout << "Maxiter threshold reached -> done" << std::endl;
				break;
			}
			{
//				cvWaitKey( 0 );
			}
				
		}		
	}
	
	void ESM::updateData( SL3Transform & H )
	{
		Eigen::Vector2d p( Eigen::Vector2d::Zero() );
		
		size_t strideT;
		uint8_t * t = temp.map( &strideT );
		strideT /= sizeof( float );
		
		size_t strideW;
		uint8_t * w = warped.map( &strideW );
		strideW /= sizeof( float );
		
		
		size_t strideTDX;
		uint8_t * tDx = tmpDx.map( &strideTDX );
		strideTDX /= sizeof( float );
		
		size_t strideTDY;
		uint8_t * tDy = tmpDy.map( &strideTDY );
		strideTDY /= sizeof( float );		
		
		float * tempPtr = ( float* )t;
		float * warpedPtr = ( float* )w;
		float * wLast = ( float* )w;
		float * wNext = ( float* )w + strideW;
		
		float * tempDxPtr = ( float* )tDx; 
		float * tempDyPtr = ( float* )tDy;
		
		float * xLast;
		float * xCurrent;
		float * xNext;		

		Eigen::Matrix<double, 1, 2> grad;

		ssd = 0.0;
		
		size_t pointIdx = 0;
		size_t h = 0;
		while ( h < temp.height() ) {			
			p[ 1 ] = ( double ) h;		
			
			xCurrent = warpedPtr;
			xLast = xCurrent;
			xNext = xCurrent + 1;
			for( size_t x = 0; x < temp.width(); x++ ){
				p[ 0 ] = ( double ) x;
				
				grad( 0, 0 ) = *xNext - *xLast;				
				grad( 0, 1 ) = wNext[ x ] - wLast[ x ];
				
				// add the template gradients
				grad( 0, 0 ) += tempDxPtr[ x ];
				grad( 0, 1 ) += tempDyPtr[ x ];

//				std::cout << "GRAD: " << std::endl << grad( 0, 0 ) << std::endl;
//				std::cout << grad( 0, 1 ) << std::endl;
				
				// calc jacobians at this point
				H.jacobiansAtPoint( p, jPose, ( pointIdx << 1 ) );
				
				// multiply by the gradients
				jCombined.block( pointIdx, 0, 1, 8 ) = grad * jPose.block( pointIdx << 1, 0, 2, 8 );
				
//				std::cout << jCombined.block( pointIdx, 0, 1, 8 ) << " -- " << grad << std::endl;
				// compute delta between intensities + ssd
				deltaI[ pointIdx ] = *xCurrent - tempPtr[ x ];
				
				ssd += Math::sqr( deltaI[ pointIdx ] );
				
				pointIdx++;
				
				if( x != 0 )
					xLast++;
				if( x < temp.width() - 1 )
					xNext++;
				xCurrent++;
			}		
			
			tempPtr += strideT;
			warpedPtr += strideW;
			
			tempDxPtr += strideTDX;
			tempDyPtr += strideTDY;
			
			if( h != 0 )
				wLast += strideW;
			
			if( h < temp.height() - 1 )
				wNext += strideW;
			
			h++;
		}
		ssd /= deltaI.rows();
		
		temp.unmap( t );
		warped.unmap( w );	
		tmpDx.unmap( tDx );
		tmpDy.unmap( tDy );
	}	
}
