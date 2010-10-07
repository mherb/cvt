#ifndef CVT_SPARSE_BUNDLE_ADJUSTMENT
#define CVT_SPARSE_BUNDLE_ADJUSTMENT

#include <vector>
#include <set>

#include <Eigen/Core>
#include <Eigen/Sparse>

#include "SBAData.h"
#include "Optimization.h"


namespace cvt {
	class SparseBundleAdjustment 
	{
	public:			
		SparseBundleAdjustment();
		~SparseBundleAdjustment();
		
		/*
		 *	SBAData must be properly filled 
		 *	- screen point predictions must be valid
		 *	- residuals computed
		 */
		double optimize( SBAData & data );
		
		size_t iterations()
		{
			return numIterations;
		}
		
		double epsilon()
		{
			return lastCosts;
		}
		
		void setTerminationCriteria( double eps, size_t maxIter )
		{
			minEps = eps;
			maxIterations = maxIter;
		}
		
	private:				
		/* Jacobians of predicted screen coordinate WRT point params 
		 */
		void pointJacobians( const CameraModel & camera, 
							 const Eigen::Vector4d & pointParams,
							 const Eigen::Vector3d & p3,
							 Eigen::Matrix<double, 2, 3> & jacobians );
		
		double buildAndSolveSystem( SBAData & sbaData, 
								    Eigen::VectorXd & deltaCam,
								    Eigen::VectorXd & deltaStruct );		
		
		bool checkTermination( )
		{
			if( ( costDecr < 1e-12 ) || 
			    ( numIterations >= maxIterations ) ||
			    ( minEps > lastCosts ) )
				return true;
			return false;
		}
		
		/* computing the intermediate block matrices (Schur complementing ...) */
		void computeIntermediateValues( SBAData & sbaData );
		
		void fillSparseMatrix( SBAData & sbaData );	
		void fillRCSMatrix( SBAData & sbaData );
		
		void cleanUp();
		
		void resizeMatrices( SBAData & sbaData );
		void clearMatrices();
		void solveStructure( Eigen::VectorXd & deltaCam, Eigen::VectorXd & deltaStruct );
		void calcAugmentations();
		void fillAndSolveMatrix( SBAData & sbaData, Eigen::VectorXd & deltaCam, Eigen::VectorXd & deltaStruct );
		double augmentAndSolveSystem( SBAData & sbaData, 
									  Eigen::VectorXd & deltaCam, 
									  Eigen::VectorXd & deltaStruct );
		
		
		
		Eigen::Vector3d currProjection;
		Eigen::Vector4d currPointParams;
		
		const double jacDelta;
		double lambda;
		double lastCosts;
		double costDecr;
		size_t numIterations;
		double minEps;
		size_t maxIterations;
		
		/* as described in MVG */
		std::vector<std::vector<Eigen::Matrix<double, 6, 3>* > > W;
		std::vector<std::vector<Eigen::Matrix<double, 6, 3>* > > Y;

		/* for each point V_i = J_P_i^T Cov_i^{-1} J_P_ */
		std::vector<Eigen::Matrix<double, 3, 3> > V;
		/* V_i^* */
		std::vector<Eigen::Matrix<double, 3, 3> > V_aug;
		/* (V_i^*)^{-1} */
		std::vector<Eigen::Matrix<double, 3, 3> > V_aug_inv;
		
		/* U_j: for each camera -> sum of jacobians of observed points, wrt current camera parameters */
		std::vector<Eigen::Matrix<double, 6, 6> > U;
		/* e_a_j */
		std::vector<Eigen::Matrix<double, 6, 1> > residualSumForCamera;
		/* e_b_i */
		std::vector<Eigen::Vector3d> residualSumForPoint;

		Eigen::SparseMatrix<double, Eigen::RowMajor> SRCS;		
		Eigen::SparseLDLT<Eigen::SparseMatrix<double, Eigen::RowMajor> > ldlt;
		
		Eigen::VectorXd eCam;
		
		typedef double (SparseBundleAdjustment::*UpdateFunc)( SBAData&, Eigen::VectorXd&, Eigen::VectorXd& );
	};
}

#endif 