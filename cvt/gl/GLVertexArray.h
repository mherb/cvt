/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVTGLVERTEXARRAY_H
#define CVTGLVERTEXARRAY_H

#include <cvt/gl/OpenGL.h>
#include <cvt/gl/GLBuffer.h>
#include <cvt/gl/GLProgram.h>
#include <cvt/gfx/Color.h>
#include <cvt/util/CVTAssert.h>

namespace cvt {

	/**
	  \ingroup GL
	*/
	class GLVertexArray {
		public:
			GLVertexArray();
			~GLVertexArray();

			void setVertexData( const GLBuffer& buffer, GLint size, GLenum type, GLsizei stride = 0, const GLvoid* offset = 0 );
			void setColorData( const GLBuffer& buffer, GLint size, GLenum type, GLsizei stride = 0, const GLvoid* offset = 0 );
			void setNormalData( const GLBuffer& buffer, GLint size, GLenum type, GLsizei stride = 0, const GLvoid* offset = 0 );
			void setTexCoordData( const GLBuffer& buffer, GLint size, GLenum type, GLsizei stride = 0, const GLvoid* offset = 0 );
			void setAttribData( GLuint index, const GLBuffer& buffer, GLint size, GLenum type, GLsizei stride = 0, const GLvoid* offset = 0 );

			void resetColor() { _arrays &= ~( 1 << GLSL_COLOR_IDX ); };
			void resetNormal() { _arrays &= ~( 1 << GLSL_NORMAL_IDX ); };
			void resetTexCoord() { _arrays &= ~( 1 << GLSL_TEXCOORD_IDX ); };
			void resetAttrib( GLuint index ) { _arrays &= ~( 1 << index ); };

			void setColor( const Color& color );
// FIXME: Remove, the single attrib stuff is not part of the VAO state
//			void setAttribData( GLuint index, float v1 );
//			void setAttribData( GLuint index, float v1, float v2 );
//			void setAttribData( GLuint index, float v1, float v2, float v3 );
//			void setAttribData( GLuint index, float v1, float v2, float v3, float v4 );

			void draw( GLenum mode, GLint first, GLsizei count ) const;
			void drawIndirect( const GLBuffer& elembuf, GLenum type, GLenum mode, GLsizei count ) const;

		private:
			GLuint _vao;
			uint32_t _arrays;
			Color  _color;
	};

	inline void GLVertexArray::setColorData( const GLBuffer& buffer, GLint size, GLenum type, GLsizei stride, const GLvoid* offset )
	{
		setAttribData( GLSL_COLOR_IDX, buffer, size, type, stride, offset );
	}

	inline void GLVertexArray::setNormalData( const GLBuffer& buffer, GLint size, GLenum type, GLsizei stride, const GLvoid* offset )
	{
		setAttribData( GLSL_NORMAL_IDX, buffer, size, type, stride, offset );
	}

	inline void GLVertexArray::setTexCoordData( const GLBuffer& buffer, GLint size, GLenum type, GLsizei stride, const GLvoid* offset )
	{
		setAttribData( GLSL_TEXCOORD_IDX, buffer, size, type, stride, offset );
	}

}



#endif
