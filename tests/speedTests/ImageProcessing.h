#ifndef TEST_IMG_PROC_H
#define TEST_IMG_PROC_H

#include <cvt/gfx/Image.h>
#include <cvt/gfx/IKernel.h>
#include <cvt/util/Time.h>
#include <cvt/io/Resources.h>

#include <cvt/vision/IntegralImage.h>

#include <opencv/cv.h>
#include <opencv/cv.hpp>
#include <opencv/highgui.h>

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
    
    cv::Mat ocvLena = cv::imread( resources.find( "lena_g.png" ), 0 );
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
	cv::Mat img = cv::imread( resources.find( "lena.png" ) );
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
	cv::Mat img = cv::imread( resources.find( "lena_g.png" ), 0 );
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

void testImageProcessing()
{
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
