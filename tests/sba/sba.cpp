#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include <cvt/io/Resources.h>

#include <cvt/util/Exception.h>
#include <cvt/util/Timer.h>

#include <Eigen/Core>
#include <Eigen/Eigen>
#include <Eigen/Geometry>

#include <cv.h>
#include <cvaux.hpp>

// test app for sparse bundle adjustment, 
// test input files from the sba-lib:
struct Point3d 
{	
	Point3d( double x, double y, double z ):
		P( x, y, z )
	{		
	}
			
	Eigen::Vector3d P;
	std::vector<Eigen::Vector2d> observations;
	std::vector<size_t> frameIds;
};

struct CameraTransform {
	Eigen::Quaterniond orientation;
	Eigen::Vector3d	 translation;
};

void parseCalibFile( const std::string & file, Eigen::Matrix3d & K )
{
	std::ifstream fileIn( file.c_str() );
	std::istringstream lineBuffer;

	double val;
	size_t row = 0;
	size_t col = 0;

	std::string line;
	while ( std::getline( fileIn, line ) ){
		lineBuffer.clear();
		lineBuffer.str( line );
		col = 0;
		while ( lineBuffer >> val ){
			K( row, col++ ) = val;
		}
		row++;
	}	
	
	fileIn.close();	
}

void parseCameraFile( const std::string & file, std::vector<CameraTransform> & cameras )
{
	std::ifstream fileIn( file.c_str() );
	std::istringstream lineBuffer;
	
	double val;
	
	CameraTransform cam;
	
	std::string line;
	while ( std::getline( fileIn, line ) ){
		lineBuffer.clear();
		lineBuffer.str( line );
		
		lineBuffer >> val; cam.orientation.w() = val;
		lineBuffer >> val; cam.orientation.x() = val;
		lineBuffer >> val; cam.orientation.y() = val;
		lineBuffer >> val; cam.orientation.z() = val;
		lineBuffer >> val; cam.translation.x() = val;
		lineBuffer >> val; cam.translation.y() = val;
		lineBuffer >> val; cam.translation.z() = val;
		
		cameras.push_back( cam );
	}	
	
	fileIn.close();	
}

void parsePtsFile( const std::string & file, std::vector<Point3d> & points3d )
{
	std::ifstream fileIn( file.c_str() );
	std::istringstream lineBuffer;
	
	double x, y, z;
	size_t frameId;
	
	unsigned int numFrames;
	unsigned int currentLine = 0;
	
	std::string line;
	
	// step over first line
	std::getline( fileIn, line );
	while ( std::getline( fileIn, line ) ){
		currentLine++;

		lineBuffer.clear();
		lineBuffer.str( line );
		
		lineBuffer >> x; lineBuffer >> y; lineBuffer >> z;
		points3d.push_back( Point3d( x, y, z ) );
		
		lineBuffer >> numFrames;
		std::cout << numFrames << std::endl;		
		while (numFrames--) {
			lineBuffer >> frameId;
			lineBuffer >> x; lineBuffer >> y;
			
			points3d.back().observations.push_back( Eigen::Vector2d( x, y ) );
			points3d.back().frameIds.push_back( frameId );
		}
	}	
	
	fileIn.close();	
}

void printLoadedData( Eigen::Matrix3d & K, 
					  std::vector<CameraTransform> & cameras, 
					  std::vector<Point3d> & points3d )
{
	std::cout << "Intrinsics:\n" << K << std::endl;
	
	for(unsigned int i = 0; i < cameras.size(); i++){
		std::cout << "Camera " << i << std::endl;
		std::cout << "\tOrientation: " 
		<< cameras[ i ].orientation.w() << ", " 
		<< cameras[ i ].orientation.x() << ", "
		<< cameras[ i ].orientation.y() << ", "
		<< cameras[ i ].orientation.z() << std::endl;
		
		std::cout << "\tTranslation" << cameras[ i ].translation( 0 ) << ", " 
									 << cameras[ i ].translation( 1 ) << ", " 
									 << cameras[ i ].translation( 2 ) << std::endl;
	}
	
	std::cout << "Observations for 3D Points:" << std::endl;
	for(unsigned int i = 0; i < points3d.size(); i++){
		std::cout << "3D Point:" << points3d[ i ].P.x() << ", " << points3d[ i ].P.y() << ", " << points3d[ i ].P.z() << std::endl;
		
		for(unsigned int k = 0; k < points3d[ i ].observations.size(); k++){
			std::cout << "\tId: " << points3d[ i ].frameIds[ k ];
			std::cout << "\tp: " << points3d[ i ].observations[ k ].x() << ", " << points3d[ i ].observations[ k ].y() << std::endl;
		}
	}
}

