#include <cvt/gui/internal/EventThreadX11.h>
#include <poll.h>

namespace cvt {

	void EventThreadX11::execute( void* )
	{
		XEvent xevent;
		Event* e;
		struct pollfd pfd;
		bool run = true;

		pfd.fd = ConnectionNumber( dpy );
		pfd.events = POLLIN;

		xatom_wmproto = XInternAtom( dpy , "WM_PROTOCOLS", False);
		xatom_wmdelete = XInternAtom( dpy, "WM_DELETE_WINDOW", False);

		while( run ) {
			poll( &pfd, 1, -1 );
			do {
				XNextEvent( dpy, &xevent );
				switch( xevent.type ) {
					case ConfigureNotify:
						{
							WidgetImplWinGLX11* w = windows[ xevent.xconfigure.window ];
							int gx, gy;
							::Window c;

							e = new ResizeEvent( xevent.xconfigure.width, xevent.xconfigure.height, 0, 0 );
							enqueue( w, e );
							XTranslateCoordinates( dpy, xevent.xconfigure.window, RootWindow( dpy, DefaultScreen( dpy ) ), 0, 0, &gx, &gy, &c );
							e = new MoveEvent( gx, gy, 0, 0 );
							enqueue( w, e );
						}
						break;
					case ReparentNotify:
						break;
					case MapNotify:
						{
							WidgetImplWinGLX11* w = windows[ xevent.xmap.window ];
							e = new ShowEvent();
							enqueue( w, e );
						}
						break;
					case UnmapNotify:
						{
							WidgetImplWinGLX11* w = windows[ xevent.xunmap.window ];
							e = new HideEvent( );
							enqueue( w, e );
						}
						break;
					case Expose:
						{
							if( xevent.xexpose.count == 0 ) {
								WidgetImplWinGLX11* w = windows[ xevent.xexpose.window ];
								e = new PaintEvent( xevent.xexpose.x, xevent.xexpose.y, xevent.xexpose.width, xevent.xexpose.height );
								enqueue( w, e );
							}
						}
						break;
					case ButtonPress:
						{
							WidgetImplWinGLX11* w = windows[ xevent.xbutton.window ];
							e = new MousePressEvent( xevent.xbutton.x, xevent.xbutton.y, xevent.xbutton.button );
							enqueue( w, e );
						}
						break;
					case ButtonRelease:
						{
							WidgetImplWinGLX11* w = windows[ xevent.xbutton.window ];
							e = new MouseReleaseEvent( xevent.xbutton.x, xevent.xbutton.y, xevent.xbutton.button );
							enqueue( w, e );
						}
						break;
					case MotionNotify:
						{
							WidgetImplWinGLX11* w = windows[ xevent.xmotion.window ];
							e = new MouseMoveEvent( xevent.xmotion.x, xevent.xmotion.y );
							enqueue( w, e );
						}
						break;
					case ClientMessage:
						{
							if( xevent.xclient.message_type == xatom_wmproto && ( ::Atom ) xevent.xclient.data.l[0] == xatom_wmdelete ) {
								WidgetImplWinGLX11* w = windows[ xevent.xclient.window ];
								e = new CloseEvent();
								enqueue( w, e );
							}
						}
						break;
					default:
						break;
				}
			} while( XPending( dpy ) );
		}
	}

};
