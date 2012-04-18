#include <RGBDVOApp.h>

#include <cvt/gui/Application.h>
#include <cvt/util/EigenBridge.h>
#include <cvt/util/Delegate.h>

namespace cvt
{

    RGBDVOApp::RGBDVOApp( const String& folder, const Matrix3f& K ) :
        _parser( folder ),
        _aligner( K, 20, 5000.0f),
        _mainWindow( "RGBD-VO" ),
        _kfMov( &_keyframeImage ),
        _imageMov( &_currentImage ),
        _gxMov( &_gxView ),
        _gyMov( &_gyView ),
        _nextButton( "next" ),
        _nextPressed( false )
    {
        _timerId = Application::registerTimer( 30, this );
        setupGui();

        _parser.loadNext();
        addNewKeyframe( _parser.data(), _parser.data().pose );
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
        if( _nextPressed ){
            _parser.loadNext();
            _nextPressed = false;
        }

        const RGBDParser::RGBDSample& d = _parser.data();
        _currentImage.setImage( d.rgb );

        // try to align:
        _aligner.alignWithKeyFrame( _relativePose, *_activeKeyframe, d.rgb, d.depth );

        // update the absolute pose
        EigenBridge::toCVT( _absolutePose, _relativePose.transformation() );
        _absolutePose *= _activeKeyframe->pose();

        std::cout << "Pose Error:\n" << _absolutePose - d.pose << std::endl;
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

        _mainWindow.addWidget( &_gxMov );
        _gxMov.setSize( 300, 200 );
        _gxMov.setPosition( 300, 0 );
        _gxMov.setTitle( "Gradient X" );

        _mainWindow.addWidget( &_gyMov );
        _gyMov.setSize( 300, 200 );
        _gyMov.setPosition( 300, 0 );
        _gyMov.setTitle( "Gradient Y" );

        WidgetLayout wl;
        wl.setRelativeLeftRight( 0.02f, 0.98f);
        wl.setRelativeTopBottom( 0.02f, 0.98f);
        _mainWindow.addWidget( &_sceneView, wl );

        wl.setAnchoredBottom( 5, 20 );
        wl.setAnchoredRight( 5, 50 );
        _mainWindow.addWidget( &_nextButton, wl );

        Delegate<void ()> d( this, &RGBDVOApp::nextPressed );
        _nextButton.clicked.add( d );

        _mainWindow.setVisible( true );
    }

    void RGBDVOApp::addNewKeyframe( const RGBDParser::RGBDSample& sample, const Matrix4f& kfPose )
    {
        VOKeyframe* kf = new VOKeyframe( sample.rgb, sample.depth, kfPose, _aligner.intrinsics(), 5000.0f );
        _keyframes.push_back( kf );
        _activeKeyframe = _keyframes.back();

        _keyframeImage.setImage( kf->gray() );
        _gxView.setImage( _activeKeyframe->gradX() );
        _gyView.setImage( _activeKeyframe->gradY() );

        // reset the relative pose
        SE3<float>::MatrixType I = SE3<float>::MatrixType::Identity();
        _relativePose.set( I );

        ScenePoints pts( "bla" );
        std::vector<Vector4f> colors;
        const float* c = kf->pixelData();
        for( size_t i = 0; i < kf->numPoints(); i++ ){
            colors.push_back( Vector4f( c[ i ], c[ i ], c[ i ], 1.0f ) );
        }

        pts.setVerticesWithColor( kf->pointsPtr(), &colors[ 0 ], kf->numPoints() );
        _sceneView.setScenePoints( pts );
    }

    void RGBDVOApp::nextPressed()
    {
        _nextPressed = true;
    }

}
