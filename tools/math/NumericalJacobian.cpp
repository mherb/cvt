/*
 *  NumericalJacobian.h
 *  VisionTools
 *
 *  Created by Sebastian Klose on 08.08.10.
 *  Copyright 2010. All rights reserved.
 *
 */

//#include "NumericalJacobian.h"

NumericalJacobian<VectorType, MatrixType>::NumericalJacobian(unsigned int measDim, 
															 unsigned int parameterDimension):
	v(parameterDimension),
	jac(measDim, parameterDimension)	
{	
}

NumericalJacobian<VectorType, MatrixType>::~NumericalJacobian()
{
}

template
<class CostType>
MatrixType const& NumericalJacobian::jacobian(VectorType const& currentParams, 
	   							     CostType const& costFunc)
{	
}

