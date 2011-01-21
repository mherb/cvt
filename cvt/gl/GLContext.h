#ifndef CVT_GLCONTEXT_H
#define CVT_GLCONTEXT_H

#include <cvt/gl/GLFormat.h>

namespace cvt {
	class GLContextImpl;

	class GLContext {
		public:
			GLContext( const GLFormat& format );
			~GLContext();

			const GLFormat& format() const;
			void makeCurrent();
			void swapBuffers();

			static GLContext* currentContext();

		private:
			GLContext( const GLContext& ctx );

			GLContextImpl* _ctx;

			static GLContext* _current;
	};

	inline GLContext* GLContext::currentContext()
	{
		return _current;
	}
}

#endif
