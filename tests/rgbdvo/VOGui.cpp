#include "VOGui.h"
#include <cvt/gui/Application.h>
#include <cvt/geom/scene/ScenePoints.h>
#include <cvt/vision/Vision.h>

namespace cvt {

    VOGui::VOGui( const Matrix3f &intrinsics, float depthScale ) :
        _nextButton( "next" ),
        _stepButton( "enable stepping" ),
        _optimizeButton( "Stop optimizing" ),
        _mainWindow( "RGBD-VO" ),        
        _ssdLabel( "SSD:" ),
        _numPixelLabel( "# Pixel: 0" ),
        _pixelPercentLabel( "\% Pixel: 0\%" ),
        _ssd( 0.0f ),
        _numPixel( 0 ),
        _pixelPercent( 0.0f ),
        _avgSpeed( 0.0 ),
        _depthFactor( depthScale ),
        _allPoints( "pointcloud" ),
        _nKfs( 0 )
    {
        _camCalib.setIntrinsics( intrinsics );
        _gtPose.setIdentity();
        _camPose.setIdentity();

        setupGui();

        _timerId = Application::registerTimer( 20 /*50fps*/, this );
    }

    VOGui::~VOGui()
    {
        Application::unregisterTimer( _timerId );
    }

    void VOGui::onTimeout()
    {
        try {
            processOutstandingKeyframes();

            ScopeLock lock( &_dataMutex );
            _depthView.setImage( _currentDepth );
            _currentImage.setImage( _currentRGB );
            _poseView.setCamPose( _camPose );
            _poseView.setGTPose( _gtPose );

            // update the title and labels
            String str;
            str.sprintf( "RGBDVO: Avg. Speed %0.1f ms", _avgSpeed );
            _mainWindow.setTitle( str );

            str.sprintf( "\% Pixel: %0.1f\%", _pixelPercent );
            _pixelPercentLabel.setLabel( str );

            str.sprintf( "# Pixel: %d", _numPixel );
            _numPixelLabel.setLabel( str );

            str.sprintf( "SSD: %0.2f", _ssd );
            _ssdLabel.setLabel( str );
        } catch ( const cvt::Exception& e ){
            std::cout << e.what() << std::endl;
        }
    }

    void VOGui::processOutstandingKeyframes()
    {
        ScopeLock lock( &_keyframeDataMutex );
        if( _newKeyframeData.size() ){
            std::vector<KeyframeData>::const_iterator it = _newKeyframeData.begin();
            const std::vector<KeyframeData>::const_iterator itEnd = _newKeyframeData.end();
            while( it != itEnd ){
                _poseView.addKeyframe( it->pose );

                if( _nKfs % 2 == 0 ){
                    _camCalib.setExtrinsics( it->pose.inverse() );
                    ScenePoints curPts( "curTmp" );
                    Vision::unprojectToScenePoints( curPts, it->rgb, it->depth, _camCalib, ( float ) ( 0xffff ) / _depthFactor );
                    _allPoints.add( curPts );
                }
                ++it;
                _nKfs++;
            }
            --it;
            _keyframeImage.setImage( it->rgb );
            _newKeyframeData.clear();

            // add the new points to the scene
            _poseView.setScenePoints( _allPoints );
        }
    }

    void VOGui::setupGui()
    {
        _mainWindow.setSize( 1024, 768 );

        WidgetLayout wl;        

        wl.setRelativeLeftRight( 0.0f, 1.0f );
        wl.setRelativeTopBottom( 0.0f, 0.69f );
        _mainWindow.addWidget( &_poseView, wl );


        wl.setRelativeTopBottom( 0.7f, 0.99f );

        wl.setRelativeLeftRight( 0.0f, 0.24f );
        _mainWindow.addWidget( &_currentImage, wl );
        wl.setRelativeLeftRight( 0.25f, 0.49f );
        _mainWindow.addWidget( &_depthView, wl );
        wl.setRelativeLeftRight( 0.5f, 0.74f );
        _mainWindow.addWidget( &_keyframeImage, wl );

        wl.setRelativeLeftRight( 0.75f, 0.99f );
        wl.setRelativeTopBottom( 0.7f, 0.75f );
        _mainWindow.addWidget( &_ssdLabel, wl );
        wl.setRelativeTopBottom( 0.76f, 0.8f );
        _mainWindow.addWidget( &_numPixelLabel, wl );
        wl.setRelativeTopBottom( 0.81f, 0.85f );
        _mainWindow.addWidget( &_pixelPercentLabel, wl );

        wl.setRelativeTopBottom( 0.86f, 0.9f );
        wl.setRelativeLeftRight( 0.75f, 0.874f );
        _mainWindow.addWidget( &_nextButton, wl );
        wl.setRelativeLeftRight( 0.875f, 0.99f );
        _mainWindow.addWidget( &_stepButton, wl );
        wl.setRelativeLeftRight( 0.75f, 0.874f );
        wl.setRelativeTopBottom( 0.91f, 0.95f );
        _mainWindow.addWidget( &_optimizeButton, wl );

        _mainWindow.setVisible( true );
    }

    void VOGui::setOptimize( bool val )
    {
        if( val ){
            _optimizeButton.setLabel( "Stop Opt." );
        } else {
            _optimizeButton.setLabel( "Start Opt." );
        }
    }

    void VOGui::setStepping( bool val )
    {
        if( val ){
            _stepButton.setLabel( "run continuous" );
        } else {
            _stepButton.setLabel( "enable stepping" );
        }
    }

    void VOGui::setOffsetPose( const Matrix4f& pose )
    {
        ScopeLock lock( &_dataMutex );
        _poseView.setOffsetPose( pose );
    }

    void VOGui::setCurrentRGB( const Image& rgb )
    {
        ScopeLock lock( &_dataMutex );
        _currentRGB = rgb;
    }

    void VOGui::setCurrentDepth( const Image& depth )
    {
        ScopeLock lock( &_dataMutex );
        _currentDepth = depth;
    }

    void VOGui::setPose( const Matrix4f& pose )
    {
        ScopeLock lock( &_dataMutex );
        _camPose = pose;
    }

    void VOGui::setGroundTruthPose( const Matrix4f& pose )
    {
        ScopeLock lock( &_dataMutex );
        _gtPose = pose;
    }

    void VOGui::setSSDLabel( float ssd )
    {
        ScopeLock lock( &_dataMutex );
        _ssd = ssd;
    }

    void VOGui::setNumPixels( size_t n )
    {
        ScopeLock lock( &_dataMutex );
        _numPixel = n;
    }

    void VOGui::setPixelPercentage( float val )
    {
        ScopeLock lock( &_dataMutex );
        _pixelPercent = val;
    }

    void VOGui::setSpeed( float speed )
    {
        ScopeLock lock( &_dataMutex );
        _avgSpeed = speed;
    }

    void VOGui::addKeyframe( const KeyframeData& data )
    {
        ScopeLock lock( &_keyframeDataMutex );
        _newKeyframeData.push_back( data );
    }

}
