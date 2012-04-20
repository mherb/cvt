#include <RGBDVOApp.h>

#include <cvt/gui/Application.h>
#include <cvt/util/EigenBridge.h>
#include <cvt/util/Delegate.h>
#include <cvt/util/Time.h>

namespace cvt
{
    RGBDVOApp::RGBDVOApp( const String& folder, const Matrix3f& K ) :
        _parser( folder, 0.1f ),
    #ifdef MULTISCALE
        _aligner( K, 5, 5000.0f, 3, 0.5f ),
    #else
        _aligner( K, 15, 5000.0f),
    #endif
        _mainWindow( "RGBD-VO" ),
        _kfMov( &_keyframeImage ),
        _imageMov( &_currentImage ),
        _poseMov( &_poseView ),
        _nextButton( "next" ),
        _nextPressed( false ),
        _stepButton( "toggle stepping" ),
        _step( true ),
        _optimizeButton( "optimize" ),
        _optimize( false )
    {
        _timerId = Application::registerTimer( 20, this );
        setupGui();

        _parser.loadNext();
        Image gray, depth;
        _parser.data().rgb.convert( gray, IFormat::GRAY_FLOAT );
        _parser.data().depth.convert( depth, IFormat::GRAY_FLOAT );

        addNewKeyframe( gray, depth, _parser.data().pose );
    }

    RGBDVOApp::~RGBDVOApp()
    {
        for( size_t i = 0; i < _keyframes.size(); i++ ){
            delete _keyframes[ i ];
        }

        Application::unregisterTimer( _timerId );
    }

    void RGBDVOApp::onTimeout()
    {
        if( _nextPressed || !_step ){
            _parser.loadNext();
            _currentImage.setImage( _parser.data().rgb );
            _nextPressed = false;
        }

        if( _optimize ){
            Time t;
            const RGBDParser::RGBDSample& d = _parser.data();
            // try to align:
            Image gray( d.rgb.width(), d.rgb.height(), IFormat::GRAY_FLOAT );
            d.rgb.convert( gray );

            _aligner.alignWithKeyframe( _relativePose, *_activeKeyframe, gray );

            std::cout << "Alignment took: " << t.elapsedMilliSeconds() << "ms" << std::endl;

            // update the absolute pose
            Matrix4f tmp;
            EigenBridge::toCVT( tmp, _relativePose.transformation() );
            _absolutePose = _activeKeyframe->pose() * tmp.inverse();

            std::cout << "Pose error: \n" << _absolutePose - d.pose << std::endl;

            if( needNewKeyframe( tmp ) ){
                Image depth( d.depth.width(), d.depth.height(), IFormat::GRAY_FLOAT );
                d.depth.convert( depth );
                addNewKeyframe( gray, depth, _absolutePose );
            }

            _poseView.setCamPose( _absolutePose );
            _poseView.setGTPose( d.pose );
        }
    }

    bool RGBDVOApp::needNewKeyframe( const Matrix4f& rel ) const
    {
        Vector3f t;
        t.x = rel[ 0 ][ 3 ];
        t.y = rel[ 1 ][ 3 ];
        t.z = rel[ 2 ][ 3 ];

        if( t.length() > 0.5f )
            return true;
        return false;
    }

    void RGBDVOApp::setupGui()
    {
        _mainWindow.setSize( 800, 600 );

        _mainWindow.addWidget( &_kfMov );
        _mainWindow.addWidget( &_imageMov );

        // add widgets as necessary
        _kfMov.setSize( 300, 200 );
        _kfMov.setTitle( "Current Keyframe" );

        _imageMov.setSize( 300, 200 );
        _imageMov.setPosition( 300, 0 );
        _imageMov.setTitle( "Current Image" );

        _mainWindow.addWidget( &_poseMov );
        _poseMov.setSize( 300, 200 );
        _poseMov.setPosition( 600, 200 );
        _poseMov.setTitle( "Poses" );

        WidgetLayout wl;

        /*
        wl.setRelativeLeftRight( 0.02f, 0.98f);
        wl.setRelativeTopBottom( 0.02f, 0.98f);
        _mainWindow.addWidget( &_sceneView, wl );
        */

        wl.setAnchoredBottom( 5, 30 );
        wl.setAnchoredRight( 5, 70 );
        _mainWindow.addWidget( &_nextButton, wl );
        Delegate<void ()> d( this, &RGBDVOApp::nextPressed );
        _nextButton.clicked.add( d );

        wl.setAnchoredBottom( 40, 30 );
        _mainWindow.addWidget( &_stepButton, wl );
        Delegate<void ()> d1( this, &RGBDVOApp::toggleStepping );
        _stepButton.clicked.add( d1 );

        wl.setAnchoredBottom( 75, 30 );
        _mainWindow.addWidget( &_optimizeButton, wl );
        Delegate<void ()> d2( this, &RGBDVOApp::optimizePressed );
        _optimizeButton.clicked.add( d2 );

        _mainWindow.setVisible( true );
    }

    void RGBDVOApp::addNewKeyframe( const Image& gray, const Image& depth, const Matrix4f& kfPose )
    {
        Time t;

#ifdef MULTISCALE
        MultiscaleKeyframe* kf = new MultiscaleKeyframe( kfPose, _aligner.intrinsics(), gray, depth, 5000.0f, 3, 0.5f );
#else
        VOKeyframe* kf = new VOKeyframe( gray, depth, kfPose, _aligner.intrinsics(), 5000.0f );
#endif
        std::cout << "Keyframe creation took: " << t.elapsedMilliSeconds() << "ms" << std::endl;

        _keyframes.push_back( kf );
        _activeKeyframe = _keyframes.back();

        _keyframeImage.setImage( gray );

        // reset the relative pose
        SE3<float>::MatrixType I = SE3<float>::MatrixType::Identity();
        _relativePose.set( I );
        _absolutePose = kfPose;

        _poseView.addKeyframe( kfPose );
        _poseView.setCamPose( _absolutePose );
    }

    void RGBDVOApp::nextPressed()
    {
        std::cout << "click" << std::endl;
        _nextPressed = true;
    }

    void RGBDVOApp::optimizePressed()
    {
        _optimize = !_optimize;
    }

    void RGBDVOApp::toggleStepping()
    {
        _step = !_step;
    }

}
