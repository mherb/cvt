#include <cvt/gui/Application.h>
#include <cvt/util/PluginManager.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/util/SIMD.h>

#if defined( APPLE ) && !defined( APPLE_X11 )
#include <cvt/gui/internal/OSX/ApplicationOSX.h>
#else
#include <cvt/gui/internal/X11/ApplicationX11.h>
#endif

namespace cvt {

	Application* Application::_app = NULL;

	Application* Application::instance()
	{
		if( !_app ) {
#if defined( APPLE ) && !defined( APPLE_X11 )
			_app = new ApplicationOSX();
#else
			_app = new ApplicationX11();
//#else
//		#error "Platform for Application undefined"
#endif
		}
		return _app;
	}

	void Application::atexit()
	{
		PluginManager::cleanup();
		CL::cleanup();
		SIMD::cleanup();
		delete _app;
	}
}
