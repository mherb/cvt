#include <cvt/vision/FaceShape.h>
#include <cvt/io/ImageIO.h>
#include <cvt/gfx/GFXEngineImage.h>
#include <cvt/math/GaussNewton.h>
#include <cvt/io/ImageSequence.h>

using namespace cvt;

int main()
{
	Image imgu, imgf;
	int bla;

	ImageSequence seq( "./imgseq/image_", "cvtraw", 30, 1016, 5 );

//	ImageIO::loadPNG( img, "/home/heise/Pictures/myface2.png" );

	imgu.reallocate( seq.width(), seq.height(), IFormat::BGRA_UINT8 );
	imgf.reallocate( seq.width(), seq.height(), IFormat::GRAY_FLOAT );


	FaceShape<float> fs;
	fs.setTransform( 55, 0, 295, 260 );

	do {
		seq.nextFrame();
		seq.frame().convert( imgu );
		imgu.convert( imgf );

		fs.updateInput( &imgf );

		TerminationCriteria<float>	termCrit( TERM_MAX_ITER | TERM_COSTS_THRESH );
		termCrit.setCostThreshold( 0.5f );
		termCrit.setMaxIterations( 100 );
		GaussNewton<float>	gn;
		SquaredDistance<float, float> costFunc;

		gn.optimize( fs, costFunc, termCrit );
		std::cout << gn.iterations() << std::endl;
		std::cout << gn.costs() << std::endl;
		/*  std::cout << fs.transform() << std::endl;
		  std::cout << fs.weights() << std::endl;*/

		{
			GFXEngineImage ge( imgu );
			GFX g( &ge );
			g.color().set( 0.0f, 1.0f, 0.0f, 1.0f );
			fs.drawCurrent( &g );
			ImageIO::savePNG( imgu, "faceshapeopt.png" );
		}
//		usleep( 500 );
		bla = getchar();
	} while( 1 );

}
