#include <cv.h>
#include <cvaux.hpp>
#include <highgui.h>

#include <vector>
#include <iostream>

#include <cvt/util/Time.h>

void testKLT()
{	
	
	CvCapture * cam = cvCaptureFromCAM( 0 );
	
	cvSetCaptureProperty( cam, CV_CAP_PROP_FRAME_WIDTH, 640.0 );
	cvSetCaptureProperty( cam, CV_CAP_PROP_FRAME_HEIGHT, 480.0 );
	
	IplImage * ipl = 0;
	
	cvt::Time timer;
	
	int threshold = 40;
	double nonMaxSuppr = true;
	
	cv::namedWindow( "Window" );
		
	std::vector<cv::Point2f> set1, set2;
	std::vector<cv::Point2f> & trackedPoints = set1;
	std::vector<cv::Point2f> & tmpPoints = set2;
	std::vector<cv::Point2f> & tmpSet = set2;
	
	std::vector<cv::Point2f> nextPts;
	std::vector<uchar> status;
	std::vector<float> error;
	size_t frame = 0;
	
	bool init = true;
	cv::Mat prev;
	
	while (true) {
		ipl = cvQueryFrame( cam );
		frame++;
		
		cv::Mat img( ipl );
		cv::Mat imgGray;
		
		cv::cvtColor( img, imgGray, CV_BGR2GRAY );
		
		if( !init ){	
			cv::Size size(20, 20);
			int maxLevels = 1;			
			cv::TermCriteria criteria( cv::TermCriteria::COUNT+cv::TermCriteria::EPS, 30, 0.01);
			double derivLambda = 1.0;
			int flags = 0;			
			cv::calcOpticalFlowPyrLK(prev, 
									 img, 
									 trackedPoints, 
									 nextPts, 
									 status, 
									 error, 
									 size, 
									 maxLevels, 
									 criteria, 
									 derivLambda,
									 flags);
			
			for( unsigned int i = 0; i < nextPts.size(); i++ ){
				//std::cout << "Error: " << error[i] << " status: " << int(status[ i ]) << std::endl;
				if( error[ i ] < 0.2 ){
					tmpPoints.push_back( nextPts[i] );
					std::cout << "Point " << i << " survived " << std::endl;
				}
			}
			
			tmpSet = trackedPoints;
			trackedPoints = tmpPoints;
			tmpPoints = tmpSet;
			tmpPoints.clear();
						
			nextPts.clear();
			status.clear();
			error.clear();
		}
		
		if( trackedPoints.size() < 20 ){
			std::cout << "detect new points" << std::endl;
			std::vector<cv::KeyPoint> keypoints;			
			cv::FAST( imgGray, keypoints, threshold, nonMaxSuppr );
			
			for( size_t i = 0; i < keypoints.size(); i++ ){
				trackedPoints.push_back( keypoints[ i ].pt );
			}
			
			init = false;
		}	
		
		prev = img;
		
		for( size_t i = 0; i < trackedPoints.size(); i++ ){
			cv::circle( img, trackedPoints[ i ], 2, CV_RGB( 255, 0, 0 ), 1 );
		}

		cv::imshow("Window", img );
		
		if( frame == 90 ){
			std::cout << "FPS: " << 90.0 / timer.elapsedSeconds() << std::endl;
			timer.reset();
			frame = 0;
		}
		
		int k = cv::waitKey( 5 );		
		char key = ( k & 0xFF );		
		
		if( key == 27 )
			break;				
	}

	cvReleaseCapture( &cam );	
}

void edgeTests()
{
	std::string input( "/Users/sebi/Documents/keynotes+powerpoints/iros2010/cam1.jpg" );
	
	cv::Mat in  = cv::imread( input );
	cv::Mat gray;
	cv::cvtColor( in, gray, CV_RGB2GRAY );
	
	cv::Mat canny( in.rows, in.cols, CV_8UC1 );
	cv::Mat sobelX( in.rows, in.cols, CV_32FC1 );
	cv::Mat sobelY( in.rows, in.cols, CV_32FC1 );
	
	cv::Canny( gray, canny, 30.0, 100.0 );
	cv::imshow( "Edges", canny );
	cv::waitKey( 0 );
	
	cv::Sobel( gray, sobelX, CV_32F, 1, 0 );
	cv::Sobel( gray, sobelY, CV_32F, 0, 1 );
	
	cv::convertScaleAbs( sobelX, gray );
	
	cv::imshow( "Edges", gray );
	cv::waitKey( 0 );
	cv::convertScaleAbs( sobelY, gray );
	cv::imshow( "Edges", gray );
	cv::waitKey( 0 );
	
	cv::imwrite( "canny1.jpg", canny );
	cv::imwrite( "sobelX1.jpg", sobelX );
	cv::imwrite( "sobelY1.jpg", sobelY );
}

int main(int argc, char* argv[])
{
	// for opencv tests ...
	edgeTests();
	//testKLT();
	
	return 0;
}
