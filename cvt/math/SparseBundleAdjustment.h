#ifndef CVT_SPARSE_BUNDLE_ADJUSTMENT
#define CVT_SPARSE_BUNDLE_ADJUSTMENT

#include <vector>
#include <set>

#include <Eigen/Core>
#include <Eigen/Sparse>

#include <cvt/vision/SlamMap.h> 
#include <cvt/math/SparseBlockMatrix.h>
#include <cvt/math/JointMeasurements.h>

namespace cvt {
	class SparseBundleAdjustment
	{
		public:
			SparseBundleAdjustment();
			~SparseBundleAdjustment();

			double optimize( SlamMap & data, const TerminationCriteria & criteria );

			size_t iterations() const { return _iterations; }
			double epsilon()	const { return _costs; }

		private:
			/* jacobians for each point */		
			const size_t pointParamDim = 3;
			const size_t camParamDim   = 6;
			typedef Eigen::Matrix<double, 2, pointParamDim>				PointScreenJacType;
			typedef Eigen::Matrix<double, 2, camParamDim>				CamScreenJacType;
			typedef Eigen::Matrix<double, camParamDim, camParamDim>		CamJTJ;
			typedef Eigen::Matrix<double, pointParamDim, pointParamDim>	PointJTJ;
			typedef Eigen::Matrix<double, camParamDim, 1>				CamResidualType;
			typedef Eigen::Matrix<double, pointParamDim, 1>				PointResidualType;


			/* approx. Point Hessians */
			std::vector<PointJTJ,		   Eigen::aligned_allocator>	_pointsJTJ;

			/* inverse of the augmented approx Point Hessians */
			std::vector<PointJTJ,		   Eigen::aligned_allocator>	_invAugPJTJ;

			std::vector<CamJTJ,			   Eigen::aligned_allocator>	_camsJTJ;
			std::vector<Eigen::Vector2d,   Eigen::aligned_allocator>	_residuals;
			std::vector<CamResidualType,   Eigen::aligned_allocator>	_camResiduals;
			std::vector<PointResidualType, Eigen::aligned_allocator>	_pointResiduals;
			
			/* Sparse Upper Left of the approx. Hessian */
			SparseBlockMatrix<camParamDim, pointParamDim>				_camPointJTJ;

			JointMeasurements											_jointMeasures;

			Eigen::SparseMatrix<double, Eigen::ColMajor>				_sparseReduced;
			Eigen::VectorXd												_reducedRHS;

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
			void prepareSparseMatrix();

	};
}

#endif
