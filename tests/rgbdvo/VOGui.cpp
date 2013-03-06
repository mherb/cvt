#include "VOGui.h"

namespace cvt {

    VOGui::VOGui() :
        _nextButton( "next" ),
        _stepButton( "enable stepping" ),
        _optimizeButton( "Stop optimizing" ),
        _mainWindow( "RGBD-VO" ),
        _kfMov( &_keyframeImage ),
        _imageMov( &_currentImage ),
        _depthViewMov( &_depthView ),
        _poseMov( &_poseView ),
        _ssdLabel( "SSD:" ),
        _numPixelLabel( "# Pixel: 0" ),
        _pixelPercentLabel( "\% Pixel: 0\%" )
    {
        setupGui();
    }

    void VOGui::setupGui()
    {
        _mainWindow.setSize( 800, 600 );

        _mainWindow.addWidget( &_kfMov );
        _mainWindow.addWidget( &_imageMov );
        _mainWindow.addWidget( &_depthViewMov );

        // add widgets as necessary
        _kfMov.setSize( 300, 200 );
        _kfMov.setTitle( "Current Keyframe" );

        _depthViewMov.setSize( 300, 200 );
        _depthViewMov.setTitle( "Current Depth" );
        _depthViewMov.setPosition( 200, 0 );

        _imageMov.setSize( 300, 200 );
        _imageMov.setPosition( 300, 0 );
        _imageMov.setTitle( "Current Image" );

        _mainWindow.addWidget( &_poseMov );
        _poseMov.setSize( 300, 200 );
        _poseMov.setPosition( 300, 200 );
        _poseMov.setTitle( "Poses" );

        WidgetLayout wl;

        wl.setAnchoredBottom( 5, 30 );
        wl.setAnchoredRight( 5, 120 );
        _mainWindow.addWidget( &_nextButton, wl );
        wl.setAnchoredBottom( 40, 30 );
        _mainWindow.addWidget( &_stepButton, wl );
        wl.setAnchoredBottom( 75, 30 );
        _mainWindow.addWidget( &_optimizeButton, wl );
        wl.setAnchoredBottom( 110, 30 );
        wl.setAnchoredRight( 5, 150 );
        _mainWindow.addWidget( &_ssdLabel, wl );
        wl.setAnchoredBottom( 145, 30 );
        _mainWindow.addWidget( &_numPixelLabel, wl );
        wl.setAnchoredBottom( 180, 30 );
        _mainWindow.addWidget( &_pixelPercentLabel, wl );
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
        _poseView.setOffsetPose( pose );
    }

    void VOGui::setCurrentRGB( const Image& rgb )
    {
        _currentImage.setImage( rgb );
    }

    void VOGui::setCurrentDepth( const Image& depth )
    {
        _depthView.setImage( depth );
    }

    void VOGui::setCurrentKeyframe( const Image& img )
    {
        _keyframeImage.setImage( img );
    }

    void VOGui::setPose( const Matrix4f& pose )
    {
        _poseView.setCamPose( pose );
    }

    void VOGui::setGroundTruthPose( const Matrix4f& pose )
    {
        _poseView.setGTPose( pose );
    }

    void VOGui::setSSDLabel( float ssd )
    {
        String str;
        str.sprintf( "SSD: %0.2f", ssd );
        _ssdLabel.setLabel( str );
    }

    void VOGui::setNumPixels( size_t n )
    {
        String str;
        str.sprintf( "# Pixel: %d", n );
        _numPixelLabel.setLabel( str );
    }

    void VOGui::setPixelPercentage( float val )
    {
        String str;
        str.sprintf( "\% Pixel: %0.1f\%", val );
        _pixelPercentLabel.setLabel( str );
    }

    void VOGui::setSpeed( float speed )
    {
        String title;
        title.sprintf( "RGBDVO: Avg. Speed %0.1f ms", speed );
        _mainWindow.setTitle( title );
    }

    void VOGui::addKeyframe( const Matrix4f& pose )
    {
        _poseView.addKeyframe( pose );
    }

}
