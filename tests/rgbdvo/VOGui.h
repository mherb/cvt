#ifndef VOGUI_H
#define VOGUI_H

#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/Label.h>
#include <cvt/gui/Button.h>
#include <PoseView.h>

namespace cvt {

    class VOGui
    {
        public:
            VOGui();

            void setStepping( bool val );
            void setOptimize( bool val );
            void setOffsetPose( const Matrix4f& pose );

            void setCurrentRGB( const Image& rgb );
            void setCurrentDepth( const Image& depth );
            void setCurrentKeyframe( const Image& img );

            void setPose( const Matrix4f& pose );
            void setGroundTruthPose( const Matrix4f& pose );

            void setSSDLabel( float ssd );
            void setNumPixels( size_t n );
            void setPixelPercentage( float val );
            void setSpeed( float speed );

            void addKeyframe( const Matrix4f& pose );

            Button          _nextButton;
            Button          _stepButton;
            Button          _optimizeButton;
        private:
            Window                      _mainWindow;
            ImageView                   _keyframeImage;
            Moveable                    _kfMov;
            ImageView                   _currentImage;
            Moveable                    _imageMov;

            ImageView                   _depthView;
            Moveable                    _depthViewMov;

            PoseView                    _poseView;
            Moveable                    _poseMov;

            Label                       _ssdLabel;
            Label                       _numPixelLabel;
            Label                       _pixelPercentLabel;

            void setupGui();


    };

}

#endif // VOGUI_H
