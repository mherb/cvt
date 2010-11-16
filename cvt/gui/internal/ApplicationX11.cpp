#include <cvt/gui/internal/ApplicationX11.h>
#include <cvt/gui/internal/WidgetImplWinGLX11.h>
#include <cvt/gui/internal/X11Handler.h>
#include <cvt/util/Exception.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/GFXGL.h>


namespace cvt {
	ApplicationX11::ApplicationX11()
	{
		/* FIXME: use GLFBConfig */
		int attrib[] = { GLX_RGBA,
			GLX_RED_SIZE, 8,
			GLX_GREEN_SIZE, 8,
			GLX_BLUE_SIZE, 8,
			GLX_ALPHA_SIZE, 8,
			GLX_DEPTH_SIZE, 8,
			GLX_DOUBLEBUFFER,
			None };

		XInitThreads();
		dpy = ::XOpenDisplay( NULL );
		if( !dpy )
			throw CVTException( "Error: Couldn't connect to X-Server\n" );

		xatom_wmdelete = XInternAtom( dpy, "WM_DELETE_WINDOW", False);

		visinfo = glXChooseVisual( dpy, DefaultScreen( dpy ), attrib );
		if( !visinfo )
			throw CVTException( "Error: couldn't get visual\n" );

		ctx = glXCreateContext( dpy, visinfo, NULL, true );
		if( !ctx )
			throw CVTException( "Error: glXCreateContext failed\n" );

		::XSetWindowAttributes attr;
		unsigned long mask;
		attr.background_pixmap = None;
		attr.border_pixel = 0;
		attr.colormap = ::XCreateColormap( dpy, RootWindow( dpy, DefaultScreen( dpy ) ), visinfo->visual, AllocNone);
		mask = CWBackPixmap | CWBorderPixel | CWColormap;

		::Window xw = ::XCreateWindow( dpy, RootWindow( dpy, DefaultScreen( dpy ) ), 0, 0, 1, 1,
							  0, visinfo->depth, InputOutput, visinfo->visual, mask, &attr );
		glXMakeCurrent( dpy, xw, ctx );

		GL::init();

		XDestroyWindow( dpy, xw );
		glXMakeCurrent( dpy, None, NULL );
	}

	ApplicationX11::~ApplicationX11()
	{
		::XCloseDisplay( dpy );
	}


	WidgetImpl* ApplicationX11::getWidgetImpl( Widget* w )
	{
		WidgetImpl* ret;
		if( !w->getParent() ) {
			WidgetImplWinGLX11* impl = new WidgetImplWinGLX11( dpy, ctx, visinfo, ( Window* ) w, &updates );
			XSetWMProtocols(dpy, impl->win, &xatom_wmdelete, 1);
			windows.insert( std::pair< ::Window, WidgetImplWinGLX11*>( impl->win, impl ) );
			ret = impl;
		} else {
			ret = NULL;//new WidgetImplGL( w );
		}
		return ret;
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
			_ioselect.handleIO( timeout );
			if( !updates.empty() ) {
				PaintEvent pe( 0, 0, 0, 0 );
				WidgetImplWinGLX11* win;

				while( !updates.empty() ) {
					win = updates.front();
					updates.pop_front();
					win->paintEvent( &pe );
				}
			}
		}

		_ioselect.unregisterIOHandler( &x11handler );

		/* FIXME: do cleanup afterwards */
	}
}

