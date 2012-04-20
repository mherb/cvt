#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/TimeoutHandler.h>
#include <cvt/gui/Button.h>
#include <cvt/io/RGBDParser.h>
#include <VOKeyframe.h>
#include <MultiscaleKeyframe.h>
#include <RGBDAlignment.h>
#include <RGBDScaleSpaceAlignment.h>
#include <PoseView.h>

#include <vector>

//#define MULTISCALE

namespace cvt
{
    class RGBDVOApp : public TimeoutHandler
    {
            public:
                RGBDVOApp( const String& folder, const Matrix3f& K );
                ~RGBDVOApp();

                void onTimeout();

            private:
                RGBDParser                  _parser;

#ifdef MULTISCALE
                std::vector<MultiscaleKeyframe*> _keyframes;
                RGBDScaleSpaceAlignment          _aligner;
                MultiscaleKeyframe*              _activeKeyframe;
#else
                std::vector<VOKeyframe*>    _keyframes;
                RGBDAlignment               _aligner;
                VOKeyframe*                 _activeKeyframe;
#endif

                Window                      _mainWindow;
                uint32_t                    _timerId;

                ImageView                   _keyframeImage;
                Moveable                    _kfMov;
                ImageView                   _currentImage;
                Moveable                    _imageMov;
                PoseView                    _poseView;
                Moveable                    _poseMov;

                Button                      _nextButton;
                bool                        _nextPressed;
                Button                      _stepButton;
                bool                        _step;
                Button                      _optimizeButton;
                bool                        _optimize;

                // pose relative to the keyframe
                SE3<float>                  _relativePose;
                Matrix4f                    _absolutePose;


                void setupGui();

                bool needNewKeyframe( const Matrix4f& relativePose ) const;
                void addNewKeyframe( const Image& gray, const Image& depth, const Matrix4f& kfPose );

                void nextPressed();
                void optimizePressed();
                void toggleStepping();
    };
}
