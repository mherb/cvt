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

#include <string>

#include <cvt/io/FileSystem.h>

using namespace cvt;

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
            mPath += "/H1to"; mPath += i+1; mPath += "p";
            loadMatrix3( homographies[ i ], mPath );
        }
    }
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

	try {
		loadTestData( dataSets[ 0 ], images, homographies );
	} catch ( const Exception & e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}


	Window win( "Feature Detector and Descriptor Test" );
	win.setSize( 800, 600 );
	win.setVisible( true );
	win.setMinimumSize( 320, 240 );

	ImageView imView;
	imView.setImage( images[ 0 ] );
	Moveable mov( &imView );
	mov.setSize( images[ 0 ].width(), images[ 0 ].height() );
	win.addWidget( &mov );

	Application::run();

	return 0;
}
