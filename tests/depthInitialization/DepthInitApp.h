#include <cvt/gui/Window.h>
#include <cvt/gui/TimeoutHandler.h>

#include "GLSceneView.h"

namespace cvt
{

    class DepthInitApp : public TimeoutHandler
    {
        public:
            DepthInitApp();

            void onTimeout();

        private:
            GLSceneView _sceneView;
    };

    inline DepthInitApp::DepthInitApp()
    {
    }

    inline void DepthInitApp::onTimeout()
    {
        // TODO
    }

}
