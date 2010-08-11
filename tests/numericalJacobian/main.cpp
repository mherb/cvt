#include <iostream>
#include <vector>

#include <tools/math/LevenbergMarquard.h>

// include all eigen stuff
#include <Eigen/Eigen>

using namespace Eigen;

class TestCostFunc
{
	public:
		TestCostFunc(){}
		~TestCostFunc(){}

		/*
		 *	@param startParams	current parameters
		 *	@param model		model values corresponding to the measurements
		 *	@param meas			current measurements
		 */
		virtual double costs(Eigen::VectorXd const& startParams,
							 std::vector<Eigen::VectorXd>& model,
							 std::vector<Eigen::VectorXd>& meas)
		{
			double costs = 0.0;
			Eigen::VectorXd res(2);

			for (unsigned int i = 0; i < meas.size(); i++) {
				this->transform(startParams, model[i], res);
				res-=meas[i];
				costs += (res).squaredNorm();
			}
			return costs;
		}

		virtual void residual(Eigen::VectorXd const& params,
							  std::vector<Eigen::VectorXd>& model,
							  std::vector<Eigen::VectorXd>& meas,
							  Eigen::VectorXd& res)
		{
			Eigen::VectorXd tmpRes(meas[0].rows());
			for(unsigned int i = 0; i < model.size(); i++){
				this->transform(params, model[i], tmpRes);
				tmpRes-=meas[i];
				res.block(i*tmpRes.rows(), 0, tmpRes.rows(), 1) = tmpRes;
			}
		}

		void transform(Eigen::VectorXd const & parameters,
					   Eigen::VectorXd const & point,
					   Eigen::VectorXd & transformed)
		{
			transformed[0] = point[0] * cos(parameters[0]) - point[1]*sin(parameters[0]) + parameters[1];
			transformed[1] = point[0] * sin(parameters[0]) + point[1]*cos(parameters[0]) + parameters[2];
		}

		virtual void jacobian(Eigen::VectorXd const& parameters, Eigen::VectorXd const& point, Eigen::MatrixXd & jac)
		{
			jac(0, 0) = -point[0] * sin(parameters[0]) - point[1]*cos(parameters[0]);
			jac(1, 0) =  point[0] * cos(parameters[0]) - point[1]*sin(parameters[0]);

			jac(0, 1) = 1.0;
			jac(1, 1) = 0.0;

			jac(0, 2) = 0.0;
			jac(1, 2) = 1.0;
		}

		/*virtual void jacobians(Eigen::VectorXd const& parameters,
							   std::vector<Eigen::VectorXd> const& points,
							   Eigen::MatrixXd & jac)
		{
			double sinp = sin(parameters[0]);
			double cosp = cos(parameters[0]);

			for(unsigned int i = 0; i < points.size(); i++){
					jac(2*i, 0) = -points[i][0] * sinp - points[i][1]*cosp;
					jac(2*i+1, 0) =  points[i][0] * cosp - points[i][1]*sinp;

					jac(2*i, 1) = 1.0;
					jac(2*i+1, 1) = 0.0;

					jac(2*i, 2) = 0.0;
					jac(2*i+1, 2) = 1.0;
			}
		}*/

		void jacobians(Eigen::VectorXd const& parameters,
					   std::vector<Eigen::VectorXd> const& points,
					   Eigen::MatrixXd & jac)
		{
			Eigen::VectorXd parPlus(3);
			Eigen::VectorXd pT(2), pTplus(2);

			double delta;
			double possibleStep;

			for(unsigned int i = 0; i < points.size(); i++){
				this->transform(parameters, points[i], pT);
				for(int p = 0; p < parameters.rows(); p++){
					parPlus = parameters;

					possibleStep = fabs(parameters[p]*0.0001);
					delta = (possibleStep>0.000001)?possibleStep:0.000001;
					parPlus[p] += delta;

					this->transform(parPlus, points[i], pTplus);

					pTplus-=pT;
					pTplus*=(1.0/delta);

					jac.block(i*pT.rows(), p, 2, 1) = pTplus;
				}
			}
		}
};

int main(void)
{
	int ret = EXIT_SUCCESS;

	TestCostFunc costFunc;

	std::vector<Eigen::VectorXd> measurements;
	std::vector<Eigen::VectorXd> originals;

	Eigen::VectorXd gtParameters(3);
	gtParameters[0] = M_PI/6.0;
	gtParameters[1] = 320.0;
	gtParameters[2] = 240.0;

	Eigen::VectorXd tmp(2);
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

	Eigen::VectorXd currentParameters(3);
	currentParameters.setZero();

	tools::LevenbergMarquard lm(tools::LevenbergMarquard::LevenbergMarquard::IterationsOrEpsilon);
	lm.setMaxIterations(10);
	lm.setMaxEpsilon(0.0000001);

	Eigen::VectorXd result(3);
	double eps = lm.optimize<TestCostFunc, TestCostFunc>(costFunc, costFunc, currentParameters, measurements, originals, result );

	std::cout << "Ground Truth parameters: \n" << gtParameters << std::endl;
	std::cout << "LM result: \n" << result << std::endl;
	std::cout << "Final Epsilon: " << eps << std::endl;

	return ret;
}
