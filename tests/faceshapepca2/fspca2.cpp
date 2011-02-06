#include <iostream>
#include <fstream>
#include <vector>
#include <cvt/geom/PointSet.h>
#include <cvt/io/FileSystem.h>
#include <cvt/math/Vector.h>
#include <cvt/math/Matrix.h>
#include <cvt/math/PCA.h>
#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/Slider.h>
#include <Eigen/Array>

using namespace cvt;

#define SAMPLEPTS 76

class FaceShapeWin : public Window
{
	public:
	FaceShapeWin( Eigen::VectorXf& mean, Eigen::MatrixXf& pc ) : Window( "FaceShape" ), _current( SAMPLEPTS * 2 ), _weights( SAMPLEPTS * 2 ), _mean( mean ), _pc( pc )
	{
		_weights.setZero();
		recalc();
	}

	template<int n>
	void setWeight( float val )
	{
		_weights( n * 2 ) = val;
		_weights( n * 2 + 1 ) = val;
		recalc();
		update();
	}

	void paintEvent( PaintEvent* event, GFX* gfx )
	{
		int w, h;
		size( w, h );
		gfx->color().set( 0.6f, 0.6f, 0.6f, 1.0f );
		gfx->fillRect( 0, 0, w, h );


		gfx->color().set( 1.0f, 1.0f, 1.0f, 1.0f );
		gfx->drawLines( &_pts[ 0 ], _pts.size() );
/*		for( int i = 0; i < SAMPLEPTS; i++ ) {
			int x, y;
			char buf[ 200 ];
			x = _current( i * 2 );
			y = _current( i * 2 + 1 );
			sprintf( buf, "%d", i );
			gfx->drawText( x, y, buf );
		}*/

//		gfx->drawIcons( ( const Vector2f* ) &_current(0), _current.rows() / 2, GFX::ICON_CROSS );
		paintChildren( gfx, Recti( 0, 0, w, h ) );
	};

	void recalc()
	{
	   _current = 100.0 * ( _mean + _pc * _weights );
	   _current.cwise() += 250.0f;
	   int map[ SAMPLEPTS ][ 2 ] = {
		   { 0,1 },
		   { 1,2 },
		   { 2,3 },
		   { 3,4 },
		   { 4,5 },
		   { 5,6 },
		   { 6,7 },
		   { 7,8 },
		   { 8,9 },
		   { 9,10},
		   { 10,11},
		   { 11,12},
		   { 12,13},
		   { 13,14},
		   { 14,14},
		   { 15,16},
		   { 16,17},
		   { 17,18},
		   { 18,19},
		   { 19,20},
		   { 20,15},
		   { 21,22},
		   { 22,23},
		   { 23,24},
		   { 24,25},
		   { 25,26},
		   { 26,21},
		   { 27,28},
		   { 28,29},
		   { 29,30},
		   { 30,27},
		   { 31,31},
		   { 32,33},
		   { 33,34},
		   { 34,35},
		   { 35,32},
		   { 36,36},
		   { 37,38},
		   { 38,39},
		   { 39,40},
		   { 40,41},
		   { 41,42},
		   { 42,43},
		   { 43,44},
		   { 44,45},
		   { 45,45},
		   { 46,46},
		   { 47,47},
		   { 48,49},
		   { 49,50},
		   { 50,51},
		   { 51,52},
		   { 52,53},
		   { 53,54},
		   { 54,55},
		   { 55,56},
		   { 56,57},
		   { 57,58},
		   { 58,59},
		   { 59,48},
		   { 48,60},
		   { 60,61},
		   { 61,62},
		   { 62,54},
		   { 54,63},
		   { 63,64},
		   { 64,65},
		   { 65,48},
	   };
		_pts.clear();
		for( int i = 0; i < SAMPLEPTS; i++ ) {
			int a = map[ i ][ 0 ];
			int b = map[ i ][ 1 ];
			if( a == b )
				continue;
			_pts.push_back( Vector2f( _current( a * 2 ), _current( a * 2 + 1 ) ) );
			_pts.push_back( Vector2f( _current( b * 2 ), _current( b * 2 + 1 ) ) );
		}
	}

	private:
		std::vector<Vector2f> _pts;
		Eigen::VectorXf _current;
		Eigen::VectorXf _weights;
		Eigen::VectorXf _mean;
		Eigen::MatrixXf _pc;
};

