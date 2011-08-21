#include <cvt/gui/Application.h>

#define _APPX11_
//#define _APPOSX_

#ifdef _APPX11_
#include <cvt/gui/internal/X11/ApplicationX11.h>
#endif

#ifdef _APPOSX_
#include <cvt/gui/internal/OSX/ApplicationOSX.h>
#endif

namespace cvt {

	Application* Application::_app = NULL;

	Application* Application::instance()
	{
		if( !_app ) {
#ifdef _APPX11_
			_app = new ApplicationX11();
#elif defined( _APPOSX_ )
			_app = new ApplicationOSX();
#else
		#error "Platform for Application undefined"
#endif
		}
		return _app;
	}

}
