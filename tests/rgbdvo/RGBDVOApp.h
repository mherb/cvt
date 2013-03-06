#ifndef CVT_RGBDVOAPP_H
#define CVT_RGBDVOAPP_H

#include <cvt/gui/Application.h>
#include <cvt/gui/TimeoutHandler.h>
#include <cvt/io/RGBDParser.h>

#include <cvt/vision/rgbdvo/RGBDKeyframe.h>
#include <cvt/vision/rgbdvo/RobustWeighting.h>
#include <cvt/vision/rgbdvo/RGBDVisualOdometry.h>
#include <cvt/util/ConfigFile.h>
#include <cvt/util/EigenBridge.h>
#include <cvt/util/Delegate.h>
#include <cvt/util/Time.h>

#include <VOGui.h>

//#define USE_CAM
#ifdef USE_CAM
#include <cvt/io/OpenNICamera.h>
#endif

#include <fstream>

namespace cvt
{
    template <class KFType, class LossFunc>
    class RGBDVOApp : public TimeoutHandler
    {
            typedef RGBDVisualOdometry<KFType, LossFunc>    VOType;
            public:
                RGBDVOApp( VOType* vo, const String& folder );
                ~RGBDVOApp();

                void onTimeout();

            private:
#ifdef USE_CAM
                OpenNICamera						_cam;
#else
                RGBDParser                          _parser;
#endif
                VOGui                       _gui;
                uint32_t                    _timerId;

                VOType*                     _vo;

                Vector3f                    _avgTransError;
                size_t                      _validPoseCounter;

                std::ofstream               _fileOut;

                float                       _cumulativeAlignmentSpeed;
                size_t                      _numAlignments;

                // gui stuff
                bool                        _nextPressed;
                bool                        _step;
                bool                        _optimize;

                float                       _lastTError;

                void setupConnections();
                void keyframeAddedCallback( const Matrix4f& pose );

                void nextPressed();
                void setStepping( bool val );
                void setOptimize( bool val );
                void toggleStepping() { setStepping( !_step ); }
                void toggleOptimize() { setOptimize( !_optimize ); }

                void writePose( std::ofstream& file, const Matrix4f& pose, double stamp );

                bool positionJumped( const Matrix4f& currentPose, const Matrix4f& lastPose );

                void preprocessGrayImage( Image& pp, const Image& gray ) const;

                // DEBUG STUFF
                Matrix4f    _keyframeGTPose;
                Matrix4f    _activeKFPose;
                Image       _activeKFRGB;
                Image       _activeKFDepth;
                size_t      _activeKFIdx;
    };

    template <class KFType, class LossFunc>
    inline RGBDVOApp<KFType, LossFunc>::RGBDVOApp( VOType* vo, const String& folder ) :
#ifdef USE_CAM
        _cam( 0, CameraMode( 640, 480, 30, IFormat::UYVY_UINT8 ) ),
#else
        _parser( folder, 0.02f ),
#endif
        _vo( vo ),
        _cumulativeAlignmentSpeed( 0.0f ),
        _numAlignments( 0 ),
        _nextPressed( false ),
        _step( false ),
        _optimize( true ),
        _lastTError( 0.0f )
    {
        setupConnections();
        _timerId = Application::registerTimer( 10, this );

        // observe the vo changes:
        Delegate<void ( const Matrix4f& )> kfAddDel( this, &RGBDVOApp<KFType, LossFunc>::keyframeAddedCallback );
        _vo->keyframeAdded.add( kfAddDel );

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

        initPose = _parser.data().template pose<float>();
        _gui.setOffsetPose( initPose );
#endif
        // preprocess the gray image
        Image preprocessed;
        preprocessed.reallocate( gray );
        preprocessGrayImage( preprocessed, gray );

        _vo->addNewKeyframe( preprocessed, depth, initPose );

        _avgTransError.setZero();
        _validPoseCounter = 0;

        _fileOut.open( "trajectory.txt" );
    }

    template <class KFType, class LossFunc>
    inline RGBDVOApp<KFType, LossFunc>::~RGBDVOApp()
    {
        Application::unregisterTimer( _timerId );
        _fileOut.close();
    }

    template <class KFType, class LossFunc>
    inline bool RGBDVOApp<KFType, LossFunc>::positionJumped( const Matrix4f& currentPose, const Matrix4f& lastPose )
    {
        Vector4f t0 = lastPose.col( 3 );
        Vector4f t1 = currentPose.col( 3 );

        if( ( t0 - t1 ).length() > 0.5f )
            return true;

        return false;
    }

    template <class KFType, class LossFunc>
    inline void RGBDVOApp<KFType, LossFunc>::preprocessGrayImage( Image& pp, const Image& gray ) const
    {
        gray.convolve( pp, IKernel::GAUSS_HORIZONTAL_3, IKernel::GAUSS_VERTICAL_3 );
        //gray.convolve( pp, IKernel::GAUSS_HORIZONTAL_5, IKernel::GAUSS_VERTICAL_5 );
        //gray.convolve( pp, IKernel::GAUSS_HORIZONTAL_7, IKernel::GAUSS_VERTICAL_7 );
        //demeanNormalize( pp, gray );
    }

