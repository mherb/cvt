/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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
