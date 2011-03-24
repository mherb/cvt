#include <cvt/vision/FaceShape.h>
#include <cvt/io/ImageIO.h>
#include <cvt/gfx/GFXEngineImage.h>
#include <cvt/math/GaussNewton.h>
#include <cvt/io/ImageSequence.h>
#include <cvt/gfx/GFX.h>

using namespace cvt;

//#define GTLINEINPUT 1

int main()
{
	Image imgu, imgf;
	int bla;

	ImageSequence seq( "./imgseq/image_", "cvtraw", 30, 1016, 5 );

//	ImageIO::loadPNG( img, "/home/heise/Pictures/myface2.png" );

	imgu.reallocate( seq.width(), seq.height(), IFormat::BGRA_UINT8 );
	imgf.reallocate( seq.width(), seq.height(), IFormat::GRAY_FLOAT );

//	srand( time( NULL ) );

	FaceShape<float> fs;
#ifndef GTLINEINPUT
	fs.setTransform( 35, 0, 265, 205 );
#else
	fs.setTransform( 50, 0, 310, 195 );
#endif

	do {
#ifndef GTLINEINPUT
		seq.nextFrame();
		seq.frame().convert( imgu );
		imgu.convert( imgf );

		fs.updateInput( &imgf );
#else
		fs.resetWeights();
		Image input( 640, 480, IFormat::GRAY_FLOAT );
		{
			GFXEngineImage ge( input );
			GFX g( &ge );
			g.color().set( 0.0f, 0.0f, 0.0f, 1.0f );
			g.fillRect( 0, 0, 640, 480 );
			g.color().set( 1.0f, 1.0f, 1.0f, 1.0f );
			Eigen::Matrix<float, Eigen::Dynamic, 1> w = fs.weights();
			w.setZero();
			for( int i = 0; i < 5; i++ )
				w[ i ] = Math::rand( -0.5f, 0.5f );

			std::cout << std::endl << w << std::endl;
			Matrix3f t;
			t.identity();
			t[ 0 ][ 0 ] = 50.0f;
			t[ 1 ][ 1 ] = 50.0f;
			t[ 0 ][ 2 ] = 300.0f;
			t[ 1 ][ 2 ] = 200.0f;
			fs.draw( &g, t, w );
		}

		ImageIO::savePNG( input, "gt.png" );
		fs.updateInput( &input );
#endif

		TerminationCriteria<float>	termCrit( TERM_MAX_ITER | TERM_COSTS_THRESH );
		termCrit.setCostThreshold( 0.4f );
		termCrit.setMaxIterations( 25 );
		GaussNewton<float>	gn;
		SquaredDistance<float, float> costFunc;

		gn.optimize( fs, costFunc, termCrit );
		std::cout << gn.iterations() << std::endl;
		std::cout << gn.costs() << std::endl;
		/*  std::cout << fs.transform() << std::endl;*/
		  std::cout << std::endl << fs.weights() << std::endl;

		{
#ifndef GTLINEINPUT
			GFXEngineImage ge( imgu );
#else
			GFXEngineImage ge( input );
#endif
			GFX g( &ge );
			g.color().set( 0.0f, 1.0f, 0.0f, 1.0f );
			fs.drawCurrent( &g );
#ifndef GTLINEINPUT
			ImageIO::savePNG( imgu, "faceshapeopt.png" );
#else
			ImageIO::savePNG( input, "faceshapeopt.png" );
#endif
		}
//		usleep( 500 );
		bla = getchar();
	} while( 1 );

}
