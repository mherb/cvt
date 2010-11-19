#include <iostream>
#include <cv.h>
#include <highgui.h>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/Image.h>
#include <cvt/io/DC1394Camera.h>
//#include <cvt/io/V4L2Camera.h>
#include <cvt/io/ImageIO.h>
#include <cvt/util/Time.h>
#include <cvt/util/Exception.h>
#include <cvt/math/ESM.h>

//#define CAMINPUT 1

//#define FILETEMPLATE

using namespace cvt;

struct Params {		
	size_t numClick;
	int x0, y0, x1, y1;
};

void mouseevent(int event, int x, int y, int flags, Params* p )
{
	if(event == CV_EVENT_LBUTTONUP){
		std::cout << " Click: " << x << ", " << y << " numClick: " << p->numClick << std::endl;
		switch ( p->numClick ) {
			case 0:
				// first point
				p->x0 = x; 
				p->y0 = y;
				p->numClick++;
				break;
			case 1:
				// second point				
				if( x < p->x0 ){
					p->x1 = p->x0;
					p->x0 = x;
				} else {
					p->x1 = x;
				}
				if( y < p->y0 ){
					p->y1 = p->y0;
					p->y0 = y;
				} else {
					p->y1 = y;
				}
				p->numClick++;		
				break;
			default:
				break;
		}
	}
}

void drawEstimate( const Image& temp, Image & out, SL3Transform & hom )
{
	CvPoint p0, p1, p2, p3;
	Eigen::Vector3d pHom, pPrime;
	
	pHom[ 0 ] = 0; pHom[ 1 ] = 0; pHom[ 2 ] = 1.0;
	pPrime = hom.matrix() * pHom;
	p0.x = (int) ( pPrime[ 0 ] / pPrime[ 2 ] );
	p0.y = (int) ( pPrime[ 1 ] / pPrime[ 2 ] );
	
	pHom[ 0 ] = temp.width()-1; pHom[ 1 ] = 0; pHom[ 2 ] = 1.0;
	pPrime = hom.matrix() * pHom;
	p1.x = (int)( pPrime[ 0 ] / pPrime[ 2 ] );
	p1.y = (int)( pPrime[ 1 ] / pPrime[ 2 ] );
	
	pHom[ 0 ] = temp.width()-1; pHom[ 1 ] = temp.height()-1; pHom[ 2 ] = 1.0;
	pPrime = hom.matrix() * pHom;
	p2.x = (int)( pPrime[ 0 ] / pPrime[ 2 ] );
	p2.y = (int)( pPrime[ 1 ] / pPrime[ 2 ] );
	
	pHom[ 0 ] = 0.0; pHom[ 1 ] = temp.height()-1; pHom[ 2 ] = 1.0;
	pPrime = hom.matrix() * pHom;
	p3.x = (int)( pPrime[ 0 ] / pPrime[ 2 ] );
	p3.y = (int)( pPrime[ 1 ] / pPrime[ 2 ] );
	
//	std::cout << "P: " << p0.x << ", " << p0.y << std::endl;
//	std::cout << "P: " << p1.x << ", " << p1.y << std::endl;
//	std::cout << "P: " << p2.x << ", " << p2.y << std::endl;
//	std::cout << "P: " << p3.x << ", " << p3.y << std::endl;
	
//	cvLine( out.iplimage(), p0, p1, CV_RGB(255, 255, 255), 2 );
//	cvLine( out.iplimage(), p1, p2, CV_RGB(255, 255, 255), 2 );
//	cvLine( out.iplimage(), p2, p3, CV_RGB(255, 255, 255), 2 );
//	cvLine( out.iplimage(), p0, p3, CV_RGB(255, 255, 255), 2 );
}



