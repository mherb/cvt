#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/io/Resources.h>

#include <cvt/gfx/Color.h>
#include <cvt/util/Exception.h>
#include <cvt/util/DataIterator.h>

#include <cvt/io/FileSystem.h>
#include <cvt/vision/ORB.h>
#include <cvt/vision/ORB2.h>
#include <cvt/vision/Feature2DROC.h>

#include <cvt/vision/FeatureMatch.h>

#define TEST_ORB2

using namespace cvt;

void matchFeatures( const ORB & orb0, const ORB & orb1, std::vector<FeatureMatch> & matches )
{
    FeatureMatch m;
	for( size_t i = 0; i < orb0.size(); i++ ){
		m.feature0 = &orb0[ i ];
		m.distance = 1e10f;
		for( size_t k = 0; k < orb1.size(); k++ ){
			size_t dist = orb0[ i ].distance( orb1[ k ] );

			if( dist < m.distance ){
				m.distance = dist;
                m.feature1 = &orb1[ k ];
			}
		}
		matches.push_back( m );
	}
}

void matchFeatures( const ORB2 & orb0, const ORB2 & orb1, std::vector<FeatureMatch> & matches )
{
    FeatureMatch m;

	// brighter features
	for( size_t i = 0; i < orb0.numBrighter(); i++ ){
		const ORB2Feature & f = orb0.brighterFeature( i );
		m.feature0 = &orb0.brighterFeature( i );
		m.distance = 1e10f;
		for( size_t k = 0; k < orb1.numBrighter(); k++ ){
			size_t dist = f.distance( orb1.brighterFeature( k ) );

			if( dist < m.distance ){
				m.distance = dist;
                m.feature1 = &orb1.brighterFeature( k );
			}
		}
		matches.push_back( m );
	}

	// darker features
	for( size_t i = 0; i < orb0.numDarker(); i++ ){
		const ORB2Feature & f = orb0.darkerFeature( i );
		m.feature0 = &orb0.darkerFeature( i );
		m.distance = 1e10f;
		for( size_t k = 0; k < orb1.numDarker(); k++ ){
			size_t dist = f.distance( orb1.darkerFeature( k ) );

			if( dist < m.distance ){
				m.distance = dist;
				m.feature1 = &orb1.darkerFeature( k );
			}
		}
		matches.push_back( m );
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


class FeatureROC
{
  public:
    FeatureROC() :
        _numScales( 4 ),
        _scaleFactor( 0.8f ),
        _fastThreshold( 30 ),
		_maxFeatures( 1000 ),
        _currentDataSet( 0 ),
        _currentImage( 0 ),
        _orb0( 0 ), _orb1( 0 )
    {
        _dataSets.push_back( "bark" );
        _dataSets.push_back( "bikes" );
        _dataSets.push_back( "boat" );
        _dataSets.push_back( "graf" );
        _dataSets.push_back( "leuven" );
        _dataSets.push_back( "trees" );
        _dataSets.push_back( "ubc" );
        _dataSets.push_back( "wall" );

        // init the orbs
        loadDataSet();
        calcOrb( 0 );
        nextImage();
        runBatchMode();
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
            _orb0 = new ORB2( gray, _numScales, _scaleFactor, _fastThreshold, _maxFeatures );
#else 
            _orb0 = new ORB( gray, _numScales, _scaleFactor, _fastThreshold, _maxFeatures );
#endif
        } else {
            if( _orb1 )
                delete _orb1;

#ifdef TEST_ORB2
            _orb1 = new ORB2( gray, _numScales, _scaleFactor, _fastThreshold, _maxFeatures );
#else 
            _orb1 = new ORB( gray, _numScales, _scaleFactor, _fastThreshold, _maxFeatures );
#endif
        }
    }

    void nextImage()
    {
        _currentImage++;
        if( _currentImage >= _images.size() )
            _currentImage = 1;

        calcOrb( _currentImage );

        _matches.clear();
        matchFeatures( *_orb0, *_orb1, _matches );
		Feature2DROC roc( _matches, _homographies[ _currentImage ] );
		std::cout << _dataSets[ _currentDataSet ] << " 1 to " << _currentImage + 1 << " AUC : " << roc.AUC() << std::endl;
		String path = _dataSets[ _currentDataSet ];
		path += "ROC1to";
		path += _currentImage + 1;
		path += ".txt";
		roc.toFile( path );
    }

  private:
    std::vector<String>     _dataSets;
    std::vector<Image>     _images;
    std::vector<Matrix3f>   _homographies;

    size_t                  _numScales;
    float                   _scaleFactor;
    size_t                  _fastThreshold;
	size_t					_maxFeatures;

    size_t                  _currentDataSet;
    size_t                  _currentImage;
#ifdef TEST_ORB2
    ORB2*                   _orb0;
    ORB2*                   _orb1;
#else
    ORB*                    _orb0;
    ORB*                    _orb1;
#endif
    std::vector<FeatureMatch> _matches;
};

int main( int argc, char* argv[] )
{
	FeatureROC roc;
	return 0;
}
