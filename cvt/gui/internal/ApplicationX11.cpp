#include <cvt/gui/internal/ApplicationX11.h>
#include <cvt/gui/internal/WidgetImplWinGLX11.h>
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

		dpy = ::XOpenDisplay( NULL );
		if( !dpy )
			throw CVTException( "Error: Couldn't connect to X-Server\n" );

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

		xatom_wmproto = XInternAtom( dpy , "WM_PROTOCOLS", False);
		xatom_wmdelete = XInternAtom( dpy, "WM_DELETE_WINDOW", False);
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
		::XEvent event;

		run = true;

		while( run ) {
			::XNextEvent( dpy, &event );
			switch( event.type ) {
				case ConfigureNotify:
					{
						WidgetImplWinGLX11* w = windows[ event.xconfigure.window ];
						int oldwidth = w->rect.width;
						int oldheight = w->rect.height;
						int oldx = w->rect.x;
						int oldy = w->rect.y;
						int gx, gy;
						::Window c;

						if( oldwidth != event.xconfigure.width || oldheight != event.xconfigure.height ) {
							ResizeEvent revent( event.xconfigure.width, event.xconfigure.height, oldwidth, oldheight );
							w->resizeEvent( &revent );
						}
						XTranslateCoordinates( dpy, event.xconfigure.window, RootWindow( dpy, DefaultScreen( dpy ) ), 0, 0, &gx, &gy, &c );
						if( oldx != gx || oldy != gy ) {
							MoveEvent mevent( gx, gy, oldx, oldy );
							w->moveEvent( &mevent );
						}
					}
					break;
				case ReparentNotify:
					break;
				case MapNotify:
					{
							WidgetImplWinGLX11* w = windows[ event.xmap.window ];
							ShowEvent sevent;
							w->showEvent( &sevent );
					}
					break;
				case UnmapNotify:
					{
							WidgetImplWinGLX11* w = windows[ event.xunmap.window ];
							HideEvent hevent;
							w->hideEvent( &hevent );
					}
					break;
				case Expose:
					{
//						if( event.xexpose.count == 0 ) {
							WidgetImplWinGLX11* w = windows[ event.xexpose.window ];
							PaintEvent pevent( event.xexpose.x, event.xexpose.y, event.xexpose.width, event.xexpose.height );
							w->paintEvent( &pevent );
//						}
					}
					break;
				case ButtonPress:
					{
						WidgetImplWinGLX11* w = windows[ event.xbutton.window ];
						MousePressEvent mpevent( event.xbutton.x, event.xbutton.y, event.xbutton.button );
						w->widget->mousePressEvent( &mpevent );
					}
					break;
				case ButtonRelease:
					{
						WidgetImplWinGLX11* w = windows[ event.xbutton.window ];
						MouseReleaseEvent mrevent( event.xbutton.x, event.xbutton.y, event.xbutton.button );
						w->widget->mouseReleaseEvent( &mrevent );
					}
					break;
				case MotionNotify:
					{
						WidgetImplWinGLX11* w = windows[ event.xmotion.window ];
						MouseMoveEvent mmevent( event.xmotion.x, event.xmotion.y );
						w->widget->mouseMoveEvent( &mmevent );
					}
					break;
				case ClientMessage:
					{
						if (event.xclient.message_type == xatom_wmproto && ( ::Atom ) event.xclient.data.l[0] == xatom_wmdelete ) {
							WidgetImplWinGLX11* w = windows[ event.xclient.window ];
							CloseEvent cevent;
							( ( Window* ) w->widget )->closeEvent( &cevent );
						}
					}
					break;
				default:
					break;
			}

			while( updates.size() ) {
				WidgetImplWinGLX11* w = updates.front();
				updates.pop();
				PaintEvent* pevent = new PaintEvent( event.xexpose.x, event.xexpose.y, event.xexpose.width, event.xexpose.height );
				w->paintEvent( pevent );
				delete pevent;
			}

		}

		/* FIXME: do cleanup - iteratre over windows and delete widget and only widgets */
	}
}

