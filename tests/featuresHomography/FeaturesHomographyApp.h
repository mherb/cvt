#ifndef CVT_FEATURES_HOMOGRAPHY_APP
#define CVT_FEATURES_HOMOGRAPHY_APP

#include <cvt/io/Camera.h>

#include <cvt/util/Time.h>

#include <cvt/gui/Window.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/TimeoutHandler.h>

namespace cvt {

    class FeaturesHomographyApp : public TimeoutHandler
    {
        public:
            FeaturesHomographyApp( VideoInput* cam,
                                   const cvt::Image& refImage );
            ~FeaturesHomographyApp();

        private:
            VideoInput* _cam;
            uint32_t    _timerId;

            size_t      _iterations;
            Time		_processingTime;

            // gui elements
            Window      _mainWindow;
            ImageView   _trackingView;

            void onTimeout();
            void updateFPS();
            void initGuiElements();

    };
}

#endif
