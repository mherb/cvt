#include <cvt/gl/GLContext.h>
#include <cvt/gl/internal/GLContextImpl.h>

namespace cvt {

	GLContext* GLContext::_current = NULL;

	GLContext::GLContext( const GLFormat& glformat )
	{
		_ctx = Application::instance()._createGLContext( glformat );
	}

	GLContext::~GLContext()
	{
		if( _current == this ) {
			 _ctx->resetCurrent();
			_current = NULL
		}
		delete _ctx;
	}

	const GLFormat& GLContext::format() const
	{
		return _ctx->format();
	}

	void GLContext::makeCurrent()
	{
		if( current == this )
			return;
		_ctx->makeCurrent();
		_current = this;
	}

	void GLContext::swapBuffers()
	{
		_ctx->swapBuffers();
	}
}
