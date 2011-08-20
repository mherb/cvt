#include <cvt/gui/internal/ApplicationX11.h>
#include <cvt/gui/internal/WidgetImplWinGLX11.h>
#include <cvt/gui/internal/X11Handler.h>
#include <cvt/util/Exception.h>
#include <cvt/gui/Window.h>

#include <cvt/cl/CLPlatform.h>

namespace cvt {
	ApplicationX11::ApplicationX11()
	{
		XInitThreads();

		dpy = ::XOpenDisplay( NULL );
		if( !dpy )
			throw CVTException( "Error: Couldn't connect to X-Server\n" );

		GLFormat format;
		_defaultctx = new GLXContext( dpy, format );

		xatom_wmdelete = ::XInternAtom( dpy, "WM_DELETE_WINDOW", False);

		const ::XVisualInfo* visinfo = _defaultctx->XVisualInfo();
		::XSetWindowAttributes attr;
		attr.background_pixmap = None;
		attr.border_pixel = 0;
		attr.colormap = ::XCreateColormap( dpy, RootWindow( dpy, DefaultScreen( dpy ) ), visinfo->visual, AllocNone);
		unsigned long mask = CWBackPixmap | CWBorderPixel | CWColormap;

		// OpenGL init
		_dummywin = ::XCreateWindow( dpy, RootWindow( dpy, DefaultScreen( dpy ) ), 0, 0, 1, 1,
							  0, visinfo->depth, InputOutput, visinfo->visual, mask, &attr );

		_defaultctx->setDrawable( _dummywin );
		_defaultctx->makeCurrent();
		GL::init();

		// OpenCL init, try to share resources with GL
		bool clinit = false;
		std::vector<CLPlatform> clplatforms;
		CLPlatform::get( clplatforms );
		if( clplatforms.size() ) {
#if defined( cl_khr_gl_sharing )
			_clsupport = true;

			/* try to find platform/device with cl_khr_gl_sharing */
			for( size_t i = 0; i < clplatforms.size(); i++ ) {
				//std::cout << clplatforms[ i ] << std::endl;
				std::vector<CLDevice> devs;
				clplatforms[ i ].devices( devs );
				for( size_t k = 0; k < devs.size(); k++ ) {
					//std::cout << devs[ k ] << std::endl;
					std::vector<String> exts;
					devs[ i ].extensions( exts );
					for( size_t l = 0; l < exts.size(); l++) {
						//std::cout << exts[ l ] << std::endl;
						if( exts[ l ] == "cl_khr_gl_sharing" ) {
							cl_context_properties props[  ] = {
								CL_CONTEXT_PLATFORM,
								( cl_context_properties ) clplatforms[ i ],
								CL_GLX_DISPLAY_KHR, ( cl_context_properties ) dpy,
								NULL
							};

							std::cout << "FOUND GL SHARING" << std::endl;
							if( CL::init( ( cl_device_id ) devs[ k ], props ) ) {
								clinit = true;
								break;
							}
						}
					}
					if( clinit )
						break;
				}
				if( clinit )
					break;
			}
#endif
			if( !clinit )
				std::cout << "CL init failed" << std::endl;
		} else
			_clsupport = false;

		_defaultctx->resetCurrent();
	}

	ApplicationX11::~ApplicationX11()
	{
		delete _defaultctx;
		XDestroyWindow( dpy, _dummywin );
		::XCloseDisplay( dpy );
	}


	WidgetImpl* ApplicationX11::_registerWindow( Widget* w )
	{
		WidgetImpl* ret;
		if( w->isToplevel() ) {
			WidgetImplWinGLX11* impl = new WidgetImplWinGLX11( dpy, _defaultctx, w, &updates );
			XSetWMProtocols(dpy, impl->win, &xatom_wmdelete, 1 );
			windows.insert( std::pair< ::Window, WidgetImplWinGLX11*>( impl->win, impl ) );
			ret = impl;
		} else {
			ret = NULL;
		}
		return ret;
	};

	void ApplicationX11::_unregisterWindow( WidgetImpl* impl )
	{
		    windows.erase( ( ( WidgetImplWinGLX11* ) impl )->win );
	};

	void ApplicationX11::runApp()
	{
		int timeout;
		X11Handler x11handler( dpy, &windows );
		_ioselect.registerIOHandler( &x11handler );

		run = true;

		XSync( dpy, false );

		while( run ) {
			_timers.handleTimers();
			timeout = _timers.nextTimeout();

			x11handler.handleQueued();
			_ioselect.handleIO( timeout );

			if( !updates.empty() ) {
				PaintEvent pe( 0, 0, 0, 0 );
				WidgetImplWinGLX11* win;

				while( !updates.empty() ) {
					win = updates.front();
					updates.pop_front();
					if( win->needsupdate )
						win->paintEvent( &pe );
				}
			}
		}

		_ioselect.unregisterIOHandler( &x11handler );

		/* FIXME: do cleanup afterwards */
	}
}

