#include <cvt/gui/internal/X11Handler.h>

namespace cvt {

	void X11Handler::handleEvents()
	{
		XEvent xevent;
		WidgetImplWinGLX11* win;

		XNextEvent( _dpy, &xevent );
		switch( xevent.type ) {
			case ConfigureNotify:
				{
					win = (*_windows)[ xevent.xconfigure.window ];

					int oldwidth = win->_rect.width;
					int oldheight = win->_rect.height;
					int nx = 0, ny = 0, npos = 0;

					if( xevent.xconfigure.send_event ) {
						nx = xevent.xconfigure.x;
						ny = xevent.xconfigure.y;
						npos = 1;
					}

					while( XCheckTypedWindowEvent( _dpy, xevent.xconfigure.window, ConfigureNotify, &xevent ) ) {
						if( xevent.xconfigure.send_event ) {
							nx = xevent.xconfigure.x;
							ny = xevent.xconfigure.y;
							npos = 1;
						}
					}

					if( oldwidth != xevent.xconfigure.width || oldheight != xevent.xconfigure.height ) {
						ResizeEvent re( xevent.xconfigure.width, xevent.xconfigure.height, oldwidth, oldheight );
						win->resizeEvent( &re );
					}
					if( npos ) {
						int oldx = win->_rect.x;
						int oldy = win->_rect.y;

						if( oldx != nx  || oldy != ny ) {
							MoveEvent me( nx, ny, oldx, oldy );
							win->moveEvent( &me );
						}
					}
				}
				break;
			case ReparentNotify:
				{
					win = (*_windows)[ xevent.xreparent.window ];
					::Window child;
					int gx, gy;
					int oldx = win->_rect.x;
					int oldy = win->_rect.y;

					XTranslateCoordinates( _dpy, xevent.xreparent.window, RootWindow( _dpy, DefaultScreen( _dpy ) ), 0, 0, &gx, &gy, &child );
					if( oldx != gx  || oldy != gy ) {
						MoveEvent me( gx, gy, 0, 0 );
						win->moveEvent( &me );
					}
				}
				break;
			case MapNotify:
				{
					ShowEvent se;

					win = (*_windows)[ xevent.xmap.window ];
					win->showEvent( &se );
				}
				break;
			case UnmapNotify:
				{
					HideEvent he;

					win = (*_windows)[ xevent.xmap.window ];
					win->hideEvent( &he );
				}
				break;
			case Expose:
				{
					win = (*_windows)[ xevent.xexpose.window ];

					// Compress resize/motions before sending expose events
					// FIXME
					int cfevent = 0;
					XEvent xevent2;
					int nx = 0, ny = 0, npos = 0;

					while( XCheckTypedWindowEvent( _dpy, xevent.xexpose.window, ConfigureNotify, &xevent2 ) ) {
						if( xevent2.xconfigure.send_event ) {
							nx = xevent2.xconfigure.x;
							ny = xevent2.xconfigure.y;
							npos = 1;
						}
						cfevent = 1;
					}

					if( cfevent ) {
						int oldwidth = win->_rect.width;
						int oldheight = win->_rect.height;

						if( oldwidth != xevent2.xconfigure.width || oldheight != xevent2.xconfigure.height ) {
							ResizeEvent re( xevent2.xconfigure.width, xevent2.xconfigure.height, oldwidth, oldheight );
							win->resizeEvent( &re );
						}
					}
					if( npos ) {
						int oldx = win->_rect.x;
						int oldy = win->_rect.y;

						if( oldx != nx  || oldy != ny ) {
							MoveEvent me( nx, ny, oldx, oldy );
							win->moveEvent( &me );
						}
					}

					// Compress expose
					// FIXME: fix the rect information
					while( XCheckTypedWindowEvent( _dpy, xevent.xexpose.window, Expose, &xevent ) )
						;
					win->update();
					//						PaintEvent pe( xevent.xexpose.x, xevent.xexpose.y, xevent.xexpose.width, xevent.xexpose.height );
					//						win->paintEvent( &pe );
				}
				break;
			case ButtonPress:
				{
					win = ( *_windows )[ xevent.xbutton.window ];
					MousePressEvent mp( xevent.xbutton.x, xevent.xbutton.y, xevent.xbutton.button );
					( ( Window* ) win->_widget )->mousePressEvent( &mp );
				}
				break;
			case ButtonRelease:
				{
					win = ( *_windows )[ xevent.xbutton.window ];
					MouseReleaseEvent mr( xevent.xbutton.x, xevent.xbutton.y, xevent.xbutton.button );
					( ( Window* ) win->_widget )->mouseReleaseEvent( &mr );
				}
				break;
			case MotionNotify:
				{
					win = ( *_windows )[ xevent.xmotion.window ];
					while( XCheckTypedWindowEvent( _dpy, xevent.xmotion.window, MotionNotify, &xevent ) )
						;
					MouseMoveEvent mme( xevent.xmotion.x, xevent.xmotion.y, ( xevent.xmotion.state >> 8 ) & 0x1F );
					( ( Window* ) win->_widget )->mouseMoveEvent( &mme );
				}
				break;
			case ClientMessage:
				{
					if( xevent.xclient.message_type == xatom_wmproto && ( ::Atom ) xevent.xclient.data.l[0] == xatom_wmdelete ) {
						CloseEvent ce;
						win = ( *_windows )[ xevent.xclient.window ];
						( ( Window* ) win->_widget )->closeEvent( &ce );
					}
				}
				break;
			default:
				break;
		}
	}


	void X11Handler::handleQueued()
	{
		while( XEventsQueued( _dpy, QueuedAlready ) )
			handleEvents();
	}

	void X11Handler::onDataReadable()
	{
		if ( !XEventsQueued( _dpy, QueuedAfterFlush) )
			XSync( _dpy, 0 );
		while( XPending( _dpy ) )
			handleEvents();
	}

}
