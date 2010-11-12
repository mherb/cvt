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
							int gx, gy;
							::Window c;

							e = new ResizeEvent( xevent.xconfigure.width, xevent.xconfigure.height, 0, 0 );
							enqueue( xevent.xconfigure.window, e );
							XTranslateCoordinates( dpy, xevent.xconfigure.window, RootWindow( dpy, DefaultScreen( dpy ) ), 0, 0, &gx, &gy, &c );
							e = new MoveEvent( gx, gy, 0, 0 );
							enqueue( xevent.xconfigure.window, e );
						}
						break;
					case ReparentNotify:
						break;
					case MapNotify:
						{
							e = new ShowEvent();
							enqueue( xevent.xmap.window, e );
						}
						break;
					case UnmapNotify:
						{
							e = new HideEvent( );
							enqueue( xevent.xunmap.window, e );
						}
						break;
					case Expose:
						{
							if( xevent.xexpose.count == 0 ) {
								e = new PaintEvent( xevent.xexpose.x, xevent.xexpose.y, xevent.xexpose.width, xevent.xexpose.height );
								enqueue( xevent.xexpose.window, e );
							}
						}
						break;
					case ButtonPress:
						{
							e = new MousePressEvent( xevent.xbutton.x, xevent.xbutton.y, xevent.xbutton.button );
							enqueue( xevent.xbutton.window, e );
						}
						break;
					case ButtonRelease:
						{
							e = new MouseReleaseEvent( xevent.xbutton.x, xevent.xbutton.y, xevent.xbutton.button );
							enqueue( xevent.xbutton.window, e );
						}
						break;
					case MotionNotify:
						{
							e = new MouseMoveEvent( xevent.xmotion.x, xevent.xmotion.y );
							enqueue( xevent.xmotion.window, e );
						}
						break;
					case ClientMessage:
						{
							if( xevent.xclient.message_type == xatom_wmproto && ( ::Atom ) xevent.xclient.data.l[0] == xatom_wmdelete ) {
								e = new CloseEvent();
								enqueue( xevent.xclient.window, e );
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
