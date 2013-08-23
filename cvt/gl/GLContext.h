/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_GLCONTEXT_H
#define CVT_GLCONTEXT_H

#include <cvt/gl/OpenGL.h>
#include <cvt/gl/GLFormat.h>
#include <cvt/cl/OpenCL.h>

namespace cvt {

	enum GLPlatform {
		GL_PLATFORM_X11,
		GL_PLATFORM_OSX
	};

	/**
	  \ingroup GL
	*/
	class GLContext {
		public:
			virtual ~GLContext();

			const GLFormat& format() const;
			virtual void makeCurrent() = 0;
			virtual void swapBuffers() = 0;
			virtual void resetCurrent() = 0;

			virtual void shareCL( cl_context_properties* props, int size, int* retsize ) const = 0;

			virtual GLPlatform platform() const = 0;

			static GLContext* currentContext();


		protected:
			GLContext( const GLFormat& format );
			GLFormat _format;
			static void setCurrent( GLContext* ctx );

		private:
			GLContext();
			GLContext( const GLContext& ctx );

			static GLContext* _current;
	};

	inline GLContext* GLContext::currentContext()
	{
		return _current;
	}

	inline const GLFormat& GLContext::format() const
	{
		return _format;
	}

	inline void GLContext::setCurrent( GLContext* c )
	{
		_current = c;
	}
}

#endif
