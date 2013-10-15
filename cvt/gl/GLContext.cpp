#include <cvt/gl/GLContext.h>
#include <cvt/gl/internal/GLContextImpl.h>

namespace cvt {

	GLContext* GLContext::_current = NULL;

	GLContext::GLContext( const GLFormat& glformat ) : _format( glformat )
	{
	}


	GLContext::~GLContext()
	{
		if( _current == this ) {
			//FIXME:
			// resetCurrent();
			_current = NULL;
		}
	}
}
