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
        _cumulativeAlignmentSpeed( 0.0f ),
        _numAlignments( 0 ),
        _mainWindow( "RGBD-VO" ),
        _kfMov( &_keyframeImage ),
        _imageMov( &_currentImage ),
        _depthViewMov( &_depthView ),
        _poseMov( &_poseView ),
        _nextButton( "next" ),
        _nextPressed( false ),
        _stepButton( "toggle stepping" ),
        _step( false ),
        _optimizeButton( "optimize" ),
        _optimize( true ),
        _ssdLabel( "SSD:" ),
        _numPixelLabel( "# Pixel: 0" ),
        _pixelPercentLabel( "\% Pixel: 0\%" )
    {
        _timerId = Application::registerTimer( 10, this );
        setupGui();

        // observe the vo changes:
        Delegate<void ( const Matrix4f& )> kfAddDel( this, &RGBDVOApp::keyframeAddedCallback );
        Delegate<void ()> actkfChgDel( this, &RGBDVOApp::activeKeyframeChangedCallback );
        _vo.keyframeAdded.add( kfAddDel );
        _vo.activeKeyframeChanged.add( actkfChgDel );

        Image gray, depth;
        Matrix4f initPose;
#ifdef USE_CAM
        _cam.setRegisterDepthToRGB( true );
        _cam.setSyncRGBDepth( true );
        _cam.startCapture();
        _cam.setAntiFlicker( OpenNICamera::OFF );

        _cam.setAutoExposure( false );
        _cam.setBacklightCompensation( false );
        _cam.setGain();

        int skipFrames = 50;
        while( skipFrames-- )
            _cam.nextFrame();

        _cam.frame().convert( gray, IFormat::GRAY_FLOAT );
        _cam.depth().convert( depth, IFormat::GRAY_FLOAT );

        initPose.setIdentity();
#else
        _parser.loadNext();
        while( _parser.data().poseValid == false )
            _parser.loadNext();

        _parser.data().rgb.convert( gray, IFormat::GRAY_FLOAT );
        _parser.data().depth.convert( depth, IFormat::GRAY_FLOAT );

        initPose = _parser.data().pose<float>();
#endif
        // preprocess the gray image
        Image preprocessed;
        preprocessed.reallocate( gray );
        preprocessGrayImage( preprocessed, gray );

        _vo.addNewKeyframe( preprocessed, depth, initPose );

        _avgTransError.setZero();
        _validPoseCounter = 0;

        _fileOut.open( "trajectory.txt" );
    }

    RGBDVOApp::~RGBDVOApp()
    {
        Application::unregisterTimer( _timerId );
        _fileOut.close();
    }

    bool RGBDVOApp::positionJumped( const Matrix4f& currentPose, const Matrix4f& lastPose )
    {

        Vector4f t0 = lastPose.col( 3 );
        Vector4f t1 = currentPose.col( 3 );

        if( ( t0 - t1 ).length() > 0.5f )
            return true;

        return false;
    }

    static void demeanNormalize( Image& pp, const Image& input )
    {
        // compute the two values
        float x, xx;
        x = xx = 0.0f;
        {
            IMapScoped<const float> inMap( input );

            size_t h = input.height();
            while( h-- ){
                const float* ptr = inMap.ptr();
                for( size_t i = 0; i < input.width(); i++ ){
                    x  += ptr[ i ];
                    xx += Math::sqr( ptr[ i ] );
                }
                inMap.nextLine();
            }
            x  /= ( input.width() * input.height() ); // mean
            xx /= ( input.width() * input.height() );
        }

        float stdDev = Math::sqrt( xx - Math::sqr( x ) );

        pp = input;
        pp.sub( x );
        pp.mul( 1.0f / stdDev );

        /*
        x = xx = 0.0f;
        {
            IMapScoped<const float> inMap( pp );

            size_t h = pp.height();
            while( h-- ){
                const float* ptr = inMap.ptr();
                for( size_t i = 0; i < pp.width(); i++ ){
                    x  += ptr[ i ];
                    xx += Math::sqr( ptr[ i ] );
                }
                inMap.nextLine();
            }
            x  /= ( input.width() * input.height() ); // mean
            xx /= ( input.width() * input.height() );
        }

        stdDev = Math::sqrt( xx - Math::sqr( x ) );
        std::cout << "Mean: " << x << " stdDev: " << stdDev << std::endl;
        */
    }

    void RGBDVOApp::preprocessGrayImage( Image& pp, const Image& gray ) const
    {
        //gray.convolve( pp, IKernel::GAUSS_HORIZONTAL_3, IKernel::GAUSS_VERTICAL_3 );
        //gray.convolve( pp, IKernel::GAUSS_HORIZONTAL_5, IKernel::GAUSS_VERTICAL_5 );
        //gray.convolve( pp, IKernel::GAUSS_HORIZONTAL_7, IKernel::GAUSS_VERTICAL_7 );
        demeanNormalize( pp, gray );
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
            lastPose = _vo.pose();
            Time t;

            Image depth, gray, grayf;

            // update the absolute pose
            Matrix4f absPose = _vo.pose();

#ifdef USE_CAM
            _cam.frame().convert( grayf, IFormat::GRAY_FLOAT );
            _cam.depth().convert( depth, IFormat::GRAY_FLOAT );
#else
            const RGBDParser::RGBDSample& d = _parser.data();

            // try to align:
            d.rgb.convert( grayf, IFormat::GRAY_FLOAT );
            d.depth.convert( depth, IFormat::GRAY_FLOAT );
#endif
            gray.reallocate( grayf );
            preprocessGrayImage( gray, grayf );
            _vo.updatePose( absPose, gray, depth );

            _cumulativeAlignmentSpeed += t.elapsedMilliSeconds();
            _numAlignments++;

            _depthView.setImage( depth );

            String str;
            str.sprintf( "SSD: %0.2f", _vo.lastSSD() / _vo.lastNumPixels() );
            _ssdLabel.setLabel( str );
            str.sprintf( "# Pixel: %d", _vo.lastNumPixels() );
            _numPixelLabel.setLabel( str );
            str.sprintf( "\% Pixel: %0.1f\%", _vo.lastPixelPercentage() );
            _pixelPercentLabel.setLabel( str );


            String title;
            title.sprintf( "RGBDVO: Avg. Speed %0.1f ms", _cumulativeAlignmentSpeed / _numAlignments );
            _mainWindow.setTitle( title );

            if( positionJumped( absPose, lastPose) ){
                std::cout << "Position Jump at iteration: " << iter << std::endl;
                _step = true;
                _optimize = false;
                _vo.setPose( lastPose );
            }

#ifndef USE_CAM
            Matrix4f gtPose = d.pose<float>();
            if( d.poseValid ){
                _avgTransError.x += Math::abs( absPose[ 0 ][ 3 ] - gtPose[ 0 ][ 3 ] );
                _avgTransError.y += Math::abs( absPose[ 1 ][ 3 ] - gtPose[ 1 ][ 3 ] );
                _avgTransError.z += Math::abs( absPose[ 2 ][ 3 ] - gtPose[ 2 ][ 3 ] );
                _validPoseCounter++;
            }
            writePose( _fileOut, absPose, d.stamp );
            _poseView.setGTPose( gtPose );
#endif

            _poseView.setCamPose( absPose );
        }
    }

    void RGBDVOApp::writePose( std::ofstream &file, const Matrix4f &pose, double stamp )
    {
        Quaterniond q( ( Matrix3d )pose.toMatrix3() );

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
        _mainWindow.addWidget( &_numPixelLabel, wl );
        wl.setAnchoredBottom( 180, 30 );
        _mainWindow.addWidget( &_pixelPercentLabel, wl );

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
#ifdef USE_CAM
        _keyframeImage.setImage( _cam.frame() );
#else
        _keyframeImage.setImage( _parser.data().rgb );
#endif
    }

}
