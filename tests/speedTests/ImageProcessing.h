#ifndef TEST_IMG_PROC_H
#define TEST_IMG_PROC_H

#include <cvt/gfx/Image.h>
#include <cvt/gfx/IKernel.h>
#include <cvt/util/Time.h>
#include <cvt/util/String.h>
#include <cvt/io/Resources.h>

#include <cvt/vision/IntegralImage.h>
#include <cvt/vision/FAST.h>
#include <cvt/vision/ORB.h>
#include <cvt/vision/Feature2D.h>

#include <opencv2/opencv.hpp>

#include <iomanip>

#define NUMSAMPLES ( 100 )

using namespace cvt;

void testIntegralImage()
{
    Resources resources;
	Image lena( resources.find( "lena_g.png" ) );

    Image ii, iiSqr;
    Time t; t.reset();
    for( size_t i = 0; i < NUMSAMPLES; i++ ){
        lena.integralImage( ii );
        lena.squaredIntegralImage( iiSqr );
    }
    std::cout << "CVT IntegralImage: " << t.elapsedMicroSeconds() / NUMSAMPLES << " microSecs"<< std::endl;

    cv::Mat ocvLena = cv::imread( resources.find( "lena_g.png" ).c_str(), 0 );
    cv::Mat intImage( ocvLena.rows, ocvLena.cols, CV_32FC1 );
    cv::Mat intSqrImage( ocvLena.rows, ocvLena.cols, CV_64FC1 );

    t.reset();
    for( size_t i = 0; i < NUMSAMPLES; i++)
        cv::integral( ocvLena, intImage, intSqrImage );
    std::cout << "OCV IntegralImage: " << t.elapsedMicroSeconds() / NUMSAMPLES << " microSecs" << std::endl;
}

void testConvolutionCVT()
{
	Resources resources;
	Image lena( resources.find( "lena.png" ) );
	Image outx( lena );
	Image outy( lena );

	IKernel kx( IKernel::GAUSS_HORIZONTAL_3 );
	IKernel ky( IKernel::GAUSS_VERTICAL_3 );
	Time t;

	std::cout << "Running Convolution Tests" << std::endl;
	t.reset();
	for( size_t i = 0; i <  NUMSAMPLES; i++ ){
		lena.convolve( outx, IKernel::GAUSS_HORIZONTAL_3, IKernel::GAUSS_VERTICAL_3 );
	}
	std::cout << "CVT:\tGauss_3x3\t-> avg. " << t.elapsedMilliSeconds() / NUMSAMPLES << "ms" << std::endl;

	kx = IKernel::GAUSS_HORIZONTAL_5;
	ky = IKernel::GAUSS_VERTICAL_5;
	t.reset();
	for( size_t i = 0; i <  NUMSAMPLES; i++ ){
		lena.convolve( outx, kx, ky );
	}
	std::cout << "CVT:\tGauss_5x5\t-> avg. " << t.elapsedMilliSeconds() / NUMSAMPLES << "ms" << std::endl;

	kx = IKernel::GAUSS_HORIZONTAL_7;
	ky = IKernel::GAUSS_VERTICAL_7;
	t.reset();
	for( size_t i = 0; i <  NUMSAMPLES; i++ ){
		lena.convolve( outx, kx, ky );
	}
	std::cout << "CVT:\tGauss_7x7\t-> avg. " << t.elapsedMilliSeconds() / NUMSAMPLES << "ms" << std::endl;

	kx = IKernel::LAPLACE_33;
	t.reset();
	for( size_t i = 0; i <  NUMSAMPLES; i++ ){
		lena.convolve( outx, kx );
	}
	std::cout << "CVT:\tLaplace_3x3\t-> avg. " << t.elapsedMilliSeconds() / NUMSAMPLES << "ms" << std::endl;
}