void convertDataToOpenCv(Eigen::Matrix3d & K, 
						 std::vector<CameraTransform> & cameras, 
						 std::vector<Point3d> & points3d,
						 cv::vector<cv::Point3d> & pointsOCV, /* 3d points */
						 cv::vector<cv::vector<cv::Point2d> > & imagePoints, /* projections of every point for every camera */
						 cv::vector<cv::vector<int> > & visibility,/* visibility of every 3d point for every camera */
						 cv::vector<cv::Mat> & intrinsics, /* for each cam*/
						 cv::vector<cv::Mat> & rotations,
						 cv::vector<cv::Mat> & translations,
						 cv::vector<cv::Mat> & distortions )
{
	imagePoints.resize( cameras.size() );
	visibility.resize( cameras.size() );

	for( unsigned int i = 0; i < imagePoints.size(); i++ ) {
		imagePoints[ i ].resize( points3d.size() );
		visibility[ i ].resize( points3d.size(), 0 );
	}
	
	cv::Point3d currentPoint;
	for(unsigned int i = 0; i < points3d.size(); i++){
		currentPoint.x = points3d[ i ].P.x();
		currentPoint.y = points3d[ i ].P.y();
		currentPoint.z = points3d[ i ].P.z();
		pointsOCV.push_back( currentPoint );
		
		for( unsigned int p = 0; p < points3d[i].observations.size(); p++ ){
			imagePoints[ points3d[ i ].frameIds[ p ] ][ p ].x = points3d[ i ].observations[ p ].x();
			imagePoints[ points3d[ i ].frameIds[ p ] ][ p ].y = points3d[ i ].observations[ p ].y();
			
			visibility[ points3d[ i ].frameIds[ p ] ][ p ] = 1; // visible
		}
	}
		
	cv::Mat intr( 3, 3, CV_64F );
	cv::Mat rot( 3, 3, CV_64F );
	cv::Mat trans( 3, 1, CV_64F );
	cv::Mat dist = cv::Mat::zeros( 5, 1, CV_64F );
	
	for(unsigned int r = 0; r < 3; r++)
		for(unsigned int c = 0; c < 3; c++)
			intr.at<double>( r, c ) = K( r, c );
	
	// fill initial camera estimates:
	Eigen::Matrix3d rotEI;
	for(unsigned int i = 0; i < cameras.size(); i++){
		// same intrinsics for all
		intrinsics.push_back( intr );		
		distortions.push_back( dist );
		
		// rotation init:
		rotEI = cameras[ i ].orientation.toRotationMatrix();
		for(unsigned int r = 0; r < 3; r++)
			for(unsigned int c = 0; c < 3; c++)
				rot.at<double>( r, c ) = rotEI( r, c );
		rotations.push_back( rot );
			
		// translation init:
		trans.at<double>( 0, 0 ) = cameras[ i ].translation.x();
		trans.at<double>( 1, 0 ) = cameras[ i ].translation.y();
		trans.at<double>( 2, 0 ) = cameras[ i ].translation.z();
		translations.push_back( trans );
	}
}

void testOpenCVImplementation( Eigen::Matrix3d & K, 
							   std::vector<CameraTransform> & cameras, 
							   std::vector<Point3d> & points3d )
{
	cv::vector<cv::Point3d> pointsOCV;
	cv::vector<cv::vector<cv::Point2d> > imagePoints;
	cv::vector<cv::vector<int> > visibility;
	cv::vector<cv::Mat> intrinsics;
	cv::vector<cv::Mat> rotations;
	cv::vector<cv::Mat> translations;
	cv::vector<cv::Mat> distortions;
	
	convertDataToOpenCv( K, cameras, points3d,
						 pointsOCV, imagePoints, visibility, 
						 intrinsics, rotations, translations, distortions );
	
	cv::TermCriteria criteria( cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 100, DBL_EPSILON );
	
	
	cvt::Timer timer;
		
	std::cout << pointsOCV.size() << std::endl;
	cv::LevMarqSparse::bundleAdjust( pointsOCV,
									 imagePoints,
									 visibility,
									 intrinsics,
									 rotations, 
									 translations,
									 distortions,
									 criteria );
	
	std::cout << "OpenCV BA took: " << timer.elapsedMiliSeconds() << "ms" << std::endl;
	
}

int main(int argc, char* argv[])
{
	cvt::Resources resources;
	
	Eigen::Matrix3d K;
	std::vector<CameraTransform> cameras;
	std::vector<Point3d> points3d;
		
	try {
		std::string intrinsicsFile = resources.find("sba/calib.txt");
				
		std::string camFile = resources.find("sba/7cams.txt");
		//std::string camFile = resources.find("sba/7camsvarK.txt");
		std::string pointFile = resources.find("sba/7pts.txt");
		
		//std::string camFile = resources.find("sba/9cams.txt");
		//std::string camFile = resources.find("sba/9camsvarK.txt");
		//std::string pointFile = resources.find("sba/9pts.txt");
		
		//std::string camFile = resources.find("sba/54cams.txt");
		//std::string camFile = resources.find("sba/54camsvarK.txt");
		//std::string camFile = resources.find("sba/54camsvarKD.txt");
		//std::string pointFile = resources.find("sba/54pts.txt");
		
		parseCalibFile( intrinsicsFile, K );		
		parseCameraFile( camFile, cameras );
		parsePtsFile( pointFile, points3d );
		
		printLoadedData( K, cameras, points3d );
		
		testOpenCVImplementation( K, cameras, points3d );

	}
	catch (const cvt::Exception & e) {
		std::cout << e.what() << std::endl;
	}	
	
	return 0;
}