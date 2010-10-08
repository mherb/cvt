#include "SparseBundleAdjustment.h"

#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/QR>
#include <Eigen/Sparse>

#include <cvt/math/Math.h>

#include <cstring>

namespace cvt {
	
	SparseBundleAdjustment::SparseBundleAdjustment():
		jacDelta( 1e-6 )
	{		
	}
	
	SparseBundleAdjustment::~SparseBundleAdjustment()
	{		
	}
	
	double SparseBundleAdjustment::optimize( SBAData & sbaData )
	{
		// initialize lambda for the LM step
		lambda = 0.0001;
		numIterations = 0;
		
		lastCosts = sbaData.costs();
		double newCosts;
		costDecr = 1000.0;

		Eigen::VectorXd deltaCam = Eigen::VectorXd::Zero( sbaData.numCams() * 6 );
		Eigen::VectorXd deltaStruct = Eigen::VectorXd::Zero( sbaData.numPoints() * 3 );
		
		std::cout << "Initial costs: " << lastCosts << std::endl;
		
		this->resizeMatrices(sbaData);
		
		SparseBundleAdjustment::UpdateFunc update = &SparseBundleAdjustment::buildAndSolveSystem;
						
		while ( true ) {
			newCosts = (this->*update)(sbaData, deltaCam, deltaStruct);
			
			std::cout << "Iteration " << numIterations << 
						 ", Costs = " << newCosts << 
						 ", lambda = " << lambda <<
						 ", lastCosts = " << lastCosts << std::endl;
			
			if(newCosts < lastCosts){
				lambda *= 0.8;
				costDecr = lastCosts-newCosts;
				lastCosts = newCosts;
				//update = &SparseBundleAdjustment::buildAndSolveSystem;
			} else {
				//lambda *= 10;
				//lambda *= 2.0;
				lambda += 1e-5;
				
				sbaData.revertDelta( deltaCam, deltaStruct );
				sbaData.costs() = lastCosts;
				//update = &SparseBundleAdjustment::augmentAndSolveSystem;
			}
			numIterations++;			
			
			if( checkTermination() )
				break;
			else {
				this->clearMatrices();
			}
		}
		
		this->cleanUp();
		
		return lastCosts;		
	}
	
	double SparseBundleAdjustment::buildAndSolveSystem( SBAData & sbaData, 
													    Eigen::VectorXd & deltaCam,
													    Eigen::VectorXd & deltaStruct )
	{
		this->computeIntermediateValues( sbaData );
		return augmentAndSolveSystem( sbaData, deltaCam, deltaStruct );
	}
	
	double SparseBundleAdjustment::augmentAndSolveSystem( SBAData & sbaData, 
														  Eigen::VectorXd & deltaCam, 
														  Eigen::VectorXd & deltaStruct )
	{
		this->calcAugmentations();
		
		this->fillAndSolveMatrix(sbaData, deltaCam, deltaStruct);
		
		// apply the parameters and evaluate the new costs
		return sbaData.applyDelta( deltaCam, deltaStruct );
	}
	
	void SparseBundleAdjustment::computeIntermediateValues( SBAData & sbaData )
	{
		std::vector<KeyFrame*> & frames = sbaData.frames();
		
		/* compute J^T * Cov^-1 only once */
		Eigen::Matrix<double, 6, 2> JcamTCov;
		Eigen::Matrix<double, 3, 2> JPointTCov;
		
		// for every camera
		std::vector<SBAMeasurement>::iterator m;
		size_t pIdx;
		for( size_t c = 0; c < frames.size(); c++ ){
			// A_ij
			frames[ c ]->cameraJacobians();
			residualSumForCamera[ c ].setZero();
			U[ c ].setZero();
			
			// jacobians wrt point parameters
			for( m = frames[ c ]->measurements.begin(); m != frames[ c ]->measurements.end(); ++m ){
				CameraModel & camera = frames[ c ]->camera;
				
				/*frames[ c ]->pointJacobians( camera.K(), camera.R(), 
											 camera.t(), *(m->point3d->data), m->screenJacobiansWRTPoint );*/
				frames[ c ]->pointJacobians(*(m->point3d->data), 
											m->projection,
											m->screenJacobiansWRTPoint );
				// accumulate results
				pIdx = m->point3d->id;
				JcamTCov = m->screenJacobiansWRTCam.transpose() * m->inverseCovariance;
				JPointTCov = m->screenJacobiansWRTPoint.transpose() * m->inverseCovariance;
				
				V[ pIdx ] += ( JPointTCov * m->screenJacobiansWRTPoint );				
				U[ c ] += ( JcamTCov * m->screenJacobiansWRTCam );
				
				if( W[ pIdx ][ c ] == NULL ){
					W[ pIdx ][ c ] = new Eigen::Matrix<double, 6, 3>;
					Y[ pIdx ][ c ] = new Eigen::Matrix<double, 6, 3>;
				} 		
				
				*( W[ pIdx ][ c ] ) = JcamTCov * m->screenJacobiansWRTPoint;
				
				//std::cout << " W[" << pIdx << "][" << c << "] = \n" << *W[ pIdx ][ c ] << std::endl;
				
				// e_b_i:
				residualSumForPoint[ pIdx ] += ( JPointTCov * m->residual );
				
				// e_a_j:
				residualSumForCamera[ c ] += ( JcamTCov * m->residual );
			}
		}		
	}
	
