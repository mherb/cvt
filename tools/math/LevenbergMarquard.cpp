/*
 *  LevenbergMarquard.cpp
 *  VisionTools
 *
 *  Created by Sebastian Klose on 07.08.10.
 *  Copyright 2010. All rights reserved.
 *
 */

#include "LevenbergMarquard.h"

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
void LevenbergMarquard::optimize(CostFunctionType& costs,
								 JacobianFunctionType& jacFunc,
								 Eigen::VectorXd & startParameters, 
								 Eigen::VectorXd & measurement,
								 Eigen::VectorXd & deltaParameters)
{
	Eigen::VectorXd residual(measurement.rows());
	
	Eigen::VectorXd currentParameters(startParameters.rows());
	Eigen::VectorXd currentDeltaP(startParameters.rows());
	Eigen::VectorXd jTres(startParameters.rows());
	
	Eigen::MatrixXd jacobian(measurement.rows(), startParameters.rows());
	Eigen::MatrixXd jT(startParameters.rows(), measurement.rows());
	Eigen::MatrixXd jTj(startParameters.rows());
	
	deltaParameters.setZero();	
	currentParameters = startParameters;
	
	// start the iteration:
	unsigned int currentIter = 1;
	double currentCosts;
	double lastCosts;
	
	while (true) {
		// get the residual:
		costs.residual(currentParameters, measurement, residual);
	
		// get the jacobians
		jacFunc.jacobian(currentParameters, jacobian);
		jT = jacobian.transpose();
		jTj = jT*jacobian;
		jTres = jT*residual;
		jTres.cwise()*=-1.0;
		
		// add lambda to the diagonal
		jTj.diagonal().cwise()+=lamda;
		
		// get the costs before the update:
		lastCosts = costs.costFunction(currentParameters, measurement);
				
		// do the update:
		jTj.svd().solve(jTres, &currentDeltaP);
		
		// update parameters:
		currentParameters+=currentDeltaP;
		
		// evaluate the current costs
		currentCosts = costs.costFunction(currentParameters, measurement);
		
		if(currentCosts > lastCosts){
			currentParameters-=currentDeltaP;
			lamda*=lamdaUpdateFactor;
			continue;
		}
		
		lamda/=lamdaUpdateFactor;
		
		currentIter++;
		if(checkTermination(currentIter, currentCosts)){
			break;
		}
	}
	
	lamda = 0.001;
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