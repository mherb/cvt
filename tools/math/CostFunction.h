/*
 *  CostFunction.h
 *  VisionTools
 *
 *  Created by Sebastian Klose on 07.08.10.
 *  Copyright 2010. All rights reserved.
 *
 */
#ifndef TOOLS_COST_FUNCTION_H
#define TOOLS_COST_FUNCTION_H

#include <Eigen/Eigen>

class CostFunction
{
public:
	virtual ~CostFunction() = 0;
	
	virtual double evaluate(Eigen::VectorXd const& parameters,
							Eigen::VectorXd const& measurement)=0;
	
};

#endif
