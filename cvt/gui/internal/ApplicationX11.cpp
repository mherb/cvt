#include <cvt/gui/internal/ApplicationX11.h>
#include <cvt/gui/internal/WidgetImplWinGLX11.h>
#include <cvt/gui/internal/EventThreadX11.h>
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
		std::pair<WidgetImplWinGLX11*, Event*> event;
		EventThreadX11 eventthread( dpy, &events, windows );

		run = true;

		eventthread.run( NULL );
		XFlush( dpy );

		while( run ) {
			event = events.waitNext();
			switch( event.second->type() ) {
				case EVENT_RESIZE:
					{
						WidgetImplWinGLX11* w = event.first;
						int oldwidth = w->rect.width;
						int oldheight = w->rect.height;

						if( oldwidth != ( ( ResizeEvent* ) event.second )->width() || oldheight != ( ( ResizeEvent* ) event.second )->height() ) {
							( ( ResizeEvent* ) event.second )->_oldwidth = oldwidth;
							( ( ResizeEvent* ) event.second )->_oldheight = oldheight;
							w->resizeEvent( ( ResizeEvent* ) event.second );
						}
						delete event.second;
					}
					break;
				case EVENT_MOVE:
					{
						WidgetImplWinGLX11* w = event.first;
						int oldx = w->rect.x;
						int oldy = w->rect.y;

						if( oldx != ( ( MoveEvent* ) event.second )->x()  || oldy != ( ( MoveEvent* ) event.second )->y() ) {
							( ( MoveEvent* ) event.second )->_oldx = oldx;
							( ( MoveEvent* ) event.second )->_oldy = oldy;
							w->moveEvent( ( MoveEvent* ) event.second );
						}
						delete event.second;
					}
					break;
				case EVENT_SHOW:
					{
						event.first->showEvent( ( ShowEvent* ) event.second );
						delete event.second;
					}
					break;
				case EVENT_HIDE:
					{
						event.first->hideEvent( ( HideEvent* ) event.second );
						delete event.second;
					}
					break;
				case EVENT_PAINT:
					{
						event.first->paintEvent( ( PaintEvent* ) event.second );
						delete event.second;
					}
					break;
				case EVENT_MOUSEPRESS:
					{
						event.first->widget->mousePressEvent( ( MousePressEvent* ) event.second );
						delete event.second;
					}
					break;
				case EVENT_MOUSERELEASE:
					{
						event.first->widget->mouseReleaseEvent( ( MouseReleaseEvent* ) event.second );
						delete event.second;
					}
					break;
				case EVENT_MOUSEMOVE:
					{
						event.first->widget->mouseMoveEvent( ( MouseMoveEvent* ) event.second );
						delete event.second;
					}
					break;
				case EVENT_CLOSE:
					{
						( ( Window* ) event.first->widget )->closeEvent( ( CloseEvent* ) event.second );
						delete event.second;
					}
					break;
				default:
					break;
			}

			while( updates.size() ) {
				WidgetImplWinGLX11* w = updates.front();
				updates.pop();
				PaintEvent* pevent = new PaintEvent( 0, 0, 0, 0 );
				w->paintEvent( pevent );
				delete pevent;
			}

		}

		/* FIXME: do cleanup - iterate over windows and delete widget and only widgets */
	}
}

