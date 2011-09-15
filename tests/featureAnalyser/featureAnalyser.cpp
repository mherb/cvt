#include <iostream>

#include <cvt/io/Resources.h>
#include <cvt/util/Exception.h>
#include <cvt/util/DataIterator.h>
#include <cvt/util/Time.h>
#include <cvt/io/FileSystem.h>
#include <cvt/vision/ORB.h>
#include <cvt/vision/ORB2.h>
#include <cvt/vision/FeatureMatch.h>
#include <cvt/gfx/GFXEngineImage.h>

#define TEST_ORB2

using namespace cvt;

void matchFeatures( const ORB & orb0, const ORB & orb1, size_t maxDistance,  std::vector<FeatureMatch> & matches )
{
    FeatureMatch m;
	for( size_t i = 0; i < orb0.size(); i++ ){
		m.feature0 = &orb0[ i ];
		m.distance = maxDistance;
		for( size_t k = 0; k < orb1.size(); k++ ){
			size_t dist = orb0[ i ].distance( orb1[ k ] );

			if( dist < m.distance ){
				m.distance = dist;
                m.feature1 = &orb1[ k ];
			}
		}

		if( m.distance < maxDistance )
			matches.push_back( m );
	}
}

void matchFeatures( const ORB2 & orb0, const ORB2 & orb1, size_t maxDistance, std::vector<FeatureMatch> & matches )
{
    FeatureMatch m;

	// brighter features
	for( size_t i = 0; i < orb0.numBrighter(); i++ ){
		const ORB2Feature & f = orb0.brighterFeature( i );
		m.feature0 = &orb0.brighterFeature( i );
		m.distance = maxDistance;
		for( size_t k = 0; k < orb1.numBrighter(); k++ ){
			size_t dist = f.distance( orb1.brighterFeature( k ) );

			if( dist < m.distance ){
				m.distance = dist;
                m.feature1 = &orb1.brighterFeature( k );
			}
		}

		if( m.distance < maxDistance ){
			matches.push_back( m );
        }
	}

	// darker features
	for( size_t i = 0; i < orb0.numDarker(); i++ ){
		const ORB2Feature & f = orb0.darkerFeature( i );
		m.feature0 = &orb0.darkerFeature( i );
		m.distance = maxDistance;
		for( size_t k = 0; k < orb1.numDarker(); k++ ){
			size_t dist = f.distance( orb1.darkerFeature( k ) );

			if( dist < m.distance ){
				m.distance = dist;
                m.feature1 = &orb1.darkerFeature( k );
			}
		}

		if( m.distance < maxDistance ){
			matches.push_back( m );
        }
	}
}

void loadMatrix3( Matrix3f & m, const String & path )
{
	Data d;
	if( FileSystem::load( d, path ) ){
		DataIterator iter( d );

		String token;
		for( int r = 0; r < m.dimension(); r++ ){
			for( int c = 0; c < m.dimension(); c++ ){
				if( !iter.nextToken( token, " \n\t\r" ) ){
					String message( "Matrix File Corrupt: " );
					message += path;
					throw CVTException( message.c_str() );
				}
				m[ r ][ c ] = token.toFloat();
			}
		}
	} else {
		String message( "Could not load homography file: " );
		message += path;
		throw CVTException( message.c_str() );
	}
}

void drawEllipse( GFX* g, float cx, float cy, float a, float b, float rotrad = 0 )
{
	float c, s;
#define STEPS 20
	float step = Math::TWO_PI / ( float ) STEPS;
	float t = 0;

	c = Math::cos( rotrad );
	s = Math::sin( rotrad );

	std::vector<Point2f> pts;

	for( size_t i = 0; i < STEPS; i++ ) {
		Point2f pt1( cx + a * Math::cos( t ) * c - b * Math::sin( t ) * s,
					 cy + a * Math::cos( t ) * s + b * Math::sin( t ) * c );
		t += step;
		Point2f pt2( cx + a * Math::cos( t ) * c - b * Math::sin( t ) * s,
					 cy + a * Math::cos( t ) * s + b * Math::sin( t ) * c );
		pts.push_back( pt1 );
		pts.push_back( pt2 );
	}

	g->drawLines( &pts[ 0 ], pts.size() / 2 );

#undef STEPS
}


