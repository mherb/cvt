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
		p[ 0 ] =  1.0f; 
		p[ 1 ] =  0.0f; 
		p[ 2 ] =  -1.0f;
		dx.unmap( ( uint8_t* )p );
		
		p = ( float* )dy.map( &stride );
		stride /= sizeof( float );
		
		p[ 0 ] =  1.0f; 
		p[ stride ] = 0.0f; 
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
				
				// calc jacobians at this point
				H.jacobiansAtPoint( p, jPose, ( pointIdx << 1 ) );
				
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
		temp.unmap( ( uint8_t* ) timg );
		tmpDx.unmap( ( uint8_t* ) tdximg );
		tmpDy.unmap( ( uint8_t* ) tdyimg );
		warped.unmap( ( uint8_t* ) wimg );
		warpedDx.unmap( ( uint8_t* ) wdximg );
		warpedDy.unmap( ( uint8_t* ) wdyimg );
	}
}