int main(int argc, char* argv[])
{
	const Image* frame;
	
#ifdef CAMINPUT
	DC1394Camera cam;
#endif

	int key;
	size_t frames = 0;
	Time timer;	
	
	try {
		
#ifdef CAMINPUT
		cam.open();
		cam.init();
		cam.captureStart();		
		
		Image out( 640, 480, IOrder::GRAY, IType::UBYTE );
		Image outF( 640, 480, IOrder::GRAY, IType::FLOAT );
#else
		std::string dataFolder(DATA_FOLDER);
		std::string inputFile(dataFolder + "/lena_g.png");
	
		// RGBA UBYTE IMAGE
		cvt::Image out;		
		cvt::ImageIO::loadPNG(out, inputFile);
		Image outF( out.width(), out.height(), IFormat::GRAY_FLOAT );
		
		out.convert( outF );
#endif
		
		/* the template */
		Image * temp = 0;

#ifndef FILETEMPLATE		
		Params p = { 0, 0, 0, 0, 0 };
//		cvShowImage( "ESM", out.iplimage() );
		cvSetMouseCallback( "ESM", ( CvMouseCallback ) mouseevent, &p );
		
		timer.reset();
		
		bool selectPatch = true;
		
		while( selectPatch ) {
#ifdef CAMINPUT
			cam.captureNext();
			frame = cam.image();

			frame->debayer( out, IBAYER_RGGB );
//			out.copy( *frame );

//			cvShowImage( "ESM", out.iplimage() );
#endif
			out.convert( outF );

			if( selectPatch ){
				if( p.numClick == 1 ){
//					cvCircle( out.iplimage(), cvPoint( p.x0, p.y0), 2, CV_RGB( 255, 255, 255 ), 2, CV_FILLED );
				} else if( p.numClick == 2 ){
					Recti roi( p.x0, p.y0, p.x1-p.x0, p.y1-p.y0 );
					temp = new Image( outF, &roi );
					selectPatch = false;
				}
			}

			key = cvWaitKey( 10 ) & 0xff;
			if( key == 27 )
				break;
		}
		if( selectPatch )
			return 0;
#else
		Image filetemplate;
		ImageIO::loadPNG(filetemplate,"/home/heise/Pictures/PRML.png");
		temp = new Image( filetemplate.width(), filetemplate.height(), IOrder::GRAY, IType::FLOAT );
		filetemplate.convert( *temp );
#endif
		
		ESM esm( *temp, 20, 1e-6 );
		
		
		SL3Transform homography;
		Eigen::Matrix<double, 8, 1> params = Eigen::Matrix<double, 8, 1>::Zero();
#ifndef FILETEMPLATE
		params[ 0 ] = p.x0;
		params[ 1 ] = p.y0;
#else
		params[ 0 ] = 150.0;
		params[ 1 ] = 250.0;

#endif
		homography.update( params );
		
//		std::cout << "Homography: " << homography.matrix() << std::endl;
//		std::cout << "Det: " << homography.matrix().determinant() << std::endl;
						
		drawEstimate( *temp, outF, homography );
//		cvShowImage( "ESM", outF.iplimage() );				
//		cvShowImage( "Template", temp->iplimage() );				
		cvWaitKey( 0 );
				
		while( 1 ) {
#ifdef CAMINPUT
			cam.captureNext();
			frame = cam.image();			
			frame->debayer( out, IBAYER_RGGB );
#endif
			out.convert( outF );
			
			esm.optimize( homography, outF );
			
			drawEstimate( *temp, outF, homography );	
//			cvShowImage( "ESM", outF.iplimage() );
			
			key = ( cvWaitKey( 5 ) & 0xff );
			if( key == 27 )
				break;
			
			frames++;
			if( timer.elapsedSeconds() > 5.0f ) {
				std::cout << "FPS: " << ( double ) frames / timer.elapsedSeconds() << std::endl;
				frames = 0;
				timer.reset();
			}
		}
		
		
	} catch( cvt::Exception e ) {
		std::cout << e.what() << std::endl;
	}
	
	return 0;
}