int main( int argc, char** argv )
{
	if( argc != 2 ) {
		std::cerr << "usage: " << argv[ 0 ] << " csv-file" << std::endl;
		return 1;
	}

	std::vector<PointSet2f> allpts;

	std::ifstream file;
	file.open( argv[ 1 ] );
	while( !file.eof() ) {
		size_t n = SAMPLEPTS;
		Vector2f pt;
		char buf[ 200 ];
		PointSet2f pts;
		std::string str;
		bool discard = false;

		file.getline( buf, 200, ',' );
		file.getline( buf, 200, ',' );

		while( n-- ) {
			file.getline( buf, 200, ',' );
			pt.x = atof( buf );
			file.getline( buf, 200, ',' );
			pt.y = atof( buf );
			pts.add( pt );
			if( pt.x == 0.0f || pt.y == 0.0f ) {
				discard = true;
				break;
			}
		}
		std::getline( file, str );
		if( !discard ) {
			pts.normalize();
			allpts.push_back( pts );
		}
	}
	file.close();

	std::cout << "Read " << allpts.size() << " entries" << std::endl;

	/* align stuff */

#define INITMEAN 50
	PointSet2f meanshape( allpts[ INITMEAN ] );

	for( size_t i = 0; i < allpts.size(); i++ ) {
		if( i == INITMEAN )
			continue;
		Matrix3f sim = allpts[ i ].alignSimilarity( allpts[ INITMEAN ] );
		for( size_t k = 0; k < meanshape.size(); k++ )
			meanshape[ k ] += sim * allpts[ i ][ k ];
	}
	meanshape.normalize();

	std::cout << "Initial mean" << std::endl;
	size_t iter = 200;
	while( iter-- ) {
		PointSet2f newmean;
		newmean.resize( SAMPLEPTS );
		for( size_t k = 0; k < newmean.size(); k++ )
			newmean[ k ].set( 0.0f, 0.0f );

		for( size_t i = 0; i < allpts.size(); i++ ) {
			Matrix3f sim = allpts[ i ].alignSimilarity( meanshape );
			for( size_t k = 0; k < newmean.size(); k++ )
				newmean[ k ] += sim * allpts[ i ][ k ];
		}
		newmean.normalize();
		meanshape = newmean;
	}

	for( size_t i = 0; i < allpts.size(); i++ ) {
			Matrix3f sim = allpts[ i ].alignSimilarity( meanshape );
			allpts[ i ].transform( sim );
//			std::cout << files[ i ] << " " << i << " : " << allpts[ i ].ssd( meanshape ) << std::endl;
	}

	std::cout << "Aligned shapes" << std::endl;

	PCAf pca( SAMPLEPTS * 2 );
	for( size_t i = 0; i < allpts.size(); i++ ) {
		pca.addSample( ( float* ) &allpts[ i ][ 0 ] );
	}

	Eigen::Matrix<float, Eigen::Dynamic, 1> mean( SAMPLEPTS * 2 );
	Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> pc( SAMPLEPTS * 2, SAMPLEPTS * 2 );
	pca.mean( mean );
	pca.principleComponents( pc );

	FaceShapeWin win( mean, pc );
	win.setSize( 640, 480 );
	win.show();

	WidgetLayout wlslider;
	Slider<float> slider[ 15 ];
#define SLIDERN( num ) \
		slider[ num ].setMinimum( -1.0f ); \
		slider[ num ].setMaximum( 1.0f ); \
		wlslider.setAnchoredRight( 10, 200 ); \
		wlslider.setAnchoredTop( num * 20 + 8 , 12 ); \
		win.addWidget( &slider[ num ], wlslider ); \
		Delegate<void (float)> sliderChange##num( &win, &FaceShapeWin::setWeight<num> ); \
		slider[ num ].valueChanged.add( &sliderChange##num );
	SLIDERN( 0 )
	SLIDERN( 1 )
	SLIDERN( 2 )
	SLIDERN( 3 )
	SLIDERN( 4 )
	SLIDERN( 5 )
	SLIDERN( 6 )
	SLIDERN( 7 )
	SLIDERN( 8 )
	SLIDERN( 9 )
/*	SLIDERN( 10 )
	SLIDERN( 11 )
	SLIDERN( 12 )
	SLIDERN( 13 )
	SLIDERN( 14 )*/

	Application::run();
}
