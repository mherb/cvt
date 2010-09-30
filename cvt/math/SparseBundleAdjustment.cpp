#include "SparseBundleAdjustment.h"

#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/QR>
#include <Eigen/Sparse>

#include <cvt/math/Math.h>

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

		Eigen::VectorXd deltaCam = Eigen::VectorXd::Zero( sbaData.numCams() * 6 );
		Eigen::VectorXd deltaStruct = Eigen::VectorXd::Zero( sbaData.numPoints() * 3 );
		
		std::cout << "Initial costs: " << lastCosts << std::endl;
		
		this->resizeMatrices(sbaData);
		
		while ( true ) {
			newCosts = buildAndSolveSystem(sbaData, deltaCam, deltaStruct);
			
			std::cout << "Iteration " << numIterations << 
						 ", Costs = " << newCosts << 
						 ", lambda = " << lambda <<
						 ", lastCosts = " << lastCosts << std::endl;
			
			if(newCosts < lastCosts){
				lambda /= 1.5;
				lastCosts = newCosts;
			} else {
				lambda += 1e-5;
				//lambda *= 2.0;
				sbaData.revertDelta( deltaCam, deltaStruct );
				sbaData.costs() = lastCosts;
			}
			numIterations++;			
			
			if( checkTermination() )
				break;
			else {
				this->clearMatrices();
			}

		}
		
		this->cleanUp();
		
		return 0.0;		
	}
	
	double SparseBundleAdjustment::buildAndSolveSystem( SBAData & sbaData, 
													    Eigen::VectorXd & deltaCam,
													    Eigen::VectorXd & deltaStruct)
	{
		this->computeIntermediateValues( sbaData );
		
		// -> build and solve the RCS:
		/* reduced camera system */
		size_t numCams = sbaData.numCams();
		size_t dimension = numCams * 6;
		/* camera residuals */
		Eigen::VectorXd eCam( sbaData.numCams() * 6 );
		
		/*
		Eigen::SparseMatrix<double, Eigen::RowMajor> RCS( dimension, dimension );		
	
		int fillSize = nonZeroEntries * 6 * 6;

		fillSparseMatrix(measurements, 
						 cameraJacAccum, 
						 residualAccumForCam, 
						 eCam, 
						 W, 
						 Y, 
						 fillSize, 
						 RCS );
		
		// solve RCS for delta_a
		Eigen::SparseLDLT<Eigen::SparseMatrix<double, Eigen::RowMajor> > ldlt( RCS );
		deltaCam = eCam;		
		ldlt.solveInPlace<Eigen::Matrix<double, Eigen::Dynamic, 1> >( deltaCam );
		*/
				
		
		// fill and solve the reduced camera system		
		fillRCSMatrix( sbaData.measurements(), U, residualSumForCamera, eCam, W, Y, RCS);		
		RCS.ldlt().solve( eCam, &deltaCam );
		
		
		// now compute the delta for the structure by back substitution
		this->solveStructure( sbaData.measurements(), deltaCam, deltaStruct );
	
		// apply the parameters and evaluate the new costs
		return sbaData.applyDelta( deltaCam, deltaStruct );
	}
	
	void SparseBundleAdjustment::computeIntermediateValues( SBAData & sbaData )
	{		
		std::vector<Measurements> & measurements = sbaData.measurements();			
		std::vector<Measurements>::iterator features = measurements.begin();
		std::vector<Measurements>::iterator featureEnd = measurements.end();
		
		std::vector<size_t>::iterator cameraId;
		std::vector<Eigen::Vector2d>::iterator observation;
		std::vector<Eigen::Vector2d>::iterator observationEnd;
		std::vector<Eigen::Vector3d>::iterator projection;
		std::vector<Eigen::Vector2d>::iterator residual;
		std::vector<Eigen::Matrix2d>::iterator inverseCovariance;
		
		// A_ij
		std::vector<Eigen::Matrix<double, 2, 6> >::iterator jacobiansWRTCamera;
		// B_ij
		std::vector<Eigen::Matrix<double, 2, 3> >::iterator jacobiansWRTPoint;		
		
		std::vector<Eigen::Matrix<double, 3, 3> >::iterator pointAccumIterator = V.begin();		
		std::vector<Eigen::Matrix<double, 3, 3> >::iterator augmentedPointAccumIterator = V_aug.begin();
		std::vector<Eigen::Matrix<double, 3, 3> >::iterator augmentedPointAccumInverseIterator = V_aug_inv.begin();
		
		size_t pointIdx = 0;
		size_t nonZeroEntries = 0;
		
		while ( features != featureEnd ) {
			cameraId = features->viewIds.begin();
			observation = features->observations.begin();
			observationEnd = features->observations.end();			
			projection = features->screenPredictions.begin();
			residual = features->residuals.begin();
			inverseCovariance = features->inverseCovariances.begin();
			jacobiansWRTCamera = features->jacWRTCamParams.begin();
			jacobiansWRTPoint = features->jacWRTPointParams.begin();
			
			features->accumulatedResiduals.setZero();
			pointAccumIterator->setZero();
			augmentedPointAccumIterator->setZero();
			
			while ( observation != observationEnd ) {				
				// camera for this observation
				CameraModel & cam = sbaData.cameraWithId( *cameraId );
				Eigen::Matrix<double, 6, 6> & currentU = U[ *cameraId ];
				
				// jac WRT cam params
				cam.screenJacobians( features->pointParameters, *projection, *jacobiansWRTCamera );
				
				// jac WRT point params
				pointJacobians(cam, features->pointParameters, *projection, *jacobiansWRTPoint );
				
				// weighted sums of jacobians U_j and V_i:				
				currentU += ( jacobiansWRTCamera->transpose() * (*inverseCovariance) * (*jacobiansWRTCamera) );
				
				*pointAccumIterator += ( jacobiansWRTPoint->transpose() * (*inverseCovariance) * (*jacobiansWRTPoint) );
				
				// the mixed jacobian weights
				// TODO: try to avoid double computations: e.g. A_ij^T*Cov_ij^-1 
				if( W[ pointIdx ][ *cameraId ] == NULL ){
					W[ pointIdx ][ *cameraId ] = new Eigen::Matrix<double, 6, 3>;
					Y[ pointIdx ][ *cameraId ] = new Eigen::Matrix<double, 6, 3>;
					nonZeroEntries++;
				} 		
				
				*( W[ pointIdx ][ *cameraId ] ) = jacobiansWRTCamera->transpose() * *inverseCovariance * (*jacobiansWRTPoint);
				
				// e_b_i:
				features->accumulatedResiduals += jacobiansWRTPoint->transpose() * *inverseCovariance * (*residual);
				
				// e_a_j:
				residualSumForCamera[ *cameraId ] += jacobiansWRTCamera->transpose() * *inverseCovariance * (*residual);
				
				// increment iterators
				++observation;
				++cameraId;
				++projection;
				++residual;
				++inverseCovariance;
				++jacobiansWRTCamera;
				++jacobiansWRTPoint;
			}
			
			*augmentedPointAccumIterator = *pointAccumIterator;
			augmentedPointAccumIterator->diagonal() *= ( 1.0 + lambda );
			augmentedPointAccumIterator->computeInverse( &( *augmentedPointAccumInverseIterator ) );
			
			// for all cameras j, observing point i, compute Y_ij
			for( unsigned int j = 0; j < features->viewIds.size(); j++ ){
				*(Y[ pointIdx ][ features->viewIds[ j ] ]) = *(W[ pointIdx ][ features->viewIds[ j ] ]) * (*augmentedPointAccumInverseIterator);
			}
			
			++augmentedPointAccumIterator;
			++augmentedPointAccumInverseIterator;
			++pointAccumIterator;
			++features;	
			pointIdx++;
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
	
	void SparseBundleAdjustment::fillSparseMatrix(std::vector<Measurements> & measurements,											 
												  std::vector<Eigen::Matrix<double, 6, 6> > & cameraJacAccum,
												  std::vector<Eigen::Matrix<double, 6, 1> > & residualAccumForCam,
												  Eigen::VectorXd & eCam,
												  std::vector<std::vector<Eigen::Matrix<double, 6, 3>* > > & W,
												  std::vector<std::vector<Eigen::Matrix<double, 6, 3>* > > & Y,
												  int fillSize,
												  Eigen::SparseMatrix<double, Eigen::RowMajor> & RCS )
	{
		Eigen::Matrix<double, 6, 6> tmpBlock;
		bool zero;
		
		Eigen::Matrix<double, 6, 6> U_augmented;
		
		size_t numCams = cameraJacAccum.size();
		
		RCS.startFill(fillSize);
		for( unsigned int c = 0; c < numCams; c++ ){			
			U_augmented = cameraJacAccum[ c ];
			U_augmented.diagonal() *= (1.0 + lambda);
			
			eCam.block( c*6, 0, 6, 1) = residualAccumForCam[ c ];
			for( unsigned int k = 0; k < numCams; k++ ){
				tmpBlock.setZero();
				zero = true;
				
				// for each point
				for( unsigned int i = 0; i < measurements.size(); i++ ){
					// S_ck -= Y_ck * W_ck^T
					if( Y[ i ][ c ] && W[ i ][ k ] ){
						zero = false;
						tmpBlock -= ( *Y[ i ][ c ] * W[ i ][ k ]->transpose() );
					}
					
					if( k == 0 ){
						if( Y[ i ][ c ] )
							eCam.block( c*6, 0, 6, 1) -= (*Y[ i ][ c ] * measurements[ i ].accumulatedResiduals );
					}
				}
				
				if ( c == k ){
					zero = false;
					tmpBlock += U_augmented;
				}
				
				if( !zero ){
					for(unsigned int row = 0; row < 6; row++)
						for(unsigned int col = 0; col < 6; col++)
							RCS.fill( c*6+row, k*6+col ) = tmpBlock( row, col );
				}
			}			
		}
		RCS.endFill();		
	}
	
	void SparseBundleAdjustment::fillRCSMatrix(std::vector<Measurements> & measurements,											 
					   std::vector<Eigen::Matrix<double, 6, 6> > & cameraJacAccum,
					   std::vector<Eigen::Matrix<double, 6, 1> > & residualAccumForCam,
					   Eigen::VectorXd & eCam,
					   std::vector<std::vector<Eigen::Matrix<double, 6, 3>* > > & W,
					   std::vector<std::vector<Eigen::Matrix<double, 6, 3>* > > & Y,
					   Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> & RCS )
	{
		Eigen::Matrix<double, 6, 6> tmpBlock;
		bool zero;
		
		Eigen::Matrix<double, 6, 6> U_augmented;
		
		size_t numCams = cameraJacAccum.size();
		
		for( unsigned int c = 0; c < numCams; c++ ){			
			U_augmented = cameraJacAccum[ c ];
			U_augmented.diagonal() *= (1.0 + lambda);
			
			eCam.block( c*6, 0, 6, 1) = residualAccumForCam[ c ];
			for( unsigned int k = 0; k < numCams; k++ ){
				tmpBlock.setZero();
				zero = true;
				
				// for each point
				for( unsigned int i = 0; i < measurements.size(); i++ ){
					// S_ck -= Y_ck * W_ck^T
					if( Y[ i ][ c ] ){
						if( k == 0 ){
							eCam.block( c*6, 0, 6, 1) -= (*Y[ i ][ c ] * measurements[ i ].accumulatedResiduals );
						}						
						if( W[ i ][ k ] ){
							zero = false;
							tmpBlock -= ( *Y[ i ][ c ] * W[ i ][ k ]->transpose() );
						}
					}					
				}
				
				if ( c == k ){
					zero = false;
					tmpBlock += U_augmented;
				}
				
				if( !zero ){
					RCS.block( c*6, k*6, 6, 6 ) = tmpBlock;
				}
			}			
		}
	}	
	
	void SparseBundleAdjustment::pointJacobians(const CameraModel & camera, 
												const Eigen::Vector4d & pointParams,
												const Eigen::Vector3d & p3,
												Eigen::Matrix<double, 2, 3> & jacobians)
	{
		double currentDelta;
		currPointParams = pointParams;
		for( unsigned int i = 0; i < 3; i++ ){
			currentDelta = cvt::Math::max( 0.0001 * currPointParams( i ), 0.000001 );
			currPointParams( i ) += currentDelta;
			
			camera.project( currPointParams, currProjection );
			
			jacobians( 0, i ) = ( currProjection( 0 ) / currProjection( 2 ) - p3( 0 ) / p3( 2 ) ) / currentDelta;
			jacobians( 1, i ) = ( currProjection( 1 ) / currProjection( 2 ) - p3( 1 ) / p3( 2 ) ) / currentDelta;
			
			currPointParams( i ) -= currentDelta;
		}		
	}	
	
	void SparseBundleAdjustment::resizeMatrices( SBAData & sbaData )
	{
		W.resize( sbaData.numPoints() );
		Y.resize( sbaData.numPoints() );
				
		V.resize( sbaData.numPoints() );		
		V_aug.resize( sbaData.numPoints() );
		V_aug_inv.resize( sbaData.numPoints() );				
		
		/* diagonal augmented by (1 + lambda) */		
		for(unsigned int i = 0; i < V.size(); i++){
			V[ i ].setZero();
			V_aug[ i ].setZero();
			
			W[ i ].resize( sbaData.numCams(), NULL );
			Y[ i ].resize( sbaData.numCams(), NULL );
		}
		
		U.resize( sbaData.numCams() );
		residualSumForCamera.resize( sbaData.numCams() );
		for(unsigned int i = 0; i < U.size(); i++){
			U[ i ].setZero();
			residualSumForCamera[ i ].setZero();
		}
		
		size_t dimension = sbaData.numCams()*6;
		RCS = Eigen::MatrixXd::Zero( dimension, dimension );
	}
	
	void SparseBundleAdjustment::clearMatrices()
	{		
		for( unsigned int i = 0; i < U.size(); i++ ){
			U[ i ].setZero();
			residualSumForCamera[ i ].setZero();
		}
	}
	
	void SparseBundleAdjustment::solveStructure( std::vector<Measurements> & measurements, 
												 Eigen::VectorXd & deltaCam, 
												 Eigen::VectorXd & deltaStruct )
	{
		Eigen::Vector3d tmpDelta;		
		for( unsigned int i = 0; i < measurements.size(); i++ ){
			tmpDelta = measurements[ i ].accumulatedResiduals;			
			for(unsigned int j = 0; j < measurements[ i ].viewIds.size(); j++){
				tmpDelta -= W[ i ][ measurements[ i ].viewIds[ j ] ]->transpose() * deltaCam.block( j*6, 0, 6, 1 );
			}
			deltaStruct.block( i * 3, 0, 3, 1) = V_aug_inv[ i ] * tmpDelta;
		}		
	}
}
