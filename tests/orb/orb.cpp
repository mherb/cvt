#include <cvt/gfx/Image.h>
#include <cvt/gfx/GFX.h>
#include <cvt/gfx/GFXEngineImage.h>
#include <cvt/vision/ORB.h>
#include <cvt/io/Resources.h>

using namespace cvt;

const ORBFeature& mindist( const ORBFeature& ft, const ORB& orb, size_t& dist )
{
  size_t mindist = 256;
  size_t index = 0;
  for( int i = 0; i< orb.size(); i++ ){
		size_t tmp = ft.distance( orb[ i ] );
		if( tmp < mindist ) {
			index = i;
			mindist = tmp;
		}
  }
  dist = mindist;
  return orb[ index ];
}

int main( int argc, char * argv[] )
{
	Resources res;

	Image img;
	Image img2;

	img.load( res.find( "lena_g.png" ).c_str() );
	//img2.load( res.find( "lena_g_rot.png" ).c_str() );
	img2.load( res.find( "lena_g.png" ).c_str() );

	Image out( img.width(), img.height(), IFormat::RGBA_UINT8 );
	Image out2( img2.width(), img2.height(), IFormat::RGBA_UINT8 );
	img.convert( out );
	img2.convert( out2 );

	Recti imgRect( 0, 0, img.width(), img.height() );
	Image correspondences( img2.width() * 2, img2.height(), IFormat::RGBA_UINT8 );
	correspondences.copyRect( 0, 0, out, imgRect );
	correspondences.copyRect( img.width(), 0, out2, imgRect );

	ORB orb1( img, 1, 0.5, 40 );
	ORB orb2( img2, 1, 0.5, 40 );

	{
		GFXEngineImage ge( correspondences );
		GFX g( &ge );
		g.color().set( 0.0f, 1.0f, 0.0f, 1.0f );


		size_t dist;
		for( int i = 0; i < orb1.size(); i++ ) {
			const ORBFeature & match = mindist( orb1[ i ], orb2, dist );

			if( dist < 45 ){
				g.drawLine( orb1[ i ].pt.x, orb1[ i ].pt.y,
						    match.pt.x + img.width(), match.pt.y );
			}

		}

	}

	correspondences.save( "corresp.png" );


	return 0;
}
