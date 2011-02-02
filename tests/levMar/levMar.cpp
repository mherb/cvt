#include <iostream>

#include <Eigen/Core>

#include <cvt/math/LevenbergMarquard.h>
#include <cvt/math/CostFunction.h>
#include <cvt/math/Math.h>
#include <cvt/util/RNG.h>

#include <cvt/vision/CamModel.h>
#include <cvt/vision/Warp.h>

using namespace cvt;



void generateData( PointCorrespondences3d2d<double> & data, size_t n,
				   SE3<double> & pose, CamModel<double> & cam )
{
	RNG rng( time( NULL ) );
	//double sigma = 0.1;
	
	Eigen::Matrix<double, 3, 1> p3d;
	Eigen::Matrix<double, 2, 1> p2d;
	
	while ( n-- ) {
		p3d[ 0 ] = rng.uniform( -100.0, 100.0 );
		p3d[ 1 ] = rng.uniform( -100.0, 100.0 );
		p3d[ 2 ] = rng.uniform( -100.0, 100.0 );	
		
		Warp::toScreen<double>( p2d, cam, pose, p3d );
				
		// add some noise
		//p2d[ 0 ] += rng.gaussian( sigma ); p2d[ 1 ] += rng.gaussian( sigma );
		
		std::cout << "Correspondence: <" << p3d[ 0 ] << ", " << p3d[ 1 ] << ", " << p3d[ 2 ] << ">, <"
										 << p2d[ 0 ] << ", " << p2d[ 1 ] << ">\n";
		
		data.add( p3d, p2d );
	}
	std::cout << std::endl;
}

void generateData( PointCorrespondences3d3d<double> & data, size_t n,
				  SE3<double> & pose, CamModel<double> & cam )
{
	RNG rng( time( NULL ) );
	//double sigma = 0.1;
	
	Eigen::Matrix<double, 3, 1> p3d;
	Eigen::Matrix<double, 3, 1> pWarped;
	
	while ( n-- ) {
		p3d[ 0 ] = rng.uniform( -1000.0, 1000.0 );
		p3d[ 1 ] = rng.uniform( -1000.0, 1000.0 );
		p3d[ 2 ] = rng.uniform( -1000.0, 1000.0 );
		
		pose.transform( pWarped, p3d );
		
		// add some noise
		//p2d[ 0 ] += rng.gaussian( sigma ); p2d[ 1 ] += rng.gaussian( sigma );
		
		//std::cout << "Adding Point Pair: <" << p3d[ 0 ] << ", " << p3d[ 1 ] << ", " << p3d[ 2 ] << ">,"
		//			<< " <" << pWarped[ 0 ] << ", " << pWarped[ 1 ] << ", " << pWarped[ 2 ] << ">" << std::endl;
		
		data.add( p3d, pWarped );
		
	}
	std::cout << std::endl;
}


int main()
{
	size_t numPoints = 30;
	
	Eigen::Matrix<double, 3, 3> K = Eigen::Matrix<double, 3, 3>::Identity();
	Eigen::Matrix<double, 4, 4> T = Eigen::Matrix<double, 4, 4>::Identity();
	
	K( 0, 0 ) = 600; K( 0, 2 ) = 399.5;
	K( 1, 1 ) = 600; K( 1, 2 ) = 299.5;
	
	CamModel<double> cam( K, T );
	
	CameraRegistry<double> & camRegistry = CameraRegistry<double>::instance();	
	size_t camId = camRegistry.registerCamera( cam );
	
	PointCorrespondences3d2d<double> data( camId );
	//PointCorrespondences3d3d<double> data( camId );
	
	Eigen::Matrix<double, 6, 1> poseVec;
	poseVec[ 0 ] = 0; 
	poseVec[ 1 ] = 0; 
	poseVec[ 2 ] = 0.0;
	poseVec[ 3 ] = 100; 
	poseVec[ 4 ] = 200; 
	poseVec[ 5 ] = 300;
	
	SE3<double> pose( poseVec );
	
	std::cout << "Ground Truth:\n" << pose.transformation() << std::endl;
	
	// generate random points
	generateData( data, numPoints, pose, cam );		
	
	// perturbe the current pose, to use it as initial estimate
	
	poseVec[ 0 ] += Math::deg2Rad( 10.0 );
	poseVec[ 1 ] -= Math::deg2Rad( 0.0 );
	poseVec[ 2 ] += Math::deg2Rad( 5.0 );
	poseVec[ 3 ] -= 10;
	poseVec[ 4 ] -= 10;
	poseVec[ 5 ] += 1;
	pose.set( poseVec );

	TerminationCriteria<double> termCriteria;
	termCriteria.setCostThreshold( 0.0000001 );
	termCriteria.setMaxIterations( 20 );
	
	LevenbergMarquard<double> lm( termCriteria );
	
	lm.optimize( pose, data, ( TERM_COSTS_THRESH | TERM_MAX_ITER ) );
	
	std::cout << "Optimized:\n" << pose.transformation() << std::endl;
	std::cout << "Iterations: " << lm.iterations() << std::endl;
	std::cout << "Final Costs: " << lm.costs() << std::endl;
	
	return 0;
}