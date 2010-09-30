#ifndef CVT_SPARSE_BUNDLE_ADJUSTMENT
#define CVT_SPARSE_BUNDLE_ADJUSTMENT

#include <vector>
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
			if( numIterations >= maxIterations )
				return true;
			if( minEps > lastCosts )
				return true;
			return false;
		}
		
		/* computing the intermediate block matrices (Schur complementing ...) */
		void computeIntermediateValues( SBAData & sbaData );
		
		void fillSparseMatrix(std::vector<Measurements> & measurements,											 
							  std::vector<Eigen::Matrix<double, 6, 6> > & cameraJacAccum,
							  std::vector<Eigen::Matrix<double, 6, 1> > & residualAccumForCam,
							  Eigen::VectorXd & eCam,
							  std::vector<std::vector<Eigen::Matrix<double, 6, 3>* > > & W,
							  std::vector<std::vector<Eigen::Matrix<double, 6, 3>* > > & Y,
							  int fillSize,
							  Eigen::SparseMatrix<double, Eigen::RowMajor> & RCS );
		
		void fillRCSMatrix(std::vector<Measurements> & measurements,											 
						   std::vector<Eigen::Matrix<double, 6, 6> > & cameraJacAccum,
						   std::vector<Eigen::Matrix<double, 6, 1> > & residualAccumForCam,
						   Eigen::VectorXd & eCam,
						   std::vector<std::vector<Eigen::Matrix<double, 6, 3>* > > & W,
						   std::vector<std::vector<Eigen::Matrix<double, 6, 3>* > > & Y,
						   Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> & RCS );
		
		void cleanUp();
		
		void resizeMatrices( SBAData & sbaData );
		void clearMatrices();
		void solveStructure( std::vector<Measurements> & measurements, 
							Eigen::VectorXd & deltaCam, 
							Eigen::VectorXd & deltaStruct );
		
		
		Eigen::Vector3d currProjection;
		Eigen::Vector4d currPointParams;
		
		const double jacDelta;
		double lambda;
		double lastCosts;		
		size_t numIterations;
		double minEps;
		size_t maxIterations;
		
		/* as described in MVG */
		std::vector<std::vector<Eigen::Matrix<double, 6, 3>* > > W;
		std::vector<std::vector<Eigen::Matrix<double, 6, 3>* > > Y;

		/* V_i */
		std::vector<Eigen::Matrix<double, 3, 3> > V;
		/* V_i^* */
		std::vector<Eigen::Matrix<double, 3, 3> > V_aug;
		/* (V_i^*)^{-1} */
		std::vector<Eigen::Matrix<double, 3, 3> > V_aug_inv;
		
		/* U_j: for each camera -> sum of jacobians of observed points, wrt current camera parameters */
		std::vector<Eigen::Matrix<double, 6, 6> > U;
		/* e_a_j */
		std::vector<Eigen::Matrix<double, 6, 1> > residualSumForCamera;
		Eigen::MatrixXd RCS;
		
	};
}

#endif 