void testConvolutionGRAYCVT()
{
	Resources resources;
	Image lena( resources.find( "lena_g.png" ) );
	Image outx( lena );
	Image outy( lena );

	IKernel kx( IKernel::GAUSS_HORIZONTAL_3 );
	IKernel ky( IKernel::GAUSS_VERTICAL_3 );
	Time t;

	std::cout << "Running Convolution Tests" << std::endl;
	t.reset();
	for( size_t i = 0; i <  NUMSAMPLES; i++ ){
		lena.convolve( outx, IKernel::GAUSS_HORIZONTAL_3, IKernel::GAUSS_VERTICAL_3 );
	}
	std::cout << "CVT:\tgray Gauss_3x3\t-> avg. " << t.elapsedMilliSeconds() / NUMSAMPLES << "ms" << std::endl;

	kx = IKernel::GAUSS_HORIZONTAL_5;
	ky = IKernel::GAUSS_VERTICAL_5;
	t.reset();
	for( size_t i = 0; i <  NUMSAMPLES; i++ ){
		lena.convolve( outx, kx, ky );
	}
	std::cout << "CVT:\tgray Gauss_5x5\t-> avg. " << t.elapsedMilliSeconds() / NUMSAMPLES << "ms" << std::endl;

	kx = IKernel::GAUSS_HORIZONTAL_7;
	ky = IKernel::GAUSS_VERTICAL_7;
	t.reset();
	for( size_t i = 0; i <  NUMSAMPLES; i++ ){
		lena.convolve( outx, kx, ky );
	}
	std::cout << "CVT:\tgray Gauss_7x7\t-> avg. " << t.elapsedMilliSeconds() / NUMSAMPLES << "ms" << std::endl;

	kx = IKernel::LAPLACE_33;
	t.reset();
	for( size_t i = 0; i <  NUMSAMPLES; i++ ){
		lena.convolve( outx, kx );
	}
	std::cout << "CVT:\tgray Laplace_3x3\t-> avg. " << t.elapsedMilliSeconds() / NUMSAMPLES << "ms" << std::endl;
}

void testConvolutionOCV()
{
	Resources resources;
	cv::Mat img = cv::imread( resources.find( "lena.png" ).c_str() );
	cv::Mat out = img;
	cv::Mat kernel;

	Time t;

	kernel = cv::getGaussianKernel( 3, -1, CV_32F );
	t.reset();
	for( size_t i = 0; i <  NUMSAMPLES; i++ ){
		cv::filter2D( img, out, -1, kernel );
	}
	std::cout << "OCV:\tGauss_3x3\t-> avg. " << t.elapsedMilliSeconds() / NUMSAMPLES << "ms" << std::endl;

	kernel = cv::getGaussianKernel( 5, -1, CV_32F );
	t.reset();
	for( size_t i = 0; i <  NUMSAMPLES; i++ ){
		cv::filter2D( img, out, -1, kernel );
	}
	std::cout << "OCV:\tGauss_5x5\t-> avg. " << t.elapsedMilliSeconds() / NUMSAMPLES << "ms" << std::endl;

	kernel = cv::getGaussianKernel( 7, -1, CV_32F );
	t.reset();
	for( size_t i = 0; i <  NUMSAMPLES; i++ ){
		cv::filter2D( img, out, -1, kernel );
	}
	std::cout << "OCV:\tGauss_7x7\t-> avg. " << t.elapsedMilliSeconds() / NUMSAMPLES << "ms" << std::endl;

	t.reset();
	for( size_t i = 0; i <  NUMSAMPLES; i++ ){
		cv::Laplacian( img, out, CV_8U );
	}
	std::cout << "OCV:\tLaplace_3x3\t-> avg. " << t.elapsedMilliSeconds() / NUMSAMPLES << "ms" << std::endl;

}

