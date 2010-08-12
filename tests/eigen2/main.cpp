#include <iostream>
#include <vector>

#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/SVD>

using namespace Eigen;

int arithmetic()
{
	Matrix3d mat1;
	Matrix3d mat2;

	mat1.setRandom();
	mat2.setRandom();

	std::cout << "M1: \n" << mat1 << "\nM2\n" << mat2 << std::endl;
	std::cout << "M1*M2:\n" << mat1*mat2 << std::endl;

	return EXIT_SUCCESS;
}

int matrixTests()
{
	// floating point 3x3 matrix:
	Eigen::Matrix3f mat;
	mat << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0;
	std::cout << mat << std::endl;

	Eigen::Matrix3d matDoubles;
	matDoubles << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0;

	std::cout << matDoubles << std::endl;

	// type casting
	Eigen::Matrix3f mf = matDoubles.cast<float>();

	// mapping std::vectors ...
	std::vector<double> values(10, 0);
	Map<VectorXd> vec(&values[0], (unsigned int)values.size());

	std::cout << "Mapped std::vector:\n" << vec << std::endl;
	std::cout << "Setting matrices blockwise:" << std::endl;
	MatrixXf blockM(5, 5);
	blockM << mf, // 3x3
			  MatrixXf::Zero(3, 5-3),
			  MatrixXf::Zero(5-3, 3),
			  MatrixXf::Identity(5-3, 5-3);
	std::cout << blockM << std::endl;

	return EXIT_SUCCESS;
}

int main(void)
{
	std::cout << "Some tests of the eigen2 library" << std::endl;

	int ret = EXIT_SUCCESS;

	ret |= matrixTests();
	ret |= arithmetic();

	return ret;
}
