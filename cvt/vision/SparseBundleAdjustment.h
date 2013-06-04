/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_SPARSE_BUNDLE_ADJUSTMENT
#define CVT_SPARSE_BUNDLE_ADJUSTMENT

//#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET
#include <Eigen/StdVector>
#include <set>

#include <Eigen/Core>
#include <Eigen/Sparse>

#include <cvt/vision/slam/SlamMap.h> 
#include <cvt/math/SparseBlockMatrix.h>
#include <cvt/math/JointMeasurements.h>
#include <cvt/math/TerminationCriteria.h>

namespace cvt {
	class SparseBundleAdjustment
	{
		public:
			SparseBundleAdjustment();
			~SparseBundleAdjustment();

			void optimize( SlamMap & data, const TerminationCriteria<double> & criteria );

			size_t iterations() const { return _iterations; }
			double costs()	const { return _costs; }

		private:
			/* jacobians for each point */		
			static const size_t pointParamDim = 3;
			static const size_t camParamDim   = 6;
			typedef Eigen::Matrix<double, 2, pointParamDim>				PointScreenJacType;
			typedef Eigen::Matrix<double, 2, camParamDim>				CamScreenJacType;
			typedef Eigen::Matrix<double, camParamDim, camParamDim>		CamJTJ;
			typedef Eigen::Matrix<double, pointParamDim, pointParamDim>	PointJTJ;
			typedef Eigen::Matrix<double, camParamDim, 1>				CamResidualType;
			typedef Eigen::Matrix<double, pointParamDim, 1>				PointResidualType;

			
			size_t _nPts;
			size_t _nCams;
			size_t _nMeas;

			/* approx. Point Hessians */
			PointJTJ*			_pointsJTJ;

			/* inverse of the augmented approx Point Hessians */
			PointJTJ*			_invAugPJTJ;

			CamJTJ*				_camsJTJ;
			CamResidualType*	_camResiduals;
			PointResidualType*	_pointResiduals;
			
			/* Sparse Upper Left of the approx. Hessian */
			SparseBlockMatrix<camParamDim, pointParamDim>			_camPointJTJ;
			JointMeasurements										_jointMeasures;
			
			Eigen::SparseMatrix<double, Eigen::ColMajor>			_sparseReduced;
			Eigen::VectorXd											_reducedRHS;

			// levenberg marquard damping
			double _lambda;
			size_t _iterations;
			double _costs;

			void buildReducedCameraSystem( const SlamMap & map );
			void evaluateApproxHessians( const SlamMap & map );
			void fillSparseMatrix( const SlamMap & map );

			// calculate the augmented inverse Hessians of the points:
			void updateInverseAugmentedPointHessians();

			// set the cam sums to zero 
			void clear();

			/* reserve appropriate space and create internal blocks once! */
			void prepareSparseMatrix( size_t numCams );

			void setBlockInReducedSparse( const CamJTJ & m,
										  size_t bRow,
										  size_t bCol );

			void updateCameras( const Eigen::VectorXd & deltaCam,
							    SlamMap & map );


			/* solve Structure and re-eval costs on the fly */ 
			void solveStructure( Eigen::VectorXd & deltaStruct, 
								 const Eigen::VectorXd & deltaCam,
								 SlamMap & map );

			void undoStep( const Eigen::VectorXd & dCam,
						   const Eigen::VectorXd & dPoint,
						   SlamMap & map );

			void resize( size_t numCams, size_t numPoints );

			void evalScreenJacWrtPoint( Eigen::Matrix<double, 2, 1> & reproj,
										PointScreenJacType & jac,
										const Eigen::Matrix<double, 3, 1> & pInCam,
										const Eigen::Matrix<double, 3, 3> & K,
										const Eigen::Matrix<double, 3, 3> & R ) const;

	};
}

#endif
