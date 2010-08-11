/*
 *  LevenbergMarquard.cpp
 *  VisionTools
 *
 *  Created by Sebastian Klose on 07.08.10.
 *  Copyright 2010. All rights reserved.
 *
 */

namespace tools {

	LevenbergMarquard::LevenbergMarquard(TerminationCriteria termCrit):
		maxIterations(20),
		maxEpsilon(0.0001),
		lamda(0.001),
		lamdaUpdateFactor(10.0),
		terminationCriteria(termCrit)
	{
	}

	LevenbergMarquard::~LevenbergMarquard()
	{
	}

	void LevenbergMarquard::setMaxIterations(unsigned int maxIters)
	{
		maxIterations = maxIters;
	}

	void LevenbergMarquard::setMaxEpsilon(double maxEps)
	{
		maxEpsilon = maxEps;
	}

	template <class CostFunctionType, class JacobianFunctionType>
		double LevenbergMarquard::optimize(CostFunctionType& costs,
										   JacobianFunctionType& jacFunc,
										   Eigen::VectorXd & startParameters,
										   std::vector<Eigen::VectorXd> & measurements,
										   std::vector<Eigen::VectorXd> & model,
										   Eigen::VectorXd & resultParameters)
		{
			int measurementDimension = measurements[0].rows();
			int jacRows = measurementDimension * (int)measurements.size();
			int parameterDimension = startParameters.rows();

			Eigen::VectorXd currentDeltaP(parameterDimension);
			Eigen::VectorXd jTres(parameterDimension);


			Eigen::MatrixXd jacobian(jacRows, parameterDimension);
			Eigen::VectorXd residual(jacobian.rows());
			Eigen::MatrixXd jT(parameterDimension, jacobian.rows());
			Eigen::MatrixXd jTj(parameterDimension, parameterDimension);

			resultParameters = startParameters;

			// start the iteration:
			unsigned int currentIter = 1;
			double currentCosts = costs.costs(resultParameters, model, measurements);
			double lastCosts;

			while (true) {
				// get the residual:
				costs.residual(resultParameters,
							   model,
							   measurements,
							   residual);

				jacFunc.jacobians(resultParameters,
								  model,
								  jacobian);

				// get the jacobians
				jT = jacobian.transpose();
				jTj = jT*jacobian;
				jTres = jT*residual;
				jTres*=-1.0;

				// add lambda to the diagonal
				jTj.diagonal().cwise()+=lamda;

				// save the costs before the update:
				lastCosts = currentCosts;

				// do the update:
				jTj.svd().solve(jTres, &currentDeltaP);

				// update parameters:
				resultParameters+=currentDeltaP;

				// evaluate the current costs
				currentCosts = costs.costs(resultParameters, model, measurements);

				if(currentCosts > lastCosts){
					resultParameters-=currentDeltaP;
					lamda*=lamdaUpdateFactor;
					continue;
				}

				lamda/=lamdaUpdateFactor;

				currentIter++;
				if(checkTermination(currentIter, currentCosts))
				{
					break;
				}

				std::cout << "CurrentIteration: " << currentIter << " Epsilon: " << currentCosts << std::endl;
			}

			lamda = 0.001;

			return currentCosts;
		}

	bool LevenbergMarquard::checkTermination(unsigned int currIter, double currEpsilon)
	{
		if(terminationCriteria == Iterations || terminationCriteria == IterationsOrEpsilon){
			if (currIter > maxIterations) {
				return true;
			}
		}

		if(terminationCriteria == Epsilon || terminationCriteria == IterationsOrEpsilon){
			if (currEpsilon < maxEpsilon) {
				return true;
			}
		}

		return false;
	}

}