	void SparseBundleAdjustment::fillAndSolveMatrix( SBAData & sbaData,
													 Eigen::VectorXd & deltaCam,
													 Eigen::VectorXd & deltaStruct )
	{	
		this->fillSparseMatrix( sbaData );
		
		ldlt.compute( SRCS );
		deltaCam = eCam;
		
		ldlt.solveInPlace<Eigen::Matrix<double, Eigen::Dynamic, 1> >( deltaCam );
		
		// now compute the delta for the structure by back substitution
		this->solveStructure( deltaCam, deltaStruct );
	}
	
	void SparseBundleAdjustment::calcAugmentations()
	{
		// calc the Y_ij
		for( size_t i = 0; i < W.size(); i++ ){
			// calc augmented V_i and inverse
			V_aug[ i ] = V[ i ]; 
			V_aug[ i ].diagonal() *= ( 1.0 + lambda );
			V_aug[ i ].computeInverse( &V_aug_inv[ i ] );
			
			for( size_t j = 0; j < W[ i ].size(); j++ ){
				if( W[ i ][ j ] ){
					*(Y[ i ][ j ]) = *(W[ i ][ j ]) * V_aug_inv[ i ];
				}
			}
		}
	}
	
	void SparseBundleAdjustment::cleanUp()
	{
		for(unsigned int i = 0; i < W.size(); i++){
			for (unsigned int j = 0; j < W[ i ].size(); j++) {
				if( W[ i ][ j ] ){				
					delete W[ i ][ j ];
					delete Y[ i ][ j ];
					W[ i ][ j ] = NULL;
					Y[ i ][ j ] = NULL;
				}					
			}
		}
	}	
	
