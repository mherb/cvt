#ifndef CVT_VOTHREAD_H
#define CVT_VOTHREAD_H

#include <cvt/util/Thread.h>
#include <cvt/util/Mutex.h>
#include <cvt/util/Time.h>
#include <cvt/util/Signal.h>
#include <cvt/vision/rgbdvo/RGBDVisualOdometry.h>
#include <cvt/io/RGBDParser.h>
#include <cvt/math/Matrix.h>

namespace cvt {
    struct StepResult {
        float       avgSSDError;
        size_t      numPixels;
        float       pixelPercentage;
        float       avgSpeed;
        Matrix4f    pose;
        double      stamp;
    };

    struct KeyframeData {
        Image    rgb;
        Image    depth;
        Matrix4f pose;
    };

    template <class KFType, class LossFunc>
    class VOThread : public Thread<void>
    {
        public:
            typedef RGBDVisualOdometry<KFType, LossFunc>    VOType;
            typedef typename KFType::WarpType               WarpType;

            VOThread( VOType* vo, const String& folder );
            ~VOThread(){}
            void execute( void* );

            const Matrix4f& cameraPose() const { return _vo->pose(); }
            const Matrix4f& referencePose() const { return _vo->activeKeyframePose(); }

            Signal<const Image&>        currentColor;
            Signal<const Image&>        currentDepth;
            Signal<const Matrix4f&>     gtPose;
            Signal<const StepResult&>   resultSignal;
            Signal<const KeyframeData&> keyframeSignal;

            void setNextPressed() { setNextPressedSave( true ); }
            void toggleStepping() { _stepping = !_stepping; }
            void toggleOptimize() { _optimize = !_optimize; }
            bool isOptimizing() const { return _optimize; }
            bool isStepping()   const { return _stepping; }

        private:
            RGBDParser          _parser;
            VOType*             _vo;

            float               _cumulativeAlignmentSpeed;
            size_t              _numAlignments;

            KeyframeData        _kfdata;
            Image               _gray;
            Matrix4f            _pose;
            bool                _nextPressed;
            bool                _stepping;
            bool                _optimize;

            mutable Mutex       _buttonMutex;

            void preprocessGrayImage( Image& pp, const Image& gray ) const;

            bool nextPressed() const
            {
                ScopeLock l( &_buttonMutex );
                return _nextPressed;
            }

            void setNextPressedSave( bool v )
            {
                ScopeLock l( &_buttonMutex );
                _nextPressed = v;
            }

            void keyframeUpdateCb( const Matrix4f& pose )
            {
                _kfdata.pose = pose;
                keyframeSignal.notify( _kfdata );
            }

    };

    template <class KF, class LF>
    inline VOThread<KF, LF>::VOThread( VOType* vo, const String& folder ) :
        _parser( folder, 0.02f ),
        _vo( vo ),
        _cumulativeAlignmentSpeed( 0.0f ),
        _numAlignments( 0 ),
        _nextPressed( false ),
        _stepping( false ),
        _optimize( true )
    {
        Delegate<void (const Matrix4f&)> d( this, &VOThread<KF, LF>::keyframeUpdateCb );
        _vo->keyframeAdded.add( d );
    }

    template <class KFType, class LossFunc>
    inline void VOThread<KFType, LossFunc>::preprocessGrayImage( Image& pp, const Image& gray ) const
    {
        gray.convolve( pp, IKernel::GAUSS_HORIZONTAL_3, IKernel::GAUSS_VERTICAL_3 );
        //gray.convolve( pp, IKernel::GAUSS_HORIZONTAL_5, IKernel::GAUSS_VERTICAL_5 );
        //gray.convolve( pp, IKernel::GAUSS_HORIZONTAL_7, IKernel::GAUSS_VERTICAL_7 );
        //demeanNormalize( pp, gray );
    }

    template <class KFType, class LossFunc>
    inline void VOThread<KFType, LossFunc>::execute( void* )
    {
        _parser.loadNext();
        while( _parser.data().poseValid == false )
            _parser.loadNext();

        _parser.data().rgb.convert( _kfdata.rgb, IFormat::RGBA_FLOAT );
        _kfdata.rgb.convert( _gray, IFormat::GRAY_FLOAT );
        _parser.data().depth.convert( _kfdata.depth, IFormat::GRAY_FLOAT );
        _pose = _parser.data().template pose<float>();


        // preprocess the gray image
        Image tmp;
        tmp.reallocate( _gray );
        preprocessGrayImage( tmp, _gray );
        _vo->addNewKeyframe( tmp, _kfdata.depth, _pose );

        size_t iter = 0;
        StepResult result;
        while( true ){
            if( nextPressed() || !isStepping() ){
                setNextPressedSave( false );
                iter++;

                if( !_parser.hasNext() ){
                    break;
                }
                _parser.loadNext();
                // current the absolute pose
                const RGBDParser::RGBDSample& d = _parser.data();

                // try to align:
                d.rgb.convert( _kfdata.rgb, IFormat::RGBA_FLOAT );
                _kfdata.rgb.convert( tmp, IFormat::GRAY_FLOAT );
                d.depth.convert( _kfdata.depth, IFormat::GRAY_FLOAT );
                currentColor.notify( _kfdata.rgb );
                currentDepth.notify( _kfdata.depth );

                _gray.reallocate( tmp );
                preprocessGrayImage( _gray, tmp );

                if( d.poseValid ){
                    gtPose.notify( d.pose<float>() );
                }
            }

            if( _optimize ){
                Time t;
                const RGBDParser::RGBDSample& d = _parser.data();
                Matrix4f absPose = _vo->pose();
                _vo->updatePose( absPose, _gray, _kfdata.depth );

                _cumulativeAlignmentSpeed += t.elapsedMilliSeconds();
                _numAlignments++;

                result.avgSpeed = _cumulativeAlignmentSpeed / _numAlignments;
                result.avgSSDError = _vo->lastSSD() / _vo->lastNumPixels();
                result.pixelPercentage = _vo->lastPixelPercentage();
                result.pose = absPose;
                result.stamp = d.stamp;
                resultSignal.notify( result );
            }
        }

    }
}

#endif // VOTHREAD_H
