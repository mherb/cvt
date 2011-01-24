#ifndef CVT_GLXCONTEXT_H
#define CVT_GLXCONTEXT_H

#include <cvt/gui/internal/X.h>
#include <cvt/gl/GLContext.h>

namespace cvt {
	class GLXContext : public GLContext
	{
		public:
			GLXContext( ::Display* dpy, const GLFormat& format, const GLXContext* share = NULL );
			~GLXContext();
			void setDrawable( ::Window win );

			virtual void makeCurrent();
			virtual void swapBuffers();
			virtual void resetCurrent();
			const ::XVisualInfo* XVisualInfo() const;

		private:
			::Display* _dpy;
			::Window _win;
			::GLXContext _glxctx;
			::XVisualInfo* _visual;
	};

	inline GLXContext::GLXContext( ::Display* dpy, const GLFormat& format, const GLXContext* share ) : GLContext( format ), _dpy( dpy ), _win( 0 )
	{
		::GLXFBConfig* fbconfig = NULL;
		int nconfigs, val;
		int attribs[] = {
			GLX_RED_SIZE,	  _format.redSize(),
			GLX_GREEN_SIZE,	  _format.greenSize(),
			GLX_BLUE_SIZE,	  _format.blueSize(),
			GLX_ALPHA_SIZE,	  _format.alphaSize(),
			GLX_DEPTH_SIZE,	  _format.depthSize(),
			GLX_STENCIL_SIZE, _format.stencilSize(),
			GLX_DOUBLEBUFFER, _format.doubleBuffer(),
			None
		};


		fbconfig = glXChooseFBConfig( _dpy,  DefaultScreen( _dpy ), attribs, &nconfigs );
		if( !nconfigs )
			throw CVTException( "Error: GLXContext creation failed\n" );

		_visual = glXGetVisualFromFBConfig( _dpy, fbconfig[ 0 ] );
		_glxctx  = glXCreateNewContext( _dpy, fbconfig[ 0 ], GLX_RGBA_TYPE, share?share->_glxctx:NULL, format.direct() );

		if( _glxctx == NULL )
			throw CVTException( "Error: GLXContext creation failed\n" );

		glXGetFBConfigAttrib( _dpy, fbconfig[ 0 ], GLX_RED_SIZE , &val );
		_format.setRedSize( val );
		glXGetFBConfigAttrib( _dpy, fbconfig[ 0 ], GLX_GREEN_SIZE , &val );
		_format.setGreenSize( val );
		glXGetFBConfigAttrib( _dpy, fbconfig[ 0 ], GLX_BLUE_SIZE , &val );
		_format.setBlueSize( val );
		glXGetFBConfigAttrib( _dpy, fbconfig[ 0 ], GLX_ALPHA_SIZE , &val );
		_format.setAlphaSize( val );
		glXGetFBConfigAttrib( _dpy, fbconfig[ 0 ], GLX_DEPTH_SIZE , &val );
		_format.setDepthSize( val );
		glXGetFBConfigAttrib( _dpy, fbconfig[ 0 ], GLX_STENCIL_SIZE , &val );
		_format.setStencilSize( val );
		glXGetFBConfigAttrib( _dpy, fbconfig[ 0 ], GLX_DOUBLEBUFFER , &val );
		_format.setDoubleBuffer( val );
		_format._direct = glXIsDirect(_dpy, _glxctx);

		::XFree( fbconfig );
	}

	inline GLXContext::~GLXContext()
	{
		resetCurrent();
		::XFree( _visual );
	}

	inline void GLXContext::setDrawable( ::Window win )
	{
		_win = win;
	}

	inline const ::XVisualInfo* GLXContext::XVisualInfo() const
	{
		return _visual;
	}

	inline void GLXContext::makeCurrent()
	{
		if( GLContext::currentContext() == this )
			return;
		glXMakeCurrent( _dpy, _win, _glxctx );
		GLContext::setCurrent( this );
	}

	inline void GLXContext::swapBuffers()
	{
		glXSwapBuffers( _dpy, _win );
	}

	inline void GLXContext::resetCurrent()
	{
		GLContext::setCurrent( NULL );
		glXMakeCurrent( _dpy, None, NULL );
	}

}

#endif
