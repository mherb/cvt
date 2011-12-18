#include "SparseBundleAdjustment.h"

#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/QR>

#include <cvt/math/Math.h>

#include <cstring>

namespace cvt {
	
	SparseBundleAdjustment::SparseBundleAdjustment()
	{		
	}
	
	SparseBundleAdjustment::~SparseBundleAdjustment()
	{		
	}


	double SparseBundleAdjustment::optimize( SlamMap & map )
	{
		_iterations = 0;
		_costs	    = 0.0;
	
		// evaluate costs, resize the vectors
		size_t numPoints = map.numFeatures();
		size_t numCams	 = map.numKeyframes();
		size_t numMeas	 = map.numMeasurements();

		_pointsJTJ.resize( numPoints );
		_camsJTJ.resize( numCams );
		_residuals.resize( numMeasurements );
		_camResiduals.resize( numCams );
		_pointResiduals.resize( numPoints );

		_camPointJTJ.resize( numCams, numPoints );

		_sparseReduced.resize( camParamDim * numCams, camParamDim * numCams );
		_reducedRHS.resize( camParamDim * numCams );

		// build the reduced system
		buildReducedCameraSystem( map );

		// prepare the sparse matrix (storage allocation) <- need jointMeasures to be filled
		prepareSparseMatrix( numCams );

		while( true ){
		
			// TODO: check if we're done
		}
	}

	void SparseBundleAdjustment::buildReducedCameraSystem( const SlamMap & map )
	{
		size_t nPts  = map.numFeatures();
		size_t nCams = map.numKeyframes();

		evaluateApproxHessians( map );
		updateInverseAugmentedPointHessians();
	}

	void SparseBundleAdjustment::evaluateApproxHessians( const SlamMap & map )
	{
		size_t nPts  = map.numFeatures();
		size_t nCams = map.numKeyframes();

		Eigen::Matrix<double, 2, 6> screenJacCam;
		Eigen::Matrix<double, 2, 3> screenJacPoint;
		Eigen::Matrix<double, 6, 2> jCamTCovInv;
		Eigen::Matrix<double, 3, 2> jPointTCovInv;
		Eigen::Matrix<double, 3, 1> point3d, pCam;
		Eigen::Matrix<double, 2, 1> reproj;
		clearJacobianSums();

		const Matrix3d & K = map.intrinsics();
		size_t currMeas = 0;

		double avgDiag = 0.0;

		// for all 3D Points
		for( size_t i = 0; i < nPts; i++ ){
			_pointsJTJ[ i ].setZero();
			_pointResiduals[ i ].setZero();

			// get the MapFeature: 
			const MapFeature & feature = map.featureForId( i );
			const Eigen::Matrix4d & ptmp = feature.estimate();
			point3d = ptmp.head<3>() / ptmp[ 3 ];


			// for all point tracks (meas. in the cams) of this point
			MapFeature::ConstPointTrackIterator camIterEnd  = feature.pointTrackEnd();
			MapFeature::ConstPointTrackIterator camIterCurr = feature.pointTrackBegin();
			
			/* update the joint meas structure: TODO -> only needed in first iteration! */
			while( camIterCurr != camIterEnd ){
				for( MapFeature::ConstPointTrackIterator camIter = ++camIterCurr;
					camIter != camIterEnd;
					++camIter ){
					_jointMeasures.addMeasurementForEntity( *camIterCurr, *camIter, i );
				}
			}

			for( MapFeature::ConstPointTrackIterator camIter = feature.pointTrackBegin();
				 camIter != camIterEnd;
				 camIter++, currMeas++; ){
				// get the keyframe:
				const Keyframe & keyframe = map.keyframeForId( *camIter );

				// screen jacobian for this 3D point in that camera
				const Eigen::Matrix4d & trans = keyframe.pose().transformation();

				pCam = trans.block<3, 3>( 0, 0 ) * point3d + trans.block<3, 1>( 0, 3 ); 

				// screen jacobian for this camera
				keyframe.pose().screenJacobian( screenJacCam, pCam );

				// point jacobian for this point in this cam:
				double invZ = 1.0 / pCam.z();
				double invZZ = 1.0 / Math::sqr( pCam.z() );
				screenJacPoint( 0, 0 ) =  K( 0, 0 ) * invZ;
				screenJacPoint( 0, 2 ) = -K( 0, 0 ) * invZZ * pCam[ 0 ];
				screenJacPoint( 1, 1 ) =  K( 1, 1 ) * invZ;
				screenJacPoint( 1, 2 ) = -K( 1, 1 ) * invZZ * pCam[ 1 ];
				screenJacPoint *= trans.block<3, 3>( 0, 0 );

				// the reprojection:
				reproj[ 0 ] = K( 0, 0 ) * pCam[ 0 ] * invZ + K( 0, 2 ); 
				reproj[ 1 ] = K( 1, 1 ) * pCam[ 1 ] * invZ + K( 1, 2 );

				const MapMeasurement & mm = keyframe.measurementForId( i );
				_residuals[ currMeas ] = mm.point - reproj;

				// J^T * Cov^-1
				jCamTCovInv = screenJacCam.transpose() * mm.information;
				jPointTCovInv = screenJacPoint.transpose() * mm.information;

				// accumulate the jacobians
				_pointsJTJ[ i ]			+= jPointTCovInv * screenJacPoint;
				_camsJTJ[ *camIter ]	+= jCamTCovInv * screenJacCam;

				// accumulate the residuals
				_pointResiduals[ i ] += ( jPointTCovInv * _residuals[ currMeas ] );
				_camResiduals[ i ]   += ( jCamTCovInv * _residuals[ currMeas ] );

				/* camPointJacobian */
				_camPointJTJ.block( *camIter, i ) = jCamTCovInv * screenJacPoint;

			}

			// the pointjac JTJ & pointResidual sums for pt i are complete now
			avgDiag += _pointsJTJ[ i ].array().diagonal().sum();

		}

		// the camJac & JTJ & pointResidual sums are complete now
		for( size_t j = 0; j < numCams; j++ ){
			avgDiag += _camsJTJ[ j ].array().diagonal().sum();
		}

		// initial lambda
		_lambda = avgDiag / ( ( numCams + numPoints ) * 1000.0 );
	}

