#include <cvt/gui/Window.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/Label.h>
#include <cvt/gui/TimeoutHandler.h>
#include <cvt/gui/Button.h>
#include <cvt/io/RGBDParser.h>
#include <PoseView.h>


#include <cvt/vision/rgbdvo/KeyframeBase.h>

#include <cvt/vision/rgbdvo/RGBDKeyframe.h>
#include <cvt/vision/rgbdvo/IntensityKeyframe.h>
#include <cvt/vision/rgbdvo/IntensityDepthKeyframe.h>
#include <cvt/vision/rgbdvo/MIKeyframe.h>
#include <cvt/vision/rgbdvo/VOKeyframe.h>
#include <cvt/vision/rgbdvo/AIIKeyframe.h>
#include <cvt/vision/rgbdvo/RobustAIIKeyframe.h>
#include <cvt/vision/rgbdvo/RobustKeyframe.h>
#include <cvt/vision/rgbdvo/MultiscaleKeyframe.h>
#include <cvt/vision/rgbdvo/RobustWeighting.h>
#include <cvt/vision/rgbdvo/RGBDVisualOdometry.h>
#include <DirectFeatureVO.h>

//#define USE_CAM
#ifdef USE_CAM
#include <cvt/io/OpenNICamera.h>
#endif

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
                void setMinPixelPercentage( float v )             { _vo.setMinPixelPercentage( v ); }
                void setSelectionPixelPercentage( float v )       { _vo.setSelectionPixelPercentage( v ); }

            private:
#ifdef USE_CAM
                OpenNICamera						_cam;
#else
                RGBDParser                          _parser;
#endif

                typedef StandardWarp<float>         WarpType;
                //typedef AffineLightingWarp<float>   WarpType;

                //typedef NoWeighting<float>          LossFunc;
                typedef Huber<float>                LossFunc;
                //typedef Tukey<float>                LossFunc;

                typedef IntensityKeyframe<WarpType>     KFType;
                //typedef IntensityDepthKeyframe<WarpType>  KFType;
                //RGBDVisualOdometry<KFType, LossFunc>    _vo;
                DirectFeatureVO                     _vo;

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

                ImageView                   _depthView;
                Moveable                    _depthViewMov;

                PoseView                    _poseView;
                Moveable                    _poseMov;
                Button                      _nextButton;

                bool                        _nextPressed;
                Button                      _stepButton;
                bool                        _step;
                Button                      _optimizeButton;
                bool                        _optimize;

                Label                       _ssdLabel;
                Label                       _numPixelLabel;
                Label                       _pixelPercentLabel;

                float                       _lastTError;

                void keyframeAddedCallback( const Matrix4f& pose );
                void activeKeyframeChangedCallback();

                void setupGui();
                void nextPressed();
                void optimizePressed();
                void toggleStepping();

                void writePose( std::ofstream& file, const Matrix4f& pose, double stamp );

                bool positionJumped( const Matrix4f& currentPose, const Matrix4f& lastPose );

                void preprocessGrayImage( Image& pp, const Image& gray ) const;
    };
}
