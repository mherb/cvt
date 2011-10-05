#include <iostream>


#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/Moveable.h>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/GFXEngineImage.h>

#include <cvt/io/Resources.h>

#include <cvt/gfx/Color.h>
#include <cvt/util/Exception.h>
#include <cvt/util/DataIterator.h>
#include <cvt/util/Time.h>

#include <string>

#include <cvt/io/FileSystem.h>
#include <cvt/vision/ORB.h>
#include <cvt/vision/ORB2.h>

#include <cvt/vision/FeatureMatch.h>

#include "ORBHashMatch.h"
#include "LSH.h"

#include <opencv2/opencv.hpp>

#define TEST_ORB2

using namespace cvt;

void matchFeatures( const ORB & orb0, const ORB & orb1, size_t maxDistance, std::vector<FeatureMatch> & matches )
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

		if( m.distance < maxDistance ){
			matches.push_back( m );
        }
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

void matchFeatures2( const ORB & orb0, const ORB & orb1, size_t maxDistance, std::vector<FeatureMatch> & matches )
{
	ORBHashMatch hashmatch( orb1 );
	int idx;
	size_t dist;

    FeatureMatch match;

	for( size_t i = 0; i < orb0.size(); i++ ){
		if( ( idx = hashmatch.find( orb0[ i ], dist, maxDistance ) ) >= 0 ) {
            match.feature0 = &orb0[ i ];
            match.feature1 = &orb1[ idx ];
            match.distance = dist;
			matches.push_back( match );
		}
	}
}

