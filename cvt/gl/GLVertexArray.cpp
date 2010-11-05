#include <cvt/gl/GLVertexArray.h>

namespace cvt {
	
#ifdef __APPLE__
	#define glBindVertexArray glBindVertexArrayAPPLE
	#define glGenVertexArrays glGenVertexArraysAPPLE
	#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif

	GLVertexArray::GLVertexArray() : _arrays( 0 )
	{
		glGenVertexArrays( 1, &_vao );
	}

	GLVertexArray::~GLVertexArray()
	{
		glDeleteVertexArrays( 1, &_vao );
	}

	void GLVertexArray::setAttribData( GLuint index, const GLBuffer& buffer, GLint size, GLenum type, GLsizei stride, const GLvoid* offset )
	{
		CVTAssert( buffer.target() == GL_ARRAY_BUFFER, "Buffer is not an array buffer!" );
		CVTAssert( index < 16, "OpenGL attrib is greater than 16!" );

		glBindVertexArray( _vao );
		_arrays |= ( 1 << index );
		buffer.bind();
		glVertexAttribPointer( index, size, type, GL_FALSE, stride, offset );
		buffer.unbind();
		glBindVertexArray( 0 );
	}

	void GLVertexArray::setAttribData( GLuint index, float v1 )
	{
		CVTAssert( index < 16, "OpenGL attrib is greater than 16!" );
		_arrays &= ~( 1 << index );
		glVertexAttrib1f( index, v1 );
	}

	void GLVertexArray::setAttribData( GLuint index, float v1, float v2 )
	{
		CVTAssert( index < 16, "OpenGL attrib is greater than 16!" );
		_arrays &= ~( 1 << index );
		glVertexAttrib2f( index, v1, v2 );
	}

	void GLVertexArray::setAttribData( GLuint index, float v1, float v2, float v3 )
	{
		CVTAssert( index < 16, "OpenGL attrib is greater than 16!" );
		_arrays &= ~( 1 << index );
		glVertexAttrib3f( index, v1, v2, v3 );
	}

	void GLVertexArray::setAttribData( GLuint index, float v1, float v2, float v3, float v4 )
	{
		CVTAssert( index < 16, "OpenGL attrib is greater than 16!" );
		_arrays &= ~( 1 << index );
		glVertexAttrib4f( index, v1, v2, v3, v4 );
	}

	void GLVertexArray::setColor( const Color& color )
	{
		_arrays &= ~( 1 << GLSL_COLOR_IDX );
		glVertexAttrib4fv( GLSL_COLOR_IDX, color.data() );
	}

	void GLVertexArray::setVertexData( const GLBuffer& buffer, GLint size, GLenum type, GLsizei stride, const GLvoid* offset )
	{
		CVTAssert( buffer.target() == GL_ARRAY_BUFFER, "Buffer is not an array buffer!" );
		glBindVertexArray( _vao );
		_arrays |= ( 1 << GLSL_VERTEX_IDX );
		buffer.bind();

		glVertexAttribPointer( GLSL_VERTEX_IDX, size, type, GL_FALSE, stride, offset );

		buffer.unbind();
		glBindVertexArray( 0 );
	}

	void GLVertexArray::draw( GLenum mode, GLint first, GLsizei count) const
	{
		if( !_arrays ) return;

		glBindVertexArray( _vao );

		for( unsigned int i = 0; i < 16; i++ ) {
			if( _arrays & ( 1 << i ) ) {
				glEnableVertexAttribArray( i );
			}
		}
		glDrawArrays( mode, first, count );

		for( unsigned int i = 0; i < 16; i++ ) {
			if( _arrays & ( 1 << i ) ) {
				glDisableVertexAttribArray( i );
			}
		}

		glBindVertexArray( 0 );
	}

}