#ifndef CVT_GLTEAPOT_H
#define CVT_GLTEAPOT_H

#include <cvt/gl/GLVertexArray.h>
#include <cvt/gfx/Color.h>

namespace cvt {
	class GLTeapot {
		public:
			GLTeapot();

			void setColor( const Color& color ) { _vao.setColor( color ); }
			void draw() const;

		private:
			GLTeapot( const GLTeapot& ) {};
			GLTeapot& operator=( const GLTeapot& );

			GLVertexArray _vao;
			GLBuffer	  _vtxbuf;
			GLBuffer	  _normbuf;
			GLBuffer	  _texcoordbuf;
			GLBuffer	  _facebuf;

			static float _vertices[][ 3 ];
			static float _normals[][ 3 ];
			static float _texcoords[][ 2 ];
			static GLuint _faces[];
	};
}

#endif