void testConvolutionGRAYOCV()
{
	Resources resources;
	cv::Mat img = cv::imread( resources.find( "lena_g.png" ).c_str(), 0 );
	cv::Mat out = img;
	cv::Mat kernel;

	Time t;

	kernel = cv::getGaussianKernel( 3, -1, CV_32F );
	t.reset();
	for( size_t i = 0; i <  NUMSAMPLES; i++ ){
		cv::filter2D( img, out, -1, kernel );
	}
	std::cout << "OCV:\tgray Gauss_3x3\t-> avg. " << t.elapsedMilliSeconds() / NUMSAMPLES << "ms" << std::endl;

	kernel = cv::getGaussianKernel( 5, -1, CV_32F );
	t.reset();
	for( size_t i = 0; i <  NUMSAMPLES; i++ ){
		cv::filter2D( img, out, -1, kernel );
	}
	std::cout << "OCV:\tgray Gauss_5x5\t-> avg. " << t.elapsedMilliSeconds() / NUMSAMPLES << "ms" << std::endl;

	kernel = cv::getGaussianKernel( 7, -1, CV_32F );
	t.reset();
	for( size_t i = 0; i <  NUMSAMPLES; i++ ){
		cv::filter2D( img, out, -1, kernel );
	}
	std::cout << "OCV:\tgray Gauss_7x7\t-> avg. " << t.elapsedMilliSeconds() / NUMSAMPLES << "ms" << std::endl;

	t.reset();
	for( size_t i = 0; i <  NUMSAMPLES; i++ ){
		cv::Laplacian( img, out, CV_8U );
	}
	std::cout << "OCV:\tgray Laplace_3x3\t-> avg. " << t.elapsedMilliSeconds() / NUMSAMPLES << "ms" << std::endl;

}

void testFAST9()
{
    Resources r;

    String file = r.find( "features_dataset/bark/img1.png" );


    cv::vector<cv::KeyPoint> kp;
    cv::Mat img = cv::imread( file.c_str(), 0 );
    cv::FAST( img, kp, 35, false );

    std::cout << "OCV" << std::endl;
    for( size_t i = 0; i < kp.size(); i++ )
        std::cout << kp[ i ].pt.x << " " << kp[ i ].pt.y << std::endl;


    std::cout << std::endl << "CVT:" << std::endl;
    std::vector<Feature2Df> ft;
    VectorFeature2DInserter<float> inserter( ft );
    Image cvtImg( file );
    FAST::detect9( cvtImg, 35, inserter );

    for( size_t i = 0; i < ft.size(); i++ )
        std::cout << ft[ i ].pt.x << " " << ft[ i ].pt.y << std::endl;

}

bool compareKP( const cv::KeyPoint & kp0, const cv::KeyPoint & kp1 )
{
    return kp0.response > kp1.response;
}

void testORB()
{
    Resources r;

    String file = r.find( "features_dataset/bark/img1.png" );

    cv::vector<cv::KeyPoint> kp;
    cv::Mat img = cv::imread( file.c_str(), 0 );

    float scaleFactor = 1.4;
    size_t nLevels = 1;
    uint8_t cornerThresh = 20;

    cv::ORB ocvORB( 1000, scaleFactor, nLevels, cornerThresh );

    cv::Mat mask, descriptors;

    Time t;
    ocvORB( img, mask, kp, descriptors );
    std::sort( kp.begin(), kp.end(), compareKP );
    std::cout << "OpenCV Time: " << t.elapsedMilliSeconds() << std::endl;

    std::ofstream out;
    out.open( "opencv_orb.txt" );

    std::cout << "OCV" << std::endl;

    for( size_t i = 0; i < kp.size(); i++ ){
        out << "( " << kp[ i ].pt.x << " , " << kp[ i ].pt.y << " ) Orientation: " << kp[ i ].angle;
        out << " Score: " << kp[ i ].response;
		out << "\nDescriptor: 0x";
		for( int c = 0; c < 32; c++ )
			out << std::hex << std::setfill( '0' ) << (int)descriptors.at<uint8_t>( i, c );
		out << std::endl;
        out << std::endl;
    }

    out.close();

    Image cvtImg( file );

    t.reset();

    ORB orb( cvtImg, nLevels, 1.0 / scaleFactor, cornerThresh, true );
    std::cout << "CVT Time: " << t.elapsedMilliSeconds() << std::endl;

    out.open( "cvt_orb.txt" );
    for( size_t i = 0; i < orb.size(); i++ )
        out << orb[ i ] << std::endl;
    out.close();
}

void testImageProcessing()
{
    std::cout << "ORB" << std::endl;
    testORB();

    return;

    std::cout << "FAST Corners" << std::endl;
    testFAST9();

	std::cout << "**************** CVT ****************" << std::endl;
	testConvolutionCVT();
	testConvolutionGRAYCVT();
	std::cout << "**************** OCV ****************" << std::endl;
	testConvolutionOCV();
	testConvolutionGRAYOCV();

    std::cout << "IntegralImage" << std::endl;
    testIntegralImage();
}


#endif
