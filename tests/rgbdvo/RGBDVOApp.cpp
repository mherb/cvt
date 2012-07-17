#include <RGBDVOApp.h>

#include <cvt/gui/Application.h>
#include <cvt/util/EigenBridge.h>
#include <cvt/util/Delegate.h>
#include <cvt/util/Time.h>

namespace cvt
{
    RGBDVOApp::RGBDVOApp( const String& folder, const Matrix3f& K, const VOParams& params) :
#ifdef USE_CAM
        _cam( 0, CameraMode( 640, 480, 30, IFormat::UYVY_UINT8 ) ),
#else
        _parser( folder, 0.02f ),
#endif
        _vo( K, params ),
        _aligner( K ),
        _cumulativeAlignmentSpeed( 0.0f ),
        _numAlignments( 0 ),
        _mainWindow( "RGBD-VO" ),
        _kfMov( &_keyframeImage ),
        _imageMov( &_currentImage ),
        _poseMov( &_poseView ),
        _nextButton( "next" ),
        _nextPressed( false ),
        _stepButton( "toggle stepping" ),
        _step( false ),
        _optimizeButton( "optimize" ),
        _optimize( true ),
        _ssdLabel( "SSD:" ),
        _tdistLabel( "T-Distance:" ),
        _rotDistLabel( "R-Distance:" )
    {
        _timerId = Application::registerTimer( 10, this );
        setupGui();

        // observe the vo changes:
        Delegate<void ( const Matrix4f& )> kfAddDel( this, &RGBDVOApp::keyframeAddedCallback );
        Delegate<void ()> actkfChgDel( this, &RGBDVOApp::activeKeyframeChangedCallback );
        _vo.keyframeAdded.add( kfAddDel );
        _vo.activeKeyframeChanged.add( actkfChgDel );

        Image gray;
#ifdef USE_CAM
        _cam.setRegisterDepthToRGB( true );
        //_cam.setSyncRGBDepth( true );
        _cam.startCapture();
        _cam.nextFrame();

        _cam.frame().convert( gray, IFormat::GRAY_FLOAT );

        Matrix4f tmp; tmp.setIdentity();
        _vo.addNewKeyframe( gray, _cam.depth(), tmp );
#else
        _parser.loadNext();
        while( _parser.data().poseValid == false )
            _parser.loadNext();

        Image dFloat;
        _parser.data().rgb.convert( gray, IFormat::GRAY_FLOAT );
        _parser.data().depth.convert( dFloat, IFormat::GRAY_FLOAT );


        _vo.addNewKeyframe( gray, dFloat, _parser.data().pose );

        _alignerRelativePose = _parser.data().pose;

        _aligner.alignFrames( _alignerRelativePose, gray, dFloat );
#endif

        _avgTransError.setZero();
        _validPoseCounter = 0;

        _fileOut.open( "trajectory.txt" );
        _fileOutFwd.open( "trajectoryFwd.txt" );
    }

    RGBDVOApp::~RGBDVOApp()
    {
        Application::unregisterTimer( _timerId );
        _fileOut.close();
        _fileOutFwd.close();
    }

    bool RGBDVOApp::positionJumped( const Matrix4f& currentPose, const Matrix4f& lastPose )
    {

        Vector4f t0 = lastPose.col( 3 );
        Vector4f t1 = currentPose.col( 3 );

        if( ( t0 - t1 ).length() > 0.5f )
            return true;

        return false;
    }

    void RGBDVOApp::onTimeout()
    {
        static size_t iter = 0;

        if( _nextPressed || !_step ){
            iter++;
#ifdef USE_CAM
            _cam.nextFrame();
            _currentImage.setImage( _cam.frame() );
#else
            if( !_parser.hasNext() ){
                _avgTransError /= _validPoseCounter;
                std::cout << "Mean Translational Error: " << _avgTransError << " Len: " << _avgTransError.length() << std::endl;
                Application::exit();
            }
            _parser.loadNext();
            _currentImage.setImage( _parser.data().rgb );
#endif
            _nextPressed = false;
        }

        if( _optimize ){
            Matrix4f lastPose;
            _vo.pose( lastPose );
            Time t;

            Image depth, gray;

#ifdef USE_CAM
            _cam.frame().convert( gray, IFormat::GRAY_FLOAT );
            _cam.frame().convert( depth, IFormat::GRAY_FLOAT );
            _vo.updatePose( gray, depth );
#else
            const RGBDParser::RGBDSample& d = _parser.data();

            // try to align:
            d.rgb.convert( gray, IFormat::GRAY_FLOAT );
            d.depth.convert( depth, IFormat::GRAY_FLOAT );

            _vo.updatePose( gray, depth );
            _aligner.alignFrames( _alignerRelativePose, gray, depth );
            std::cout << "Forward Compositional: \n" << _alignerRelativePose << std::endl;
#endif

            const VOResult& result = _vo.lastResult();
            String str;
            str.sprintf( "SSD: %0.2f", result.SSD / result.numPixels );
            _ssdLabel.setLabel( str );

            _cumulativeAlignmentSpeed += t.elapsedMilliSeconds();
            _numAlignments++;
            String title;
            title.sprintf( "RGBDVO: Avg. Speed %0.1f ms", _cumulativeAlignmentSpeed / _numAlignments );
            _mainWindow.setTitle( title );

            // update the absolute pose
            Matrix4f absPose;
            _vo.pose( absPose );
            if( positionJumped( absPose, lastPose) ){
                std::cout << "Position Jump at iteration: " << iter << std::endl;
            }

#ifndef USE_CAM
            if( d.poseValid ){
                _avgTransError.x += Math::abs( absPose[ 0 ][ 3 ] - d.pose[ 0 ][ 3 ] );
                _avgTransError.y += Math::abs( absPose[ 1 ][ 3 ] - d.pose[ 1 ][ 3 ] );
                _avgTransError.z += Math::abs( absPose[ 2 ][ 3 ] - d.pose[ 2 ][ 3 ] );
                _validPoseCounter++;
            }
            writePose( _fileOut, absPose, d.stamp );
            writePose( _fileOutFwd, _alignerRelativePose, d.stamp );
            _poseView.setGTPose( d.pose );
#endif

            _poseView.setCamPose( absPose );
        }
    }

    void RGBDVOApp::writePose( std::ofstream &file, const Matrix4f &pose, double stamp )
    {
        Quaternionf q( pose.toMatrix3() );

        file.precision( 15 );
        file << std::fixed << stamp << " "
                 << pose[ 0 ][ 3 ] << " "
                 << pose[ 1 ][ 3 ] << " "
                 << pose[ 2 ][ 3 ] << " "
                 << q.x << " "
                 << q.y << " "
                 << q.z << " "
                 << q.w << std::endl;
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

        wl.setAnchoredBottom( 110, 30 );
        wl.setAnchoredRight( 5, 150 );
        _mainWindow.addWidget( &_ssdLabel, wl );
        wl.setAnchoredBottom( 145, 30 );
        _mainWindow.addWidget( &_tdistLabel, wl );
        wl.setAnchoredBottom( 180, 30 );
        _mainWindow.addWidget( &_rotDistLabel, wl );

        _mainWindow.setVisible( true );
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

    void RGBDVOApp::keyframeAddedCallback( const Matrix4f& pose )
    {
        _poseView.addKeyframe( pose );
    }

    void RGBDVOApp::activeKeyframeChangedCallback()
    {
        // TODO: draw the active keyframe in green in the view
    }

}
