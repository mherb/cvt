#include <iostream>
#include <vector>

#include <cvt/math/Math.h>

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

int expTest()
{	
	Eigen::Matrix3d A, gt, diff, expA;
	
	A.setZero(); gt.setIdentity();
	cvt::Math::exponential<Eigen::Matrix3d>( A, expA );

	// exp(0) = I
	gt -= expA;	
	if( ( gt.sum() / 9.0 ) > 1e-7 ){
		std::cout << "exp( 0 ) should be identity, result:\n" << expA << std::endl;
		return EXIT_FAILURE;
	}
	
	// test 1:
	A << 0.01, 0.2, 0.7, 1, 0.8, 0, 0.5, 0.6, 0.9;	
	gt << 1.52175688997768, 0.74086263293972, 1.2668677744925,
		  1.71508600350382, 2.54513697254656, 0.66307572039826,
		  1.47325617069315, 1.62960875457729, 2.94303856886149;
	
	cvt::Math::exponential<Eigen::Matrix3d>( A, expA );
	diff = gt - expA;	
	if( ( diff.sum() / 9.0 ) > 1e-7 ){
		std::cout << "exp should be\n" << gt << "\nresult:\n" << expA << std::endl;
		return EXIT_FAILURE;
	}
	
	// test 2:
	A << 0.814723686393179, 0.913375856139019, 0.278498218867048,
		 0.905791937075619, 0.63235924622541,  0.546881519204984,
		 0.126986816293506, 0.0975404049994095,0.957506835434298;
	gt << 3.28809877964228, 2.22902628122005, 1.38018121042008,
		  2.26170206577467, 2.87122947992211, 1.71281407137404,
		  0.461456417001916,0.390977932699033,2.75537321694693;
	cvt::Math::exponential<Eigen::Matrix3d>( A, expA );
	diff = gt - expA;	
	if( ( diff.sum() / 9.0 ) > 1e-7 ){
		std::cout << "exp should be\n" << gt << "\nresult:\n" << expA << std::endl;
		return EXIT_FAILURE;
	}	
	
	return EXIT_SUCCESS;
}

int main(void)
{
	std::cout << "Some tests of the eigen2 library" << std::endl;

	int ret = EXIT_SUCCESS;

//	ret |= matrixTests();
//	ret |= arithmetic();
	ret |= expTest();

	return ret;
}
