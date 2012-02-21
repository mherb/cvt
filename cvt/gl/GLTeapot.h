/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_GLTEAPOT_H
#define CVT_GLTEAPOT_H

#include <cvt/gl/GLVertexArray.h>
#include <cvt/gfx/Color.h>

namespace cvt {

	/**
	  \ingroup GL
	*/
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
