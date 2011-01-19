#include <cvt/gl/progs/GLFillRectProg.h>

#include <cvt/gl/shader/120/basic_120_vert.h>
#include <cvt/gl/shader/120/basic_120_frag.h>
#include <cvt/gl/shader/150/basic_150_vert.h>
#include <cvt/gl/shader/150/basic_150_frag.h>

namespace cvt {
	GLFillRectProg::GLFillRectProg() : _vbo( GL_ARRAY_BUFFER )
	{
		try {
			if( GL::isGLSLVersionSupported( 1, 50 ) ) {
				build( _basic_150_vert_source, _basic_150_frag_source );
			} else {
				build( _basic_120_vert_source, _basic_120_frag_source );
			}
		} catch( GLException e ) {
			std::cout << e.what() << e.log() << std::endl;
		}

		bind();
		_vbo.alloc( GL_STATIC_DRAW, sizeof( GLint ) * 8 );
		_mvploc = uniformLocation( "MVP" );
		unbind();
	}

	GLFillRectProg::~GLFillRectProg()
	{

	}

	void GLFillRectProg::setProjection( const Matrix4f& projection )
	{
		glUniformMatrix4fv( _mvploc, 1, true , ( const GLfloat* ) projection.ptr() );
	}

	void GLFillRectProg::setColor( const Color& color )
	{
		_vao.setColor( color );
	}

	void GLFillRectProg::fillRect( int x, int y, int w, int h )
	{
		GLint* buf;
		buf = ( GLint* ) _vbo.map( GL_WRITE_ONLY );
		*buf++ = x;
		*buf++ = y + h;
		*buf++ = x;
		*buf++ = y;
		*buf++ = x + w;
		*buf++ = y + h;
		*buf++ = x + w;
		*buf++ = y;
		_vbo.unmap();
		_vao.setVertexData( _vbo, 2, GL_INT );
		_vao.draw( GL_TRIANGLE_STRIP, 0, 4 );
	}

}
