#ifndef VOGUI_H
#define VOGUI_H

#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/Label.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/TimeoutHandler.h>
#include <cvt/util/Mutex.h>
#include <cvt/vision/CameraCalibration.h>
#include <PoseView.h>
#include <VOThread.h>

namespace cvt {

    class VOGui : public TimeoutHandler
    {
        public:
            VOGui( const Matrix3f& intrinsics, float depthScale );
            ~VOGui();

            void onTimeout();

            void setStepping( bool val );
            void setOptimize( bool val );
            void setOffsetPose( const Matrix4f& pose );

            void setCurrentRGB( const Image& rgb );
            void setCurrentDepth( const Image& depth );
            void setCurrentKeyframe( const Image& img, const Image& depth );

            void setPose(const Matrix4f &data );
            void setGroundTruthPose( const Matrix4f& pose );

            void setSSDLabel( float ssd );
            void setNumPixels( size_t n );
            void setPixelPercentage( float val );
            void setSpeed( float speed );

            void addKeyframe( const KeyframeData &data );

            Button          _nextButton;
            Button          _stepButton;
            Button          _optimizeButton;
        private:
            Window                      _mainWindow;
            ImageView                   _keyframeImage;            
            ImageView                   _currentImage;
            ImageView                   _depthView;            

            PoseView                    _poseView;

            Label                       _ssdLabel;
            Label                       _numPixelLabel;
            Label                       _pixelPercentLabel;
            float                       _ssd;
            size_t                      _numPixel;
            float                       _pixelPercent;
            float                       _avgSpeed;

            Image                       _currentRGB;
            Image                       _currentDepth;

            Matrix4f                    _camPose;
            Matrix4f                    _gtPose;

            uint32_t                    _timerId;

            Mutex                       _dataMutex;
            Mutex                       _keyframeDataMutex;
            std::vector<KeyframeData>   _newKeyframeData;

            CameraCalibration           _camCalib;
            float                       _depthFactor;
            ScenePoints                 _allPoints;
            size_t                      _nKfs;


            void setupGui();
            void processOutstandingKeyframes();


    };

}

#endif // VOGUI_H