    template <class KFType, class LossFunc>
    inline void RGBDVOApp<KFType, LossFunc>::onTimeout()
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
            _gui.setCurrentRGB( _parser.data().rgb );
#endif
            _nextPressed = false;
        }

        if( _optimize ){
            Matrix4f lastPose;
            lastPose = _vo->pose();
            Time t;

            Image depth, gray, grayf;

            // current the absolute pose
            Matrix4f absPose = _vo->pose();

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

            Matrix4f startRelative = _activeKFPose.inverse() * absPose;
            Matrix4f gtPose = d.pose<float>();
            Matrix4f gtRel = _keyframeGTPose.inverse() * gtPose;
            size_t activeIdx = _activeKFIdx;

            _vo->updatePose( absPose, gray, depth );

            _cumulativeAlignmentSpeed += t.elapsedMilliSeconds();
            _numAlignments++;

            _gui.setCurrentDepth( depth );

            _gui.setSSDLabel( _vo->lastSSD() / _vo->lastNumPixels() );
            _gui.setNumPixels( _vo->lastNumPixels() );
            _gui.setPixelPercentage( _vo->lastPixelPercentage() );
            _gui.setSpeed( _cumulativeAlignmentSpeed / _numAlignments );

            if( positionJumped( absPose, lastPose) ){
                std::cout << "Position Jump at iteration: " << iter << std::endl;
            }


#ifndef USE_CAM
            if( d.poseValid ){
                Vector4f eps = absPose.col( 3 ) - gtPose.col( 3 );
                _avgTransError.x += Math::abs( eps.x );
                _avgTransError.y += Math::abs( eps.y );
                _avgTransError.z += Math::abs( eps.z );

                float currError = eps.length();
                float errorChange = currError - _lastTError;
                if( errorChange > 0.04 ){
                    // more than x m change in error
                    std::fixed( std::cout );
                    std::cout << "Stamp: " << d.stamp << ", dataIdx: " << _parser.iter() << " ERROR CHANGE: " << errorChange << std::endl;
                    std::cout << "referenceRGB = " << _parser.rgbFile( _parser.iter() ) << std::endl;
                    std::cout << "referenceDepth = " << _parser.depthFile( _parser.iter() ) << std::endl;
                    std::cout << "currentRGB = " << _parser.rgbFile( activeIdx ) << std::endl;
                    std::cout << "currentDepth = " << _parser.depthFile( activeIdx ) << std::endl;

                    Quaternionf qTrue( gtRel.toMatrix3() );
                    Quaternionf qStart( startRelative.toMatrix3() );
                    std::cout << "gt_q_w = " << qTrue.w << std::endl;
                    std::cout << "gt_q_x = " << qTrue.x << std::endl;
                    std::cout << "gt_q_y = " << qTrue.y << std::endl;
                    std::cout << "gt_q_z = " << qTrue.z << std::endl;
                    std::cout << "gt_t_x = " << gtRel[ 0 ][ 3 ] << std::endl;
                    std::cout << "gt_t_y = " << gtRel[ 1 ][ 3 ] << std::endl;
                    std::cout << "gt_t_z = " << gtRel[ 2 ][ 3 ] << std::endl;

                    std::cout << "start_q_w = " << qStart.w << std::endl;
                    std::cout << "start_q_x = " << qStart.x << std::endl;
                    std::cout << "start_q_y = " << qStart.y << std::endl;
                    std::cout << "start_q_z = " << qStart.z << std::endl;
                    std::cout << "start_t_x = " << startRelative[ 0 ][ 3 ] << std::endl;
                    std::cout << "start_t_y = " << startRelative[ 1 ][ 3 ] << std::endl;
                    std::cout << "start_t_z = " << startRelative[ 2 ][ 3 ] << std::endl;

                    setStepping( true );
                    setOptimize( false );
                }
                _lastTError = currError;

                _validPoseCounter++;
            }
            writePose( _fileOut, absPose, d.stamp );
            _gui.setGroundTruthPose( gtPose );
#endif
            _gui.setPose( absPose );
        }
    }

    template <class KFType, class LossFunc>
    inline void RGBDVOApp<KFType, LossFunc>::writePose( std::ofstream &file, const Matrix4f &pose, double stamp )
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

    template <class KFType, class LossFunc>
    inline void RGBDVOApp<KFType, LossFunc>::setOptimize( bool val )
    {
        _optimize = val;
        _gui.setOptimize( val );
    }

    template <class KFType, class LossFunc>
    inline void RGBDVOApp<KFType, LossFunc>::setStepping( bool val )
    {
        _step = val;
        _gui.setStepping( val );
    }

    template <class KFType, class LossFunc>
    inline void RGBDVOApp<KFType, LossFunc>::nextPressed()
    {
        std::cout << "click" << std::endl;
        _nextPressed = true;
    }

    template <class KFType, class LossFunc>
    inline void RGBDVOApp<KFType, LossFunc>::keyframeAddedCallback( const Matrix4f& pose )
    {
        _gui.addKeyframe( pose );

        // remember the keyframe ground truth pose!
        _keyframeGTPose = _parser.data().template pose<float>();
        _activeKFPose = pose;
        _activeKFRGB.reallocate( _parser.data().rgb );
        _activeKFDepth.reallocate( _parser.data().depth );
        _activeKFRGB   = _parser.data().rgb;
        _activeKFDepth = _parser.data().depth;
        _activeKFIdx = _parser.iter();
    }

    template <class KFType, class LossFunc>
    inline void RGBDVOApp<KFType, LossFunc>::setupConnections()
    {
        Delegate<void ()> dn( this, &RGBDVOApp<KFType, LossFunc>::nextPressed );
        _gui._nextButton.clicked.add( dn );
        Delegate<void ()> ds( this, &RGBDVOApp<KFType, LossFunc>::toggleStepping );
        _gui._stepButton.clicked.add( ds );
        Delegate<void ()> opt( this, &RGBDVOApp<KFType, LossFunc>::toggleOptimize );
        _gui._optimizeButton.clicked.add( opt );
    }

}

#endif
