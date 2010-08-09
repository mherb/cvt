#include <iostream>
#include <vector>

// include all eigen stuff
#include <Eigen/Eigen>

using namespace Eigen;

class TestCostFunc
{
public:
	TestCostFunc(){}
	~TestCostFunc(){}
	
	double costs(Eigen::Vector3d const& currentP, 
				 std::vector<Eigen::Vector2d>& meas, 
				 std::vector<Eigen::Vector2d>& start)
	{
		double costs = 0.0;
		Eigen::Vector2d transformed;
		
		for (unsigned int i = 0; i < meas.size(); i++) {
			this->transform(currentP, start[i], transformed);
			
			costs += (meas[i]-transformed).squaredNorm();
		}
		return costs;
	}
	
	void transform(Eigen::Vector3d const & parameters,
				   Eigen::Vector2d const & point,
				   Eigen::Vector2d & transformed)
	{
		transformed[0] = point[0] * cos(parameters[0]) - point[1]*sin(parameters[0]) + parameters[0];
		transformed[1] = point[0] * sin(parameters[0]) + point[1]*cos(parameters[0]) + parameters[1];		
	}
	
	void jacobian()
	{
			
	}
};

int main(void)
{		
	int ret = EXIT_SUCCESS;
	
	TestCostFunc costFunc;

	std::vector<Eigen::Vector2d> measurements;
	std::vector<Eigen::Vector2d> originals;
	
	Eigen::Vector3d gtParameters;
	gtParameters[0] = M_PI/4.0;
	gtParameters[1] = 320.0;
	gtParameters[2] = 240.0;
	
	Eigen::Vector2d tmp;
	tmp[0] = -100; tmp[1] = -100;
	originals.push_back(tmp);
	tmp[0] = -100; tmp[1] = 100;
	originals.push_back(tmp);
	tmp[0] = 100; tmp[1] = 100;
	originals.push_back(tmp);
	tmp[0] = 100; tmp[1] = -100;
	originals.push_back(tmp);
	
	for(unsigned int i = 0; i < originals.size(); i++){
		costFunc.transform(gtParameters, originals[i], tmp);
		measurements.push_back(tmp);
	}
	
	Eigen::Vector3d currentParameters;
	currentParameters.setZero();
	
		
	return ret;
}