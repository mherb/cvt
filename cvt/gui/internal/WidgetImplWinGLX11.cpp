#include <cvt/gui/internal/WidgetImplWinGLX11.h>

namespace cvt {

	WidgetImplWinGLX11::WidgetImplWinGLX11( ::Display* display, ::GLXContext context, ::XVisualInfo* visinfo, Widget* _window, std::deque<WidgetImplWinGLX11*>* updateq ) : _widget( _window ), dpy( display ), ctx( context ), visible( false ), _rect( 0, 0, 1, 1 ), needsupdate( false ), _updateq( updateq )
	{
		::XSetWindowAttributes attr;
		unsigned long mask;

		attr.win_gravity = NorthWestGravity;
		attr.colormap = ::XCreateColormap( dpy, RootWindow( dpy, DefaultScreen( dpy ) ), visinfo->visual, AllocNone);
		attr.event_mask = StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | ExposureMask;
		mask = CWColormap | CWEventMask | CWWinGravity;

		win = ::XCreateWindow( dpy, RootWindow( dpy, DefaultScreen( dpy ) ), 0, 0, _rect.width, _rect.height,
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

	void WidgetImplWinGLX11::size( int& width, int& height ) const
	{
		width = _rect.width;
		height = _rect.height;
	}

	void WidgetImplWinGLX11::setSize( int width, int height )
	{
		if( _rect.width == width && _rect.height == height )
			return;

		::XResizeWindow( dpy, win, width, height );
	}

	void WidgetImplWinGLX11::position( int& x, int& y ) const
	{
		x = _rect.x;
		y = _rect.y;
	}

	void WidgetImplWinGLX11::setPosition( int x, int y )
	{
		if( _rect.x == x && _rect.y == y )
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

		if( _rect == r )
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
		if( needsupdate || !visible )
			return;
		_updateq->push_back( this );
	}

	void WidgetImplWinGLX11::update( const Recti& rect )
	{
	    update();
	}

	void WidgetImplWinGLX11::setMinimumSize( int w, int h )
	{
	    _minSize.set( Math::max( w, 0 ), Math::max( h, 0 ) );
	    XSizeHints hints;
	    hints.flags = PMinSize;
	    hints.min_width = ( uint32_t ) _minSize[ 0 ];
	    hints.min_height = ( uint32_t ) _minSize[ 1 ];
	    ::XSetWMNormalHints( dpy, win , &hints );
	}

	void WidgetImplWinGLX11::setMaximumSize( int w, int h )
	{
	    _maxSize.set( Math::max( w, 0 ), Math::max( h, 0 ) );
	    XSizeHints hints;
	    hints.flags = PMaxSize;
	    hints.max_width = ( uint32_t ) _maxSize[ 0 ];
	    hints.max_height = ( uint32_t ) _maxSize[ 1 ];
	    ::XSetWMNormalHints( dpy, win , &hints );
	}

	void WidgetImplWinGLX11::minimumSize( int& w, int& h )
	{
	    w = _minSize[ 0 ];
	    h = _minSize[ 1 ];
	}

	void WidgetImplWinGLX11::maximumSize( int& w, int& h )
	{
	    w = _maxSize[ 0 ];
	    h = _maxSize[ 1 ];
	}


	void WidgetImplWinGLX11::paintEvent( PaintEvent* event )
	{
		if( win != glXGetCurrentDrawable() )
			glXMakeCurrent( dpy, win, ctx );

		gfx->setDefault();
		Recti viewport( 0, 0, _rect.width, _rect.height );
		gfx->setViewport( viewport );
		gfx->setChildrect( viewport );
		glClear( GL_DEPTH_BUFFER_BIT );
		_widget->paintEvent( event, gfx );
		glXSwapBuffers( dpy, win );
		needsupdate = false;
	}

	void WidgetImplWinGLX11::resizeEvent( ResizeEvent* event )
	{
		event->getSize( _rect.width, _rect.height );

		glXMakeCurrent(dpy, win, ctx);
		glViewport(0, 0, (GLsizei) _rect.width, (GLsizei) _rect.height );
		_widget->resizeEvent( event );
	}

	void WidgetImplWinGLX11::moveEvent( MoveEvent* event )
	{
		event->position( _rect.x, _rect.y );
		_widget->moveEvent( event );
	}

	void WidgetImplWinGLX11::showEvent( ShowEvent* event )
	{
		visible = true;
		_widget->showEvent( event );
	}

	void WidgetImplWinGLX11::hideEvent( HideEvent* event )
	{
		visible = false;
		_widget->hideEvent( event );
	}

}
