#include <iostream>


#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/Moveable.h>

#include <cvt/gfx/Image.h>
#include <cvt/io/Resources.h>

#include <cvt/gfx/Color.h>
#include <cvt/util/Exception.h>
#include <cvt/util/DataIterator.h>
#include <cvt/util/Time.h>

#include <string>

#include <cvt/io/FileSystem.h>
#include <cvt/vision/ORB.h>

#include "cvt/vision/FeatureMatch.h"

#include "ORBHashMatch.h"

using namespace cvt;

void matchFeatures( const ORB & orb0, const ORB & orb1, size_t maxDistance, std::vector<FeatureMatch> & matches )
{
	FeatureMatch match;
	for( size_t i = 0; i < orb0.size(); i++ ){
		match.idx0 = i;
		match.idx1 = 0;
		match.distance = maxDistance;
		for( size_t k = 0; k < orb1.size(); k++ ){
			size_t dist = orb0[ i ].distance( orb1[ k ] );

			if( dist < match.distance ){
				match.distance = dist;
				match.idx1 = k;
			}
		}

		if( match.distance < maxDistance )
			matches.push_back( match );
	}
}

void matchFeatures2( const ORB & orb0, const ORB & orb1, size_t maxDistance, std::vector<FeatureMatch> & matches )
{
	FeatureMatch match;
	ORBHashMatch hashmatch( orb1 );
	int idx;
	size_t dist;

	for( size_t i = 0; i < orb0.size(); i++ ){
		match.idx0 = i;
		match.idx1 = 0;

		if( ( idx = hashmatch.find( orb0[ i ], dist, maxDistance ) ) >= 0 ) {
			match.idx1 = idx;
			match.distance = dist;
			matches.push_back( match );
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
					String message( "Matrix File Corrpt: " );
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

void loadTestData( const String & dataFolder, std::vector<Image> & images, std::vector<Matrix3f> & homographies )
{
    cvt::Resources resources;
    String folder = resources.find( "features_dataset" );
	folder += "/";
    folder += dataFolder;

    images.resize( 6 );
    homographies.resize( 6 );

    // first one is identity
    homographies[ 0 ].identity();
    for( size_t i = 0; i < 6; i++ ){
        // load image i:
        String iPath( folder );
        iPath += "/img"; iPath += ( i+1 ); iPath += ".ppm";
        images[ i ].load( iPath );

        if( i != 0 ){
            // load the matrix
            String mPath( folder );
            mPath += "/H1to";
			mPath += (int)( i+1 );
			mPath += "p";
            loadMatrix3( homographies[ i ], mPath );
        }
    }
}

void checkResult( const ORB & orb0, const ORB & orb1, const std::vector<FeatureMatch> & matches, const Matrix3f & H, float time = 0.0f )
{
	// check reprojection error: dist( H * orb0 - orb1 )
	std::vector<FeatureMatch>::const_iterator it = matches.begin();
	std::vector<FeatureMatch>::const_iterator itEnd = matches.end();

	size_t inlier = 0;

	while( it != itEnd ){
		Vector2f gt = H * orb0[ it->idx0 ].pt;

		float error = ( orb1[ it->idx1 ].pt - gt ).length();

		if( error < 10.0f )
			inlier++;
		/*else {
			std::cout << "Outlier <" << it->idx0 << ", " << it->idx1 << ">\t" << it->distance << "\tReprojection Error: " << error << std::endl;
			std::cout << "PT2: " << orb1[ it->idx1 ].pt << std::endl;
			std::cout << "GT: " << gt << std::endl;
		}*/

		it++;
	}
	std::cout << "Inlier: " << inlier << " / " << matches.size()  << "\t" << inlier * 100.0f / matches.size() << "%" << "\tmatch-time " << time << " ms" << std::endl;

}

int main()
{
    std::vector<String> dataSets;
    dataSets.push_back( "bark" );
    dataSets.push_back( "bikes" );
    dataSets.push_back( "boat" );
    dataSets.push_back( "graf" );
    dataSets.push_back( "leuven" );
    dataSets.push_back( "trees" );
    dataSets.push_back( "ubc" );
    dataSets.push_back( "wall" );

	std::vector<Image> images;
	std::vector<Matrix3f> homographies;

	size_t numScales = 4;
	float  scaleFactor = 0.7f;
	size_t featureThreshold = 35;
	size_t maxDistance = 30;

    for ( size_t i = 0; i < dataSets.size( ); i++ ) {
        try {
            loadTestData( dataSets[ i ], images, homographies );
        } catch ( const Exception & e ) {
            std::cerr << e.what( ) << std::endl;
            return 1;
        }

        Image gray;

        images[ 0 ].convert( gray, IFormat::GRAY_UINT8 );
        ORB orb0( gray, numScales, scaleFactor, featureThreshold );

		std::cout << "Set: " << dataSets[ i ] << std::endl;

		for( size_t k = 1; k < images.size(); k++ ){
            images[ k ].convert( gray, IFormat::GRAY_UINT8 );
            ORB orb1( gray, numScales, scaleFactor, featureThreshold );

            std::vector<FeatureMatch> matches;
			Time t;
			t.reset();
            matchFeatures( orb0, orb1, maxDistance, matches );
			float ms = t.elapsedMilliSeconds();

			std::cout << "Image " << k << ":\t";
            checkResult( orb0, orb1, matches, homographies[ k ], ms );
		}
		std::cout << std::endl;
    }

	Window win( "Feature Detector and Descriptor Test" );
	win.setSize( 800, 600 );
	win.setVisible( true );
	win.setMinimumSize( 320, 240 );

	ImageView imView;
	imView.setImage( images[ 0 ] );
	Moveable mov( &imView );
	mov.setSize( images[ 0 ].width() / 2, images[ 0 ].height() / 2 );
	win.addWidget( &mov );

	Application::run();

	return 0;
}