void extractPatches( Image & p, 
					 size_t x, 
					 size_t y, 
					 size_t patchSize,
					 size_t spacing, 
					 const Image & g0, 
					 const Image & g1, 
					 const FeatureMatch & m, 
					 const Matrix3f & H,
					 GFX* g )
{
	Recti r;
	Vector2f pos;
	r.width = patchSize;
	r.height = patchSize;
	int offset = patchSize >> 1;

	// extract patch from image 0:
	r.x = ( int )m.feature0->pt.x - offset; 
	r.y = ( int )m.feature0->pt.y - offset;
    p.copyRect( x, y, g0, r );	

	drawEllipse( g, x + offset, y + offset, 
				( ( ORB2Feature* ) m.feature0 )->sx * offset / 2,  ( ( ORB2Feature* ) m.feature0 )->sy * offset / 2,
				 m.feature0->angle );

	// extract patch from image 1:
	r.x = ( int )m.feature1->pt.x - offset; 
	r.y = ( int )m.feature1->pt.y - offset;
	x += patchSize + 2 * spacing;
    p.copyRect( x, y, g1, r );

	drawEllipse( g, x + offset, y + offset, 
				( ( ORB2Feature* ) m.feature1 )->sx * offset / 2,  ( ( ORB2Feature* ) m.feature1 )->sy * offset / 2,
				 m.feature1->angle );

	// the true position 
	pos = H * m.feature0->pt;
	r.x = ( int )pos.x - offset; 
	r.y = ( int )pos.y - offset;
	x += patchSize + 2 * spacing;
    p.copyRect( x, y, g1, r );
}

int main( int argc, char* argv[] )
{
    if( argc != 4 ){
		std::cout << "Usage: " << argv[ 0 ] << " <img0> <img1> <H0to1>" << std::endl;
		return 0;
    }

	String file0( argv[ 1 ] );
	String file1( argv[ 2 ] );
	String transformFile( argv[ 3 ] );

	Matrix3f H;
	Image img0, img1, g0, g1;

	img0.load( file0 );
	img1.load( file1 );
	loadMatrix3( H, transformFile );


	// ORB parameters:
	size_t	octaves = 3;
	float	scaleFactor = 0.7f;
	uint8_t cornerThreshold = 23;
	size_t	maxFeatures = 1000;
	bool    nonmaxSuppress = true;

	img0.convert( g0, IFormat::GRAY_UINT8 );
	img1.convert( g1, IFormat::GRAY_UINT8 );

#ifdef TEST_ORB2
	ORB2 orb0( g0, octaves, scaleFactor, cornerThreshold, maxFeatures, nonmaxSuppress );
	ORB2 orb1( g1, octaves, scaleFactor, cornerThreshold, maxFeatures, nonmaxSuppress );
#else	
	ORB orb0( g0, octaves, scaleFactor, cornerThreshold, maxFeatures, nonmaxSuppress );
	ORB orb1( g1, octaves, scaleFactor, cornerThreshold, maxFeatures, nonmaxSuppress );
#endif

	std::vector<FeatureMatch> matches;	
	matchFeatures( orb0, orb1, 100, matches );

	size_t patchSize = 31;
	size_t spacing = 2;
	size_t interDataSpacing = 5;
	size_t numPatches = 3;
	size_t featureSpaceX = numPatches * ( 2 * spacing + patchSize ) + interDataSpacing;  	
	size_t featureSpaceY = 2 * spacing + patchSize;

	size_t nx = 1024 / featureSpaceX;
	size_t ny = Math::floor( (float)matches.size() / (float)nx ) + 1;

	Image big( nx * featureSpaceX, ny * featureSpaceY, IFormat::GRAY_UINT8 );
	big.fill( Color::BLACK );
	size_t currX = spacing;
	size_t currY = spacing;
	size_t num = 0;

	{
		GFXEngineImage gfximg( big );
		GFX g( &gfximg );
		g.color() = Color::WHITE;
		g.setLineWidth( 1.0f );
		for( size_t y = 0; y < ny; y++ ){
			currX = spacing;
			for( size_t x = 0; x < nx; x++ ){
				extractPatches( big, currX, currY, patchSize, spacing, g0, g1, matches[ num ], H, &g );
				num++;
				if( num == matches.size() )
					break;
				currX += featureSpaceX;

			}
			currY += featureSpaceY;
		}
	}

	big.save( "feature_analysis.png" );	

	return 0;
}
