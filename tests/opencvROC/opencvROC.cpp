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

#include <opencv2/opencv.hpp>

//#define TEST_ORB2

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


class FeatureROC
{
  public:
    FeatureROC() :
        _numScales( 4 ),
        _scaleFactor( 0.75f ),
        _fastThreshold( 30 ),
        _currentDataSet( 0 ),
        _currentImage( 0 ),
		_maxFeatures( 1000 ),
		_orbParams( 1.0 / _scaleFactor, _numScales ),
		_orb( _maxFeatures, _orbParams )
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
        calcOrb( _kp0, _desc0, _images[ 0 ] );
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

	void calcOrb( cv::vector<cv::KeyPoint> & kp, cv::Mat & desc, const cvt::Image & img )
	{
        cv::Mat cvImg;
		cv::Mat mask;

		kp.clear();

        cvt2Ocv( img, cvImg );
        _orb( cvImg, mask, kp, desc );
	}

    void nextDataSet()
    {
        _currentImage = 0;
        _currentDataSet++;
        if( _currentDataSet >= _dataSets.size() )
            _currentDataSet = 0;
        loadDataSet();

        // update the base ORB
        calcOrb( _kp0, _desc0, _images[ 0 ] );

        nextImage();
    }

    void nextImage()
    {
        _currentImage++;
        if( _currentImage >= _images.size() )
            _currentImage = 1;

		// calc the orb for the current image
		calcOrb( _kp1, _desc1, _images[ _currentImage ] );

        _matches.clear();
        matchFeatures();
		Feature2DROC roc( _matches, _homographies[ _currentImage ] );
		String path = _dataSets[ _currentDataSet ];
		path += "_ocv_ROC1to";
		path += _currentImage + 1;
		path += ".txt";
		roc.toFile( path );
    }

	void matchFeatures()
    {
        cv::BruteForceMatcher<cv::Hamming> matcher;
        std::vector<cv::DMatch> matches;
        matcher.match( _desc1, _desc0, matches );

		_f0.resize( _kp0.size() );
		for( size_t i = 0; i < _kp0.size(); i++ ){
			_f0[ i ].pt.x = _kp0[ i ].pt.x;
			_f0[ i ].pt.y = _kp0[ i ].pt.y;
			_f0[ i ].angle = _kp0[ i ].angle;
			_f0[ i ].octave = _kp0[ i ].octave;
			_f0[ i ].score = _kp0[ i ].response;
		}

		_f1.resize( _kp1.size() );
		for( size_t i = 0; i < _kp1.size(); i++ ){
			_f1[ i ].pt.x   = _kp1[ i ].pt.x;
			_f1[ i ].pt.y   = _kp1[ i ].pt.y;
			_f1[ i ].angle  = _kp1[ i ].angle;
			_f1[ i ].octave = _kp1[ i ].octave;
			_f1[ i ].score  = _kp1[ i ].response;
		}

		_matches.resize( matches.size() );
		for( size_t i = 0; i < matches.size(); i++ ){
			_matches[ i ].feature0 = &_f0[ matches[ i ].trainIdx ];
			_matches[ i ].feature1 = &_f1[ matches[ i ].queryIdx ];
			_matches[ i ].distance = matches[ i ].distance;
		}
    }

  private:
    std::vector<String>       _dataSets;
    std::vector<Image>        _images;
    std::vector<Matrix3f>     _homographies;

    size_t                    _numScales;
    float                     _scaleFactor;
    size_t                    _fastThreshold;

    size_t                    _currentDataSet;
    size_t                    _currentImage;
    std::vector<FeatureMatch> _matches;
	size_t					  _maxFeatures;


    cv::ORB::CommonParams	  _orbParams;
	cv::vector<cv::KeyPoint>  _kp0;
	cv::vector<cv::KeyPoint>  _kp1;
	cv::ORB					  _orb;
	cv::Mat					  _mask, _desc0, _desc1;

	std::vector<Feature2Df>	  _f0, _f1;
};

int main( int argc, char* argv[] )
{
	FeatureROC roc;
	return 0;
}
