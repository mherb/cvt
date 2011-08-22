#include <cvt/gui/Application.h>

#ifdef APPLE
#include <cvt/gui/internal/OSX/ApplicationOSX.h>
#else
#include <cvt/gui/internal/X11/ApplicationX11.h>
#endif

namespace cvt {

	Application* Application::_app = NULL;

	Application* Application::instance()
	{
		if( !_app ) {
#ifdef APPLE
			_app = new ApplicationOSX();
#else
			_app = new ApplicationX11();
//#else
//		#error "Platform for Application undefined"
#endif
		}
		return _app;
	}

}
