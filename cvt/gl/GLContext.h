#ifndef CVT_GLCONTEXT_H
#define CVT_GLCONTEXT_H

#include <cvt/gl/OpenGL.h>
#include <cvt/gl/GLFormat.h>

namespace cvt {

	class GLContext {
		public:
			virtual ~GLContext();

			const GLFormat& format() const;
			virtual void makeCurrent() = 0;
			virtual void swapBuffers() = 0;
			virtual void resetCurrent() = 0;

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