	void SparseBundleAdjustment::fillSparseMatrix( SBAData & sbaData )
	{			
		Eigen::Matrix<double, 6, 6> U_augmented;
		
		size_t numCams = U.size();
		size_t numPoints = V.size();
		
		size_t fillSize = sbaData.numNonZero() * 6 * 6;	


		size_t eRow = 0;
		
		SRCS.startFill( fillSize );		
		for( unsigned int c = 0; c < numCams; c++ ){			
			// get the jacobian accumulations for this camera
			U_augmented = U[ c ];
			U_augmented.diagonal() *= (1.0 + lambda);
			
			eRow = c*6;
			eCam.block( eRow, 0, 6, 1) = residualSumForCamera[ c ];
			
			
			std::set<size_t> const & corresp = sbaData.correspondenceSet( c );
			std::set<size_t>::const_iterator otherCam = corresp.begin();
			std::set<size_t>::const_iterator otherCamEnd = corresp.end();
						
			// values for the c-th column of the S matrix
			Eigen::MatrixXd blockCol = Eigen::MatrixXd::Zero( 6 * corresp.size(), 6 );
			
			size_t row = 0;
		
			bool sameCam;
			while( otherCam != otherCamEnd )
			{
				sameCam = ( *otherCam == c );
				
				// for each point visible in camera c
				std::vector<SBAMeasurement>::const_iterator point = sbaData.frames()[ c ]->measurements.begin();
				
				while ( point != sbaData.frames()[ c ]->measurements.end() ) {
					// S_ck -= Y_ck * W_ck^T
					size_t i = point->point3d->id;
					if( Y[ i ][ *otherCam ] ){
						// eCam_c = e_ac - sum( Y_ic*e_bi ) <- contribution only once! 
						if( sameCam ){
							eCam.block( eRow, 0, 6, 1) -= (*Y[ i ][ *otherCam ] * residualSumForPoint[ i ] );
						}
						
						if( W[ i ][ c ] ){
							blockCol.block( row, 0, 6, 6 ) -= ( *Y[ i ][ *otherCam ] * W[ i ][ c ]->transpose() );
						}
					}
					++point;
				}
				
				// the diagonal need the augmented U added
				if ( sameCam ){
					blockCol.block( row, 0, 6, 6 ) += U_augmented;
				}			
				
				++otherCam;
				row+=6;
			}
						
			size_t c2;
			
			for( size_t bCol = 0; bCol < 6; bCol++ ){
				otherCam = corresp.begin();
				row = 0;

				while ( otherCam != otherCamEnd ) {
					c2 = *otherCam * 6;
				
					SRCS.fill( c2, eRow + bCol ) = blockCol( row, bCol );
					SRCS.fill( c2+1, eRow + bCol ) = blockCol( row+1, bCol );
					SRCS.fill( c2+2, eRow + bCol ) = blockCol( row+2, bCol );
					SRCS.fill( c2+3, eRow + bCol ) = blockCol( row+3, bCol );
					SRCS.fill( c2+4, eRow + bCol ) = blockCol( row+4, bCol );
					SRCS.fill( c2+5, eRow + bCol ) = blockCol( row+5, bCol );
					
					otherCam++;
					row+=6;
				}
			}
		}	
		SRCS.endFill();
	}
	
	void SparseBundleAdjustment::resizeMatrices( SBAData & sbaData )
	{
		size_t numCams = sbaData.numCams();
		size_t numPoints = sbaData.numPoints();
		
		W.resize( numPoints );
		Y.resize( numPoints );
				
		V.resize( numPoints );		
		V_aug.resize( numPoints );
		V_aug_inv.resize( numPoints );
		residualSumForPoint.resize( numPoints );		
		
		/* diagonal augmented by (1 + lambda) */
		for(unsigned int i = 0; i < V.size(); i++){
			V[ i ].setZero();
			V_aug[ i ].setZero();
			residualSumForPoint[ i ].setZero();
			
			W[ i ].resize( numCams, NULL );
			Y[ i ].resize( numCams, NULL );
		}
		
		U.resize( numCams );
		residualSumForCamera.resize( numCams );				
		
		size_t dimension = numCams*6;
		
		//RCS = Eigen::MatrixXd::Zero( dimension, dimension );	

		eCam.resize( dimension );
		
		SRCS.resize( dimension, dimension );
		
		SRCS.setZero();
	}
	
	void SparseBundleAdjustment::clearMatrices()
	{		
		// cameras matrices are clear in the loop
		std::vector<Eigen::Matrix<double, 3, 3> >::iterator v = V.begin();
		std::vector<Eigen::Matrix<double, 3, 3> >::const_iterator vEnd = V.end();
		std::vector<Eigen::Vector3d>::iterator r = residualSumForPoint.begin();
		
		while ( v != vEnd ) {
			v->setZero();
			r->setZero();
			
			++v;
			++r;
		}
		
		/*for( unsigned int i = 0; i < V.size(); i++ ){
			V[ i ].setZero();
			residualSumForPoint[ i ].setZero();
		}*/
		
		SRCS.setZero();
	}
	
	void SparseBundleAdjustment::solveStructure( Eigen::VectorXd & deltaCam, 
												 Eigen::VectorXd & deltaStruct )
	{
		Eigen::Vector3d tmpDelta;		
		for( unsigned int i = 0; i < V.size(); i++ ){
			tmpDelta = residualSumForPoint[ i ];
			
			for(unsigned int j = 0; j < U.size(); j++){
				if( W[ i ][ j ] )
					tmpDelta -= W[ i ][ j ]->transpose() * deltaCam.block( j*6, 0, 6, 1 );
			}
			deltaStruct.block( i * 3, 0, 3, 1) = V_aug_inv[ i ] * tmpDelta;
		}		
	}	
}
