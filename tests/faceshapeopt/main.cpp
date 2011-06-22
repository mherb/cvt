#include <cvt/vision/FaceShape.h>
#include <cvt/gfx/GFXEngineImage.h>
#include <cvt/math/GaussNewton.h>
#include <cvt/io/ImageSequence.h>
#include <cvt/gfx/GFX.h>

using namespace cvt;

void drawCurrent( const Image& _img, const FaceShape<double>& fs )
{
	Image img( _img.width(), _img.height(), IFormat::RGBA_FLOAT );
	_img.convert( img );
	GFXEngineImage ge( img );
	GFX g( &ge );
	g.color().set( 0.0f, 1.0f, 0.0f, 1.0f );
	fs.drawCurrent( &g );
    img.save( "current.png" );
	int	bla = getchar();
}


int main()
{
	Image imgu, imgf;
	int bla;

	ImageSequence seq( "./imgseq/image_", "cvtraw", 30, 1016, 5 );

	imgu.reallocate( seq.width(), seq.height(), IFormat::BGRA_UINT8 );
	imgf.reallocate( seq.width(), seq.height(), IFormat::GRAY_FLOAT );

//	srand( time( NULL ) );

	FaceShape<double> fs;
#ifndef GTLINEINPUT
	fs.setTransform( 35, 0, 265, 205 );
#else
	fs.setTransform( 50, 0, 310, 195 );
#endif

	TerminationCriteria<double>	termCrit( TERM_MAX_ITER );
	termCrit.setCostThreshold( 0.01f );
	termCrit.setMaxIterations( 25 );
	GaussNewton<double>	gn;
	RobustHuber<double, double> costFunc( 2.5 );

	Matrix3<double> matdown;
	Matrix3<double> matup;

#define DOWNFACTOR 0.8
#define UPFACTOR ( 1.0 / ( DOWNFACTOR ) )

	matdown.identity();
	matdown[ 0 ][ 0 ] = DOWNFACTOR;
	matdown[ 1 ][ 1 ] = DOWNFACTOR;
	matdown[ 2 ][ 2 ] = 1.0;

	matup.identity();
	matup[ 0 ][ 0 ] = UPFACTOR;
	matup[ 1 ][ 1 ] = UPFACTOR;
	matup[ 2 ][ 2 ] = 1.0;


	do {
#ifndef GTLINEINPUT
		seq.nextFrame();
		seq.frame().convert( imgu );
		imgu.convert( imgf );

//		fs.updateInput( &imgf );
		{
			Image tmp;
			IScaleFilterGauss scaler;

			imgf.scale( tmp, imgf.width() * DOWNFACTOR * DOWNFACTOR, imgf.height() * DOWNFACTOR * DOWNFACTOR, scaler );
			fs.setTransform( matdown * matdown * fs.transform()  );
			fs.updateInput( &tmp );
			gn.optimize( fs, costFunc, termCrit );
//			drawCurrent( tmp, fs );
		std::cout << gn.iterations() << std::endl;
		std::cout << gn.costs() << std::endl;


			imgf.scale( tmp, imgf.width() * DOWNFACTOR, imgf.height() * DOWNFACTOR, scaler );
			fs.setTransform( matup * fs.transform()  );
			fs.updateInput( &tmp );
			gn.optimize( fs, costFunc, termCrit );
//			drawCurrent( tmp, fs );
		std::cout << gn.iterations() << std::endl;
		std::cout << gn.costs() << std::endl;


			fs.setTransform( matup * fs.transform() );
			fs.updateInput( &imgf );
			gn.optimize( fs, costFunc, termCrit );
//			drawCurrent( imgf, fs );
		std::cout << gn.iterations() << std::endl;
		std::cout << gn.costs() << std::endl;

		}
#else
		fs.resetWeights();
		Image input( 640, 480, IFormat::GRAY_FLOAT );
		{
			GFXEngineImage ge( input );
			GFX g( &ge );
			g.color().set( 0.0f, 0.0f, 0.0f, 1.0f );
			g.fillRect( 0, 0, 640, 480 );
			g.color().set( 1.0f, 1.0f, 1.0f, 1.0f );
			Eigen::Matrix<double, Eigen::Dynamic, 1> w = fs.weights();
			w.setZero();
			for( int i = 0; i < 5; i++ )
				w[ i ] = Math::rand( -0.25f, 0.25f );

			fs.setTransform( 50, 0, 310, 195 );

//			std::cout << std::endl << w << std::endl;
			Matrix3<double> t;
			t.identity();
			t[ 0 ][ 0 ] = 50.0f + Math::rand( -1.0f, 1.0f );
			t[ 1 ][ 1 ] = 50.0f + Math::rand( -1.0f, 1.0f );
			t[ 0 ][ 1 ] = Math::rand( -2.5f, 2.5f );
			t[ 1 ][ 0 ] = -t[ 0 ][ 1 ];
			t[ 0 ][ 2 ] = 300.0f;
			t[ 1 ][ 2 ] = 200.0f;
			fs.draw( &g, t, w );
		}

		ImageIO::savePNG( input, "gt.png" );
		fs.updateInput( &input );
		gn.optimize( fs, costFunc, termCrit );
#endif

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
			static int x = 0;
			char buf[ 255 ];
			sprintf( buf, "out/faceshapeopt%05d.png", x++ );
			if( x == 1000 )
				return 0;

			imgu.save( buf );
#else
			input.save( "faceshapeopt.png" );
#endif
		}
//		usleep( 500 );
//		bla = getchar();
	} while( 1 );

}
