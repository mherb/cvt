#include <iostream>
#include <vector>

#include <cvt/math/Math.h>
#include <cvt/util/Time.h>
#include <cvt/vision/DLT.h>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "RANSAC.h"

using namespace cvt;

void genpoints( std::vector<Eigen::Vector2d> & pts, size_t numPts )
{
	Eigen::Vector2d p;
	for( size_t i = 0; i < numPts; i++ ){
		p[ 0 ] = Math::rand( 0.0, 640.0 );
		p[ 1 ] = Math::rand( 0.0, 480.0 );
		pts.push_back( p );
	}
}

void transformNoise( std::vector<Eigen::Vector2d> & pts, std::vector<Eigen::Vector2d> & trans, Eigen::Matrix3d & H )
{
	Eigen::Vector3d p, pPrime;
	Eigen::Vector2d p2;
	p[ 2 ] = 1.0;
	
	double noiseRange = 6.0;
	for( size_t i = 0; i < pts.size(); i++ ){
		p[ 0 ] = pts[ i ][ 0 ];
		p[ 1 ] = pts[ i ][ 1 ];
		
		pPrime = H * p;
		
		
		p2[ 0 ]	= pPrime[ 0 ] / pPrime[ 2 ] + Math::rand( -noiseRange, noiseRange );
		p2[ 1 ]	= pPrime[ 1 ] / pPrime[ 2 ] + Math::rand( -noiseRange, noiseRange );
		
		trans.push_back( p2 );		
	}	
}

void transform3d( std::vector<Eigen::Vector2d> & pts, std::vector<Eigen::Vector2d> & trans, Eigen::Matrix3d & K )
{	
	
	Eigen::Matrix3d R = ( Eigen::AngleAxisd( Math::deg2Rad( 10.0f ), Eigen::Vector3d::UnitZ() ) *
						  Eigen::AngleAxisd( Math::deg2Rad(  0.0f ), Eigen::Vector3d::UnitY() ) *
						  Eigen::AngleAxisd( Math::deg2Rad( 10.0f ), Eigen::Vector3d::UnitX() ) ).toRotationMatrix();
	Eigen::Vector3d t( 500, 300, 200 );
	
	std::cout << "Ground Truth Rotation:\n" << R << std::endl;
	std::cout << "Ground Truth Translation:\n" << t << std::endl;
	
	Eigen::Vector3d p, pPrime;
	Eigen::Vector2d p2;
	p[ 2 ] = 0.0;

	double noiseRange = 0.5;
	for( size_t i = 0; i < pts.size(); i++ ){
		p[ 0 ] = pts[ i ][ 0 ];
		p[ 1 ] = pts[ i ][ 1 ];

		pPrime = K*R*p + K*t;

		p2[ 0 ]	= pPrime[ 0 ] / pPrime[ 2 ] + Math::rand( -noiseRange, noiseRange );
		p2[ 1 ]	= pPrime[ 1 ] / pPrime[ 2 ] + Math::rand( -noiseRange, noiseRange );

		trans.push_back( p2 );
	}
}

void convert3d( Eigen::Matrix3d & H, Eigen::Matrix3d & K, Eigen::Matrix<double, 3, Eigen::Dynamic> & pts )
{
	Eigen::Matrix3d tmp = K.inverse() * H;
	tmp *= 1.0 / tmp.col( 0 ).norm();

	Eigen::Vector3d t = tmp.col( 2 );
	tmp.col( 2 ) = tmp.col( 0 ).cross( tmp.col( 1 ) );

	Eigen::Matrix<double, 3, Eigen::Dynamic> pT = tmp * pts;

	double min = pT.row( 2 ).minCoeff();

	if( min + t[ 2 ] < 0.0 ) {
		tmp *= -1.0;
		t = tmp.col( 2 );
		tmp.col( 2 ) = tmp.col( 0 ).cross( tmp.col( 1 ) );
	}

	std::cout << "R:\n" << tmp << std::endl;
	std::cout << "t:\n" << t << std::endl;
}

int main( )
{
	Eigen::Matrix3d H_gt = Eigen::Matrix3d::Identity();
	/*H_gt( 0, 0 ) = 0.9;
	H_gt( 1, 1 ) = 0.8;
	H_gt( 0, 2 ) = 200;
	H_gt( 1, 2 ) = 100;*/

	Eigen::Matrix3d K = Eigen::Matrix3d::Identity();
	K( 0, 0 ) = 600.0;
	K( 1, 1 ) = 602.0;
	K( 0, 2 ) = 400.0;
	K( 1, 2 ) = 300.0;

	std::vector<Eigen::Vector2d> reference;
	genpoints( reference, 50 );

	std::vector<Eigen::Vector2d> trans;

	//transformNoise( reference, trans, H_gt );
	transform3d( reference, trans, K );

	Eigen::Matrix3d H_dlt, H_ndlt;

	Eigen::Matrix<double, 3, Eigen::Dynamic> pMat( 3, reference.size() );
	Eigen::Matrix<double, 3, Eigen::Dynamic> tMat( 3, reference.size() );
	for( size_t i = 0; i < reference.size(); i++ ){
		pMat( 0, i ) = reference[ i ][ 0 ];
		pMat( 1, i ) = reference[ i ][ 1 ];
		pMat( 2, i ) = 1.0;

		tMat( 0, i ) = trans[ i ][ 0 ];
		tMat( 1, i ) = trans[ i ][ 1 ];
		tMat( 2, i ) = 1.0;
	}
	//tMat = H_gt * pMat;

	Time time;

	#define NUMITER ( 1 )
	time.reset();
	for( size_t i = 0; i < NUMITER; i++ )
		DLT::dlt( reference, trans, H_dlt );
	std::cout << "Standard DLT took: " << time.elapsedMilliSeconds() / NUMITER << std::endl;
	std::cout << "DLT: \n" << H_dlt << "\n" << std::endl;
	time.reset();
	for( size_t i = 0; i < NUMITER; i++ )
		DLT::ndlt( reference, trans, H_ndlt );
	std::cout << "Standard NDLT took: " << time.elapsedMilliSeconds() / NUMITER << std::endl;
	std::cout << "Normalized DLT: \n" << H_ndlt << std::endl;
	time.reset();

	for( size_t i = 0; i < NUMITER; i++ )
		DLT::dlt( pMat, tMat, H_dlt );
	std::cout << "EigenMatrix DLT took: " << time.elapsedMilliSeconds() / NUMITER << std::endl;
	std::cout << "DLT: \n" << H_dlt << "\n" << std::endl;
	time.reset();

	for( size_t i = 0; i < NUMITER; i++ )
		DLT::ndlt( pMat, tMat, H_dlt );
	std::cout << "EigenMatrix NDLT took: " << time.elapsedMilliSeconds() / NUMITER << std::endl;
	std::cout << "DLT: \n" << H_dlt << "\n" << std::endl;

	// RANSAC DLT:
	// sigma= 2.0 pixel, outlierProb = 10%
	RANSAC ransac( 4.0, 0.1 );
	time.reset();
	ransac.dlt( pMat, tMat, H_dlt );
	std::cout << "RANSAC took: " << time.elapsedMilliSeconds() << std::endl;
	std::cout << "DLT: \n" << H_dlt << "\n" << std::endl;
	
	convert3d( H_dlt, K, pMat );
		
	return 0;
}
