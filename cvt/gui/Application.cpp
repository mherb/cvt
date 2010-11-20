#include <cvt/gui/Application.h>

#define _APPX11_

#ifdef _APPX11_
#include <cvt/gui/internal/ApplicationX11.h>
#endif

namespace cvt {

	Application* Application::_app = NULL;

	Application* Application::instance()
	{
		if( !_app ) {
#ifdef _APPX11_
			_app = new ApplicationX11();
#else
		#error "Platform for Application undefined"
#endif
		}
		return _app;
	}

}
