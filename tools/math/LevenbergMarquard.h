/*
 *  LevenbergMarquard.h
 *  VisionTools
 *
 *  Created by Sebastian Klose on 07.08.10.
 *  Copyright 2010. All rights reserved.
 *
 */
#ifndef TOOLS_LEVENBERG_MARQUARD_H
#define TOOLS_LEVENBERG_MARQUARD_H

#include <Eigen/Core>
#include <Eigen/Array>

#include <vector>

#include <boost/function.hpp>

namespace tools {
	typedef Eigen::Matrix<double, Eigen::Dynamic, 1> DynamicEigenVec;
	typedef boost::function<double (DynamicEigenVec const&, 
									std::vector<DynamicEigenVec> const&,
									std::vector<DynamicEigenVec> const&,
									DynamicEigenVec &)> CostFunctionType;
	
	typedef boost::function<void (DynamicEigenVec const&,
								  std::vector<DynamicEigenVec> const&,
								  Eigen::MatrixXd &)> JacobianFunctionType;
	
	class LevenbergMarquard
	{
		public:
			enum TerminationCriteria
			{
				Iterations,
				Epsilon,
				IterationsOrEpsilon
			};

			LevenbergMarquard(TerminationCriteria termCrit);

			void setMaxIterations(unsigned int maxIters);
			void setMaxEpsilon(double maxEps);

			virtual ~LevenbergMarquard();
		
		//double optimize(boost::Function<double ()>)

			//template <class CostFunctionType, class JacobianFunctionType>
				double optimize(CostFunctionType & costs,
								JacobianFunctionType & jacFunc,
								Eigen::VectorXd const& start,
								std::vector<Eigen::VectorXd> const& measurements,
								std::vector<Eigen::VectorXd> const& model,
								Eigen::VectorXd & deltaParameters);

			bool checkTermination(unsigned int currIter, double currEpsilon);

		private:
			unsigned int maxIterations;
			double maxEpsilon;
			double lamda;
			const double lamdaUpdateFactor;

			TerminationCriteria terminationCriteria;
	};
}

#endif