	void SparseBundleAdjustment::clear()
	{
		for( size_t i = 0; i < _camsJTJ.size(); i++ ){
			_camsJTJ[ i ].setZero();
			_camResiduals[ i ].setZero();
		}
	}

	void SparseBundleAdjustment::prepareSparseMatrix( size_t numCams )
	{
		// according to the joint Point tracks, we can create the matrix once
		_sparseReduced.reserve( camParamDim * camParamDim * _jointMeasures.numBlocks() );

		size_t c2;
		for( size_t c = 0; c < numCams; c++ ){
			JointMeasurements::MapIteratorType iStop  = _jointMeasures.secondEntityIteratorEnd( c );
			JointMeasurements::MapIteratorType iBeg   = _jointMeasures.secondEntityIteratorBegin( c );
			for( size_t innerCol =  0; innerCol < camParamDim; innerCol++ ){
				_sparseReduced.coeffRef( c + 0, c + innerCol ) = 0;
				_sparseReduced.coeffRef( c + 1, c + innerCol ) = 0;
				_sparseReduced.coeffRef( c + 2, c + innerCol ) = 0;
				_sparseReduced.coeffRef( c + 3, c + innerCol ) = 0;
				_sparseReduced.coeffRef( c + 4, c + innerCol ) = 0;
				_sparseReduced.coeffRef( c + 5, c + innerCol ) = 0;
				JointMeasurements::MapIteratorType iStart = iBeg;
				while( iStart != iStop ){
					c2 = iStart->first;
					_sparseReduced.coeffRef( c2 + 0, c + innerCol ) = 0;
					_sparseReduced.coeffRef( c2 + 1, c + innerCol ) = 0;
					_sparseReduced.coeffRef( c2 + 2, c + innerCol ) = 0;
					_sparseReduced.coeffRef( c2 + 3, c + innerCol ) = 0;
					_sparseReduced.coeffRef( c2 + 4, c + innerCol ) = 0;
					_sparseReduced.coeffRef( c2 + 5, c + innerCol ) = 0;
					++iStart;
				}
			}

		}
	}

	void SparseBundleAdjustment::fillSparseMatrix( const SlamMap & map )
	{
		// according to the joint Point tracks, we can now fill our matrix:
		CamJTJ tmpBlock;

		double lambdaAug = 1.0 + _lambda;
		for( size_t c = 0; c < numCams; c++ ){
			// first create block for this cam:
			tmpBlock = _camsJTJ[ c ];

			// augment 
			tmpBlock.diagonal().array() *= lambdaAug;

			// go over all point measures:
			const Keyframe & k = map.keyframeForId( c );
			Keyframe::MeasurementIterator measIter = k.measurementsBegin();
			Keyframe::MeasurementIterator measEnd  = k.measurementsEnd();
			while( measIter != measEnd ){
				size_t pointId = measIter->first;
				
				const Eigen::Matrix<double, camParamDim, pointParamDim> & cp = _camPointJTJ.block( c, pointId ); 

				tmpBlock += cp * _invAugPJTJ[ pointId ] * cp.transpose();

				++measIter;
			}

			// set the block in the sparse matrix:
			_sparseReduced.block<camParamDim, camParamDim>( camParamDim * c, camParamDim * c ) = tmpBlock;
			tmpBlock.setZero();

			JointMeasurements::MapIteratorType iter   = _jointMeasures.secondEntityIteratorBegin( c );
			JointMeasurements::MapIteratorType iStop  = _jointMeasures.secondEntityIteratorEnd( c );
			while( iter != iStop ){
				size_t c2 = iter.first; // id of second cam:
				const std::set<size_t>::const_iterator pIdIter = iter.second.begin();
				const std::set<size_t>::const_iterator pEnd    = iter.second.begin();
				while( pIdIter != pEnd ){
					size_t pId = *pIdIter;
					tmpBlock += _camPointJTJ.block( c, pId ) * _invAugPJTJ[ pId ] * _camPointJTJ.block( c2, pId ).transpose();
					++pIdIter;
				}
				++iter;
				
				_sparseReduced.block<camParamDim, camParamDim>( camParamDim * c, camParamDim * c2 ) = tmpBlock;
			}
		}
	}


	void SparseBundleAdjustment::updateInverseAugmentedPointHessians()
	{
		size_t numPts = _pointsJTJ.size();
		_invAugPJTJ.resize( numPts );
		double lamdaAug = 1.0 + _lambda;
		PointJTJ inv;
		for( size_t i = 0; i < numPts; i++ ){
			inv = _pointsJTJ[ i ];
			// augment the diagonal:
			inv.diagonal().array() *= lambdaAug;
			_invAugPJTJ[ i ] = inv.inverse(); // TODO: we should exploit that its a symmetric matrix!
		}
	}
	

/*	
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
*/
}
