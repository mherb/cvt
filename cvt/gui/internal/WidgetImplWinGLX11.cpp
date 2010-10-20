#include <cvt/gui/internal/WidgetImplWinGLX11.h>

namespace cvt {

	WidgetImplWinGLX11::WidgetImplWinGLX11( ::Display* display, ::GLXContext context, ::XVisualInfo* visinfo, Window* _window ) : WidgetImpl( _window ), dpy( display ), ctx( context ), visible( false ), rect( 0, 0, 1, 1 )
	{
		::XSetWindowAttributes attr;
		unsigned long mask;
		::Atom wmDelete;

		attr.background_pixmap = None;
		attr.border_pixel = 0;
		attr.colormap = ::XCreateColormap( dpy, RootWindow( dpy, DefaultScreen( dpy ) ), visinfo->visual, AllocNone);
		attr.event_mask = StructureNotifyMask | ButtonPressMask | ExposureMask;
		mask = CWBackPixmap | CWBorderPixel | CWColormap | CWEventMask;

		win = ::XCreateWindow( dpy, RootWindow( dpy, DefaultScreen( dpy ) ), 0, 0, rect.width, rect.height,
							  0, visinfo->depth, InputOutput,
							  visinfo->visual, mask, &attr );

		glXMakeCurrent(dpy, win, ctx);
		gfx = new GFXGL();
	}

	WidgetImplWinGLX11::~WidgetImplWinGLX11()
	{
		setVisible( false );
		::XDestroyWindow( dpy , win );
		delete gfx;
	}

	void WidgetImplWinGLX11::getSize( int& width, int& height ) const
	{
		width = rect.width;
		height = rect.height;
	}

	void WidgetImplWinGLX11::setSize( int width, int height )
	{
		if( rect.width == width && rect.height == height )
			return;

		::XResizeWindow( dpy, win, width, height );
	}

	void WidgetImplWinGLX11::getPosition( int& x, int& y ) const
	{
		x = rect.x;
		y = rect.y;
	}

	void WidgetImplWinGLX11::setPosition( int x, int y )
	{
		if( rect.x == x && rect.y == y )
			return;

		::XMoveWindow( dpy, win, x, y );
	}

	void WidgetImplWinGLX11::setTitle( const std::string& title )
	{
		char buffer[ 128 ];

		title.copy( buffer, 128 );
		::XTextProperty windowName;
		windowName.value    = ( unsigned char* ) buffer;
		windowName.encoding = XA_STRING;
		windowName.format   = 8;
		windowName.nitems   = Math::min( ( int ) title.length(), 128 );

		::XSetWMName( dpy, win, &windowName );
	}

	void WidgetImplWinGLX11::setRect( const Recti& r )
	{
		::XWindowChanges values;

		if( rect == r )
			return;

		values.x = r.x;
		values.y = r.y;
		values.width = r.width;
		values.height = r.height;
		::XConfigureWindow( dpy , win, CWX | CWY | CWWidth | CWHeight , &values );
	}

	void WidgetImplWinGLX11::setVisible( bool visibility )
	{
		if( visible == visibility )
			return;

		visible = visibility;

		if( visible )
			::XMapWindow( dpy, win );
		else
			::XUnmapWindow( dpy, win );
	}

	void WidgetImplWinGLX11::update()
	{

	}

	void WidgetImplWinGLX11::paintEvent( PaintEvent* event )
	{
		glXMakeCurrent(dpy, win, ctx);
		gfx->setDefault();
		gfx->updateState();
		widget->paintEvent( event, gfx );
		glXSwapBuffers( dpy, win );
	}

	void WidgetImplWinGLX11::resizeEvent( ResizeEvent* event )
	{
		event->getSize( rect.width, rect.height );

		glXMakeCurrent(dpy, win, ctx);
		glViewport(0, 0, (GLsizei) rect.width, (GLsizei) rect.height );
		gfx->setViewport( rect.width, rect.height );
		widget->resizeEvent( event );
	}

	void WidgetImplWinGLX11::moveEvent( MoveEvent* event )
	{
		event->getPosition( rect.x, rect.y );
		widget->moveEvent( event );
	}

	void WidgetImplWinGLX11::showEvent( ShowEvent* event )
	{
		visible = true;
		widget->showEvent( event );
	}

	void WidgetImplWinGLX11::hideEvent( HideEvent* event )
	{
		visible = false;
		widget->hideEvent( event );
	}

}
