/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/gl/GLTeapot.h>

namespace cvt {

#define INDICES_SIZE	2976
#define VERTEX_SIZE		783
#define NORMAL_SIZE		783
#define TEXCOORD_SIZE	783

	GLTeapot::GLTeapot( ) :
		_vtxbuf( GL_ARRAY_BUFFER ),
		_normbuf( GL_ARRAY_BUFFER ),
		_texcoordbuf( GL_ARRAY_BUFFER ),
		_facebuf( GL_ELEMENT_ARRAY_BUFFER )
	{
		_vtxbuf.alloc( GL_STATIC_DRAW, sizeof( GLfloat ) * 3 * VERTEX_SIZE, _vertices );
		_texcoordbuf.alloc( GL_STATIC_DRAW, sizeof( GLfloat ) * 2 * TEXCOORD_SIZE, _texcoords );
		_normbuf.alloc( GL_STATIC_DRAW, sizeof( GLfloat ) * 3 * NORMAL_SIZE, _normals );
		_facebuf.alloc( GL_STATIC_DRAW, sizeof( GLuint ) * INDICES_SIZE, _faces );

		_vao.setVertexData( _vtxbuf, 3, GL_FLOAT );
		_vao.setTexCoordData( _texcoordbuf, 2, GL_FLOAT );
		_vao.setNormalData( _normbuf, 3, GL_FLOAT );
	}

	void GLTeapot::draw() const
	{
		_vao.drawIndirect( _facebuf, GL_UNSIGNED_INT, GL_TRIANGLES, INDICES_SIZE );
	}

	#include "GLTeapot.inl"
}

