#ifndef CVT_OPTIMIZATION_H
#define CVT_OPTIMIZATION_H

#include <Eigen/Core>
#include <boost/function.hpp>

namespace cvt {
	typedef Eigen::Matrix<double, Eigen::Dynamic, 1> DynamicEigenVec;
    
    typedef boost::function<double (const DynamicEigenVec &,
									const std::vector<DynamicEigenVec> &,
									const std::vector<DynamicEigenVec> &,
									DynamicEigenVec &)> CostFunctionType;
    
    typedef boost::function<void (const DynamicEigenVec &,
								  const std::vector<DynamicEigenVec> &,
								  Eigen::MatrixXd &)> JacobianFunctionType;
    
    typedef boost::function<void (const DynamicEigenVec &,
								  const DynamicEigenVec &,
								  DynamicEigenVec &)> TransformFunctionType;
	
}

#endif