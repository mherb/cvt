#include <cvt/gui/internal/ApplicationX11.h>
#include <cvt/gui/internal/WidgetImplWinGLX11.h>
#include <cvt/gui/internal/EventThreadX11.h>
#include <cvt/gui/TimerInfo.h>
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
			WidgetImplWinGLX11* impl = new WidgetImplWinGLX11( dpy, ctx, visinfo, ( Window* ) w, &events );
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
		std::pair< ::Window, Event*> event;
		EventThreadX11 eventthread( dpy, &events, &timers );

		run = true;

		eventthread.run( NULL );
		XSync( dpy, false );

		while( run ) {
			event = events.waitNext();
			switch( event.second->type() ) {
				case EVENT_RESIZE:
					{
						WidgetImplWinGLX11* w = windows[ event.first ];
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
						WidgetImplWinGLX11* w = windows[ event.first ];
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
						WidgetImplWinGLX11* w = windows[ event.first ];
						w->showEvent( ( ShowEvent* ) event.second );
						delete event.second;
					}
					break;
				case EVENT_HIDE:
					{
						WidgetImplWinGLX11* w = windows[ event.first ];
						w->hideEvent( ( HideEvent* ) event.second );
						delete event.second;
					}
					break;
				case EVENT_PAINT:
					{
						WidgetImplWinGLX11* w = windows[ event.first ];
						w->paintEvent( ( PaintEvent* ) event.second );
						delete event.second;
					}
					break;
				case EVENT_MOUSEPRESS:
					{
						WidgetImplWinGLX11* w = windows[ event.first ];
						w->widget->mousePressEvent( ( MousePressEvent* ) event.second );
						delete event.second;
					}
					break;
				case EVENT_MOUSERELEASE:
					{
						WidgetImplWinGLX11* w = windows[ event.first ];
						w->widget->mouseReleaseEvent( ( MouseReleaseEvent* ) event.second );
						delete event.second;
					}
					break;
				case EVENT_MOUSEMOVE:
					{
						WidgetImplWinGLX11* w = windows[ event.first ];
						w->widget->mouseMoveEvent( ( MouseMoveEvent* ) event.second );
						delete event.second;
					}
					break;
				case EVENT_CLOSE:
					{
						WidgetImplWinGLX11* w = windows[ event.first ];
						( ( Window* ) w->widget )->closeEvent( ( CloseEvent* ) event.second );
						delete event.second;
					}
					break;
				case EVENT_TIMER:
					{
						TimerInfo* tinfo = ( ( TimerEvent * ) event.second )->timerinfo;
						tinfo->dispatch();
					}
					break;
				default:
					break;
			}
		}

		/* FIXME: do cleanup afterwards */
	}
}

