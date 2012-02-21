/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/gui/Application.h>
#include <cvt/util/PluginManager.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/util/SIMD.h>

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

	void Application::atexit()
	{
		PluginManager::cleanup();
		CL::cleanup();
		SIMD::cleanup();
		delete _app;
	}
}
