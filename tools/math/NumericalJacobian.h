/*
 *  NumericalJacobian.h
 *  VisionTools
 *
 *  Created by Sebastian Klose on 08.08.10.
 *  Copyright 2010. All rights reserved.
 *
 */

#ifndef TOOLS_NUMERICAL_JACOBIAN_H
#define TOOLS_NUMERICAL_JACOBIAN_H

template 
<class VectorType,
 class MatrixType>		
class NumericalJacobian
{
public:
	NumericalJacobian(unsigned int measDim, 
					  unsigned int parameterDimension);
	
	virtual ~NumericalJacobian();
	
	template
	<class CostType>
	MatrixType const& jacobian(VectorType const& currentParams, CostType const& costFunc);
	
private:
	VectorType v;
	MatrixType jac;	
};

#include "NumericalJacobian.cpp"

#endif
