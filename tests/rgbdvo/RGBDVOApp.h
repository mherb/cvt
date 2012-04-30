#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/TimeoutHandler.h>
#include <cvt/gui/Button.h>
#include <cvt/io/RGBDParser.h>
#include <PoseView.h>

#include <RGBDVisualOdometry.h>
#include <ESMKeyframe.h>
#include <VOKeyframe.h>
#include <AIIKeyframe.h>
#include <RobustKeyframe.h>
#include <MultiscaleKeyframe.h>
#include <RobustWeighting.h>

#include <fstream>

namespace cvt
{
    class RGBDVOApp : public TimeoutHandler
    {
            public:
                RGBDVOApp( const String& folder, const Matrix3f& K, const VOParams& params );
                ~RGBDVOApp();

                void onTimeout();

                void setMaxTranslationDistance( float dist )      { _vo.setMaxTranslationDistance( dist ); }
                void setMaxRotationDistance( float dist )         { _vo.setMaxRotationDistance( dist ); }
                void setMaxSSD( float dist )                      { _vo.setMaxSSD( dist ); }

            private:                
                RGBDParser                          _parser;
                //typedef ESMKeyframe KFType;
                typedef AIIKeyframe KFType;
                //typedef VOKeyframe KFType;
                //typedef MultiscaleKeyframe<ESMKeyframe> KFType;
                //typedef MultiscaleKeyframe<VOKeyframe> KFType;
                RGBDVisualOdometry<KFType>  _vo;
                Vector3f                    _avgTransError;
                size_t                      _validPoseCounter;

                std::ofstream               _fileOut;

                float                       _cumulativeAlignmentSpeed;
                size_t                      _numAlignments;

                // gui stuff
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

                void keyframeAddedCallback( const Matrix4f& pose );
                void activeKeyframeChangedCallback();

                void setupGui();
                void nextPressed();
                void optimizePressed();
                void toggleStepping();

                void writePose( const Matrix4f& pose, double stamp );
    };
}