void matchFeaturesLSH( const ORB & orb0, const ORB & orb1, size_t maxDistance, std::vector<FeatureMatch> & matches )
{
	LSH<10, 3> hashmatch( orb1 );
	int idx;
	size_t dist;

    FeatureMatch match;

	for( size_t i = 0; i < orb0.size(); i++ ){
		if( ( idx = hashmatch.find( orb0[ i ], dist, maxDistance ) ) >= 0 ) {
            match.feature0 = &orb0[ i ];
            match.feature1 = &orb1[ idx ];
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

void checkResult( const std::vector<FeatureMatch> & matches, const Matrix3f & H, float etime = 0.0f, float mtime = 0.0f )
{
	// check reprojection error: dist( H * orb0 - orb1 )
	std::vector<FeatureMatch>::const_iterator it = matches.begin();
	std::vector<FeatureMatch>::const_iterator itEnd = matches.end();

	size_t inlier = 0;

	while( it != itEnd ){
		Vector2f gt = H * it->feature0->pt;

		float error = ( it->feature1->pt - gt ).length();

		if( error < 10.0f )
			inlier++;
		/*else {
			std::cout << "Outlier <" << it->idx0 << ", " << it->idx1 << ">\t" << it->distance << "\tReprojection Error: " << error << std::endl;
			std::cout << "PT2: " << orb1[ it->idx1 ].pt << std::endl;
			std::cout << "GT: " << gt << std::endl;
		}*/

		it++;
	}
	std::cout << "Inlier: " << inlier << " / " << matches.size()  << "\t" << inlier * 100.0f / matches.size() << "%" << "\textract-time " << etime << " ms\tmatch-time " << mtime << " ms" << std::endl;

}


class FeatureWindow : public Window
{
  public:
    FeatureWindow( bool batch ) :
        Window( "Feature Data Tests" ),
        _numScales( 3 ),
        _scaleFactor( 0.7f ),
        _fastThreshold( 20 ),
        _maxDescDistance( 75 ),
        _currentDataSet( 0 ),
        _currentImage( 0 ),
        _orb0( 0 ), _orb1( 0 ),
        _imageMoveable( &_imageView ),
        _nextDataSet( "Next Dataset" ),
        _nextImage( "Next Image" )
    {
        _dataSets.push_back( "bark" );
        _dataSets.push_back( "bikes" );
        _dataSets.push_back( "boat" );
        _dataSets.push_back( "graf" );
        _dataSets.push_back( "leuven" );
        _dataSets.push_back( "trees" );
        _dataSets.push_back( "ubc" );
        _dataSets.push_back( "wall" );

        // create the gui
        setupGui();

        // init the orbs
        loadDataSet();
        calcOrb( 0 );

        nextImage();

        if( batch ){
            runBatchMode();
            exit( 0 );
        }
    }

        void cvt2Ocv( const Image & im, cv::Mat & mat )
        {
            Image img;
            im.convert( img, IFormat::GRAY_UINT8 );

            size_t stride;
            const uint8_t * ptr = img.map( &stride );

            mat.create( img.height(), img.width(), CV_8UC1 );

            SIMD * simd = SIMD::instance();

            for( size_t y = 0; y < img.height(); y++ ){
                simd->Memcpy( mat.ptr( y ), ptr + y * stride, img.width() );
            }

            img.unmap( ptr );
        }

    void testOpenCV()
    {
        cv::ORB::CommonParams orbParams( 1.0f / _scaleFactor, _numScales );
        cv::ORB ocvORB( 1000, orbParams );

        std::vector<cv::KeyPoint> kp0, kp1;

        cv::Mat cvImg;

        cv::Mat mask, desc0, desc1;

        cvt2Ocv( _images[ 0 ], cvImg );
        ocvORB( cvImg, mask, kp0, desc0 );

        cvt2Ocv( _images[ _currentImage ], cvImg );

        ocvORB( cvImg, mask, kp1, desc1 );
        matchOCV( kp0, kp1, desc0, desc1 );
    }

    void matchOCV( const std::vector<cv::KeyPoint> & kp0,
                   const std::vector<cv::KeyPoint> & kp1,
                   const cv::Mat & desc0,
                   const cv::Mat & desc1 )
    {
        cv::BruteForceMatcher<cv::Hamming> matcher;
        std::vector<cv::DMatch> matches;

        matcher.match( desc1, desc0, matches );

        size_t numMatches = 0;
        size_t inlier = 0;
        for( size_t i = 0; i < matches.size(); i++ ){
            if( matches[ i ].distance < _maxDescDistance ){
                numMatches++;

                // test if inlier
                const cv::KeyPoint & kp = kp0[ matches[ i ].trainIdx ];
                Vector2f pt( kp.pt.x, kp.pt.y );

                const cv::KeyPoint & kpQ = kp1[ matches[ i ].queryIdx ];
                Vector2f ptQ( kpQ.pt.x, kpQ.pt.y );

                Vector2f ptPrime = _homographies[ _currentImage ] * pt;

                if( (ptQ - ptPrime).length() < 10 ){
                    inlier++;
                }
            }
        }

        std::cout << "Inlier: " << inlier << " / " << numMatches << "\t" << std::setprecision( 5 ) << 100.0f * (float)inlier / (float)numMatches << "%\t num Overall matches: " << matches.size() << std::endl << std::endl;
    }

    void runBatchMode()
    {
        for( size_t i = 0; i < _dataSets.size(); i++ ){
            if( i != 0 )
                nextDataSet();
            for( size_t k = 0; k < _images.size() - 2 ; k++ ){
                nextImage();
            }
        }
    }

    void setupGui()
    {
        _imageMoveable.setSize( 320, 240 );
        this->addWidget( &_imageMoveable );

        WidgetLayout wl;
        wl.setAnchoredBottom( 10, 20 );
        wl.setAnchoredRight( 10, 100 );

        addWidget( &_nextDataSet, wl );
        wl.setAnchoredBottom( 40, 20 );
        addWidget( &_nextImage, wl );

        _nextDataSet.clicked.add( Delegate<void ()>( this, &FeatureWindow::nextDataSet ) );
        _nextImage.clicked.add( Delegate<void ()>( this, &FeatureWindow::nextImage ) );
    }


    void repaint()
    {
        size_t w = _images[ 0 ].width() + _images[ _currentImage ].width();
        size_t h = cvt::Math::max( _images[ 0 ].height(), _images[ _currentImage ].height() );
        Image out( w, h, _images[ 0 ].format() );


        // draw the two images into a single one
        out.copyRect( 0, 0, _images[ 0 ], _images[ 0 ].rect() );
        out.copyRect( _images[ 0 ].width(), 0, _images[ _currentImage ], _images[ _currentImage ].rect() );

        // draw the features
        {
            GFXEngineImage ge( out );
            GFX g( &ge );
            g.color().set( 0.0f, 1.0f, 0.0f, 1.0f );

            for( size_t i = 0; i < _orb0->size(); i++ ) {
                g.fillRect( (*_orb0)[ i ].pt.x-1, (*_orb0)[ i ].pt.y-1, 3, 3 );
			}
            for( size_t i = 0; i < _orb1->size(); i++ ) {
                g.fillRect( (*_orb1)[ i ].pt.x + _images[ 0 ].width() - 1, (*_orb1)[ i ].pt.y-1, 3, 3 );
			}

            // draw lines between the matching features
            g.color() = Color::RED;
            for( size_t i = 0; i < _matches.size(); i++ ){
                Vector2f pt = _matches[ i ].feature1->pt;
                pt.x += _images[ 0 ].width();
                g.drawLine( _matches[ i ].feature0->pt, pt );
            }
		}

        // set the image:
        _imageView.setImage( out );
    }


    void loadDataSet()
    {
        try {
        cvt::Resources resources;
        String folder = resources.find( "features_dataset" );
        folder += "/";
        folder += _dataSets[ _currentDataSet ];

        _images.resize( 6 );
        _homographies.resize( 6 );

        // first one is identity
        _homographies[ 0 ].setIdentity();
        for ( size_t i = 0; i < 6; i++ ) {
            // load image i:
            String iPath( folder );
            iPath += "/img";
            iPath += ( i + 1 );
            iPath += ".ppm";
            _images[ i ].load( iPath );

            if ( i != 0 ) {
                // load the matrix
                String mPath( folder );
                mPath += "/H1to";
                mPath += ( int ) ( i + 1 );
                mPath += "p";
                loadMatrix3( _homographies[ i ], mPath );
            }
        }

        } catch( const Exception & e ){
            std::cerr << e.what() << std::endl;
        }

        _imageMoveable.setTitle( _dataSets[ _currentDataSet ] );
    }


    void nextDataSet()
    {
        _currentImage = 0;
        _currentDataSet++;
        if( _currentDataSet >= _dataSets.size() )
            _currentDataSet = 0;
        loadDataSet();

        // update the base ORB
        calcOrb( 0 );

        nextImage();
    }

    void calcOrb( size_t index )
    {
        Image gray;
        _images[ index ].convert( gray, IFormat::GRAY_UINT8 );

        if( index == 0 ){
            if( _orb0 )
                delete _orb0;
#ifdef TEST_ORB2
            _orb0 = new ORB2( gray, _numScales, _scaleFactor, _fastThreshold );
#else 
            _orb0 = new ORB( gray, _numScales, _scaleFactor, _fastThreshold );
#endif
        } else {
            if( _orb1 )
                delete _orb1;

#ifdef TEST_ORB2
            _orb1 = new ORB2( gray, _numScales, _scaleFactor, _fastThreshold );
#else 
            _orb1 = new ORB( gray, _numScales, _scaleFactor, _fastThreshold );
#endif
        }
    }

    void nextImage()
    {
        _currentImage++;
        if( _currentImage >= _images.size() )
            _currentImage = 1;

        Time t;
        calcOrb( _currentImage );
        _extractTime = t.elapsedMilliSeconds();

        t.reset();
        _matches.clear();

        //matchFeaturesLSH( *_orb0, *_orb1, _maxDescDistance, _matches );
        matchFeatures( *_orb0, *_orb1, _maxDescDistance, _matches );
        //matchFeatures2( *_orb0, *_orb1, _maxDescDistance, _matches );
        _matchTime = t.elapsedMilliSeconds();

        std::cout << "Image: " << _dataSets[ _currentDataSet ] << " 0 -> " << _currentImage << ":" << std::endl;
        checkResult( _matches, _homographies[ _currentImage ], _extractTime, _matchTime );

        repaint();

        std::cout << "OPENCV:\n";
        testOpenCV();
    }

  private:
    std::vector<String>     _dataSets;
    std::vector<Image>     _images;
    std::vector<Matrix3f>   _homographies;

    size_t                  _numScales;
    float                   _scaleFactor;
    size_t                  _fastThreshold;
    size_t                  _maxDescDistance;

    size_t                  _currentDataSet;
    size_t                  _currentImage;
#ifdef TEST_ORB2
    ORB2*                   _orb0;
    ORB2*                   _orb1;
#else
    ORB*                    _orb0;
    ORB*                    _orb1;
#endif
    float                   _extractTime;
    float                   _matchTime;

    std::vector<FeatureMatch> _matches;

    // gui components
    ImageView               _imageView;
    Moveable                _imageMoveable;
    Button                  _nextDataSet;
    Button                  _nextImage;
};

int main( int argc, char* argv[] )
{

    bool batch = false;
    if( argc == 2 ){
        String argument( argv[ 1 ] );

        if( argument == "batch" )
            batch = true;
    }

	FeatureWindow win( batch );
	win.setSize( 800, 600 );
	win.setVisible( true );
	win.setMinimumSize( 320, 240 );

	Application::run();

	return 0;
}
