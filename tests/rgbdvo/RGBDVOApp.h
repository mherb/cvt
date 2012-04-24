#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/TimeoutHandler.h>
#include <cvt/gui/Button.h>
#include <cvt/io/RGBDParser.h>
#include <VOKeyframe.h>
#include <ESMKeyframe.h>
#include <MultiscaleKeyframe.h>
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
                static const float DepthScale = 5000.0f / 1.031f;
                RGBDParser                  _parser;
				Matrix3f					_K;

#ifdef MULTISCALE
                std::vector<MultiscaleKeyframe*> _keyframes;
                RGBDScaleSpaceAlignment          _aligner;
                MultiscaleKeyframe*              _activeKeyframe;
#else
                //std::vector<VOKeyframe*>    _keyframes;
                //RGBDAlignment               _aligner;
                //VOKeyframe*                 _activeKeyframe;
                std::vector<ESMKeyframe*>    _keyframes;
                ESMKeyframe*                 _activeKeyframe;
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

                //bool needNewKeyframe( const Matrix4f& relativePose, const RGBDAlignment::Result& alignResult ) const;
                bool needNewKeyframe( const Matrix4f& relativePose, const ESMKeyframe::Result& alignResult ) const;
                void addNewKeyframe( const Image& gray, const Image& depth, const Matrix4f& kfPose );

                void nextPressed();
                void optimizePressed();
                void toggleStepping();
    };
}
