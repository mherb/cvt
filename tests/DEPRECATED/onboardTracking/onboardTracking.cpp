#include <iostream>
#include <vector>
#include <Eigen/Core>
#include <Eigen/LU>
#include <Eigen/SVD>
#include <Eigen/Geometry>

#include "VisiblePoints.h"

#include <cvt/util/Exception.h>
#include <cvt/util/Time.h>
#include <cvt/gfx/Image.h>

#include <cv.h>
#include <highgui.h>

int main(int argc, char* argv[])
{
	std::cout << "Test application for quadrocopter onboard tracking system" << std::endl;

	if(argc < 2){
		std::cout << "Usage: " << argv[0] << " <fileName> " << std::endl;
		return 0;
	}

	// angles in RAD
	double alphaCam = 0.0 * M_PI/180.0;
	double betaCam = 0.0 * M_PI/180.0;
	double gammaCam = 0.0 * M_PI/180.0;

	Eigen::Vector3d translation;
	translation[0] = 0.0;
	translation[1] = 0.0;
	translation[2] = 1500.0;

	Eigen::Transform3d T;
	T.setIdentity();

	// rotation around Z axis not applied yet
	Eigen::AngleAxisd rot = Eigen::AngleAxisd(gammaCam, Eigen::Vector3d::UnitZ()) *
							Eigen::AngleAxisd(betaCam, Eigen::Vector3d::UnitY()) *
							Eigen::AngleAxisd(alphaCam, Eigen::Vector3d::UnitX());

	T.rotate(rot);
	T.translation() = translation;

	Eigen::Matrix<double, 3, 4> K;
	K.setZero();
	double focus = 1000.0;

	K(0, 0) = focus;
	K(0, 2) = 320;
	K(1, 1) = focus;
	K(1, 2) = 240;
	K(2, 2) = 1.0;

	Eigen::Matrix<double, 3, 4> P = K*T.matrix();

	std::cout << "K: " << K << std::endl;
	std::cout << "T: " << T.matrix() << std::endl;
	std::cout << "P: " << P << std::endl;

	Eigen::MatrixXd points;
	Eigen::MatrixXd edges;

	cv::namedWindow("Test");

	cvt::Image img(640, 480);

	cvt::Time timer;

	try {
		VisiblePoints visiblePoints(argv[1]);

		Eigen::Matrix<double, 2, Eigen::Dynamic> screenNormals;

		timer.reset();
		for(unsigned int a = 0; a < 100; a++){
			ViewInformation const& visInfo = visiblePoints.get(alphaCam, betaCam);

			points = P * visInfo.points;
			edges = P * visInfo.edges;

			screenNormals.setZero(2, points.cols());

			CvPoint point;
			for( unsigned int i = 0; i < (unsigned int)points.cols(); i++ ){
				/*point.x = cvRound(points(0, i) / points( 2, i ));
				point.y = cvRound(points(1, i) / points( 2, i ));
				cvCircle( img.iplimage(), point, 1, CV_RGB( 255, 0, 0 ), 2 );*/

				screenNormals( 0, i ) = edges( 1, 2*i )/edges( 2, 2*i ) - edges( 1, 2 * i + 1 ) / edges( 2, 2 * i + 1 );
				screenNormals( 1, i ) = -edges( 0, 2*i )/edges( 2, 2*i ) + edges( 0, 2 * i + 1 ) / edges( 2, 2 * i + 1 );
				double normLen = 1.0 / sqrt(cvt::Math::sqr(screenNormals(0, i)) + cvt::Math::sqr(screenNormals(1, 4)));
				screenNormals( 0, i ) *= normLen;
				screenNormals( 1, i ) *= normLen;

			}

			//cvShowImage("Test", img.iplimage());
			//cvWaitKey(0);
		}
		std::cout << "Time: " << timer.elapsedMiliSeconds()/100.0 << "ms" << std::endl;

	} catch (cvt::Exception const & e){
		std::cout << e.what() << std::endl;
	}

	return 0;
}
