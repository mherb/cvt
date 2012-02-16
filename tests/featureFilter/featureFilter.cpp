#include <cvt/vision/ORB.h>
#include <cvt/util/Time.h>

#include <cvt/gfx/GFXEngineImage.h>

#include <cvt/vision/FeatureFilter.h>

using namespace cvt;

static inline void drawOrbsInImage( Image& debug, const Color& c, const ORB& points )
{
	{
		GFXEngineImage ge( debug );
		GFX g( &ge );
		g.color() = c;

		Recti r;
		size_t pSize = 5;
		size_t phSize = pSize >> 1;
		r.width = pSize;
		r.height = pSize;
		for( size_t i = 0; i < points.size(); i++ ){
			r.x = ( int )points[ i ].pt[ 0 ] - phSize;
			r.y = ( int )points[ i ].pt[ 1 ] - phSize;
			g.drawRect( r );
		}
	}
}

static inline void drawFeatures( Image& debug, const Color& c, const std::vector<const Feature2Df*>& points )
{
	{
		GFXEngineImage ge( debug );
		GFX g( &ge );
		g.color() = c;

		Recti r;
		size_t pSize = 5;
		size_t phSize = pSize >> 1;
		r.width = pSize;
		r.height = pSize;
		for( size_t i = 0; i < points.size(); i++ ){
			r.x = ( int )points[ i ]->pt[ 0 ] - phSize;
			r.y = ( int )points[ i ]->pt[ 1 ] - phSize;
			g.drawRect( r );
		}
	}
}

int main( int argc, char* argv[] )
{
	if( argc < 3 ){
		std::cout << "Usage: " << argv[ 0 ] << " <img> <gridsize>" << std::endl;
		return 0;
	}

	Image img( argv[ 1 ] );
	int grid = atoi( argv[ 2 ] );

	Time t;
	ORB orb( img, 3, 0.5f, 25, 0, true );

	std::cout << "Orb detection time: " << t.elapsedMilliSeconds() << "ms" << std::endl;

	t.reset();
	FeatureFilter gridFilter( grid, img.width(), img.height() );
	for( size_t i = 0; i < orb.size(); i++ ){
		gridFilter.addFeature( &orb[ i ] );
	}
	std::vector<const Feature2Df*> features;
	gridFilter.gridFilteredFeatures( features, 1000 );
	
	std::cout << "Filter time: " << t.elapsedMilliSeconds() << "ms" << std::endl;

	std::cout << "ORB size: " << orb.size() << std::endl;

	Image out, out2;
	img.convert( out, IFormat::RGBA_UINT8 );
	img.convert( out2, IFormat::RGBA_UINT8 );
	drawOrbsInImage( out, Color::GREEN, orb );
	drawFeatures( out2, Color::RED, features );

	out.save( "orb_res.png" );
	out2.save( "orb_filtered.png" );

	return 0;
}
