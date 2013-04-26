#include <cvt/util/ConfigFile.h>
#include <cvt/math/Vector.h>
#include <cvt/gfx/Image.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/util/Time.h>
#include <ALP.h>

using namespace cvt;


class Grid {
	public:
		Grid( size_t width, size_t height, size_t nx, size_t ny, const Matrix3f& K )
		{
			float cellWidth = (float)width / nx;
			float cellHeight = (float)height / ny;

			_gridPoints.reserve( nx * ny );
			_canonicalPoints.reserve( nx * ny );


			float fx = K[ 0 ][ 0 ];
			float fy = K[ 1 ][ 1 ];
			float cx = K[ 0 ][ 2 ];
			float cy = K[ 1 ][ 2 ];
			Vector2f p;
			p.y = 1.5f * cellHeight;
			for( size_t y = 1; y < (ny - 1); y++ ){
				p.x = 1.5f * cellWidth;
				for( size_t x = 1; x < ( nx - 1 ); x++ ){
					_gridPoints.push_back( p );
					_canonicalPoints.push_back( Vector2f( ( p.x - cx ) / fx, ( p.y - cy ) / fy ) );
					p.x += cellWidth;
				}
				p.y += cellHeight;
			}
		}

		void samplePointsFromImage( std::vector<float>&		intensities,
									std::vector<Vector3f>&	points,
									const Image& grayf,
									const Image& depth )
		{
			intensities.reserve( _gridPoints.size() );
			points.reserve( _gridPoints.size() );

			IMapScoped<const float> imap( grayf );
			IMapScoped<const float> dmap( depth );

			for( size_t i = 0; i < _gridPoints.size(); i++ ){
				const Vector2f& p = _gridPoints[ i ];

				int x = p.x;
				int y = p.y;
				float d = dmap( y, x );
				if( d > 0.1 ){
					const Vector2f& pc = _canonicalPoints[ i ];
					intensities.push_back( imap( y, x ) );
					points.push_back( Vector3f( pc.x * d, pc.y * d, d ) );
				}
			}
		}

		size_t numPoints()					const { return _gridPoints.size(); }
		const Vector2f& point( size_t i )	const { return _gridPoints[ i ]; }

	private:
		std::vector<Vector2f>	_gridPoints;

		// u = f_x * X / d + c_x -> X = d * ( u - c_x ) / f_x
		// v = f_y * Y / d + c_y -> Y = d * ( v - c_y ) / f_y
		std::vector<Vector2f>	_canonicalPoints;


};


int main( int argc, char* argv[] )
{
	if( argc != 2 ){
		std::cout << "Usage: " << argv[ 0 ] << " <configfile>" << std::endl;
		return 0;
	}

	ConfigFile cfg( argv[ 1 ] );

	String rgbFileName = cfg.valueForName<String>( "rgb_file", "rgb.png" );
	String depthFileName = cfg.valueForName<String>( "depth_file", "depth.png" );
	float depthScale = cfg.valueForName( "depth_scale", 1000.0f );

	cfg.save( argv[ 1 ] );

	std::cout << "RGB: " << rgbFileName << std::endl;
	std::cout << "Depth: " << depthFileName << std::endl;
	std::cout << "Depth_scale: " << depthScale << std::endl;

	Image rgb( rgbFileName ), grayf;
	Image depth( depthFileName ), depthf;

	rgb.convert( grayf, IFormat::GRAY_FLOAT );
	depth.convert( depthf, IFormat::GRAY_FLOAT );


	try {
		depthf.mul( ( float )0xFFFF / depthScale );

		Matrix3f K( 525.0f,   0.0f, 319.5f,
					0.0f, 525.0f, 239.5f,
					0.0f,   0.0f,   1.0f );
		Grid grid( grayf.width(), grayf.height(), 20, 10, K );

		std::vector<float> intensities;
		std::vector<Vector3f> points;
		grid.samplePointsFromImage( intensities, points, grayf, depthf );

		ALP3D alp( K );

		Time t;
		alp.learn( points, intensities, grayf );

		Matrix4f pose;
		pose.setIdentity();

		// slight offset in tx
		pose[ 0 ][ 2 ] = 0.1f;
		alp.track( pose, grayf );

		std::cout << pose << std::endl;

		// TODO:
		// implement extensions and reduction
		// implement addition of samples
			// do this based on the current image for all current sample points

		/*
			2) track motion
			3) remove regions at border
			4) add sample points/subpredictors at regions with few points
		*/

		std::cout << t.elapsedMilliSeconds() << "ms" << std::endl;

	} catch ( const Exception& e ){
		std::cout << e.what() << std::endl;
	}


	return 0;
}
