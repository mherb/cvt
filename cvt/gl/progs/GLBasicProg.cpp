#include <cvt/gl/progs/GLBasicProg.h>
#include <cvt/util/SIMD.h>

#include <cvt/gl/shader/120/basic_120_vert.h>
#include <cvt/gl/shader/120/basic_120_frag.h>
#include <cvt/gl/shader/150/basic_150_vert.h>
#include <cvt/gl/shader/150/basic_150_frag.h>

namespace cvt {
	GLBasicProg::GLBasicProg() : _vbo( GL_ARRAY_BUFFER )
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

	GLBasicProg::~GLBasicProg()
	{

	}

	void GLBasicProg::setProjection( const Matrix4f& projection )
	{
		glUniformMatrix4fv( _mvploc, 1, true , ( const GLfloat* ) projection.ptr() );
	}

	void GLBasicProg::setColor( const Color& color )
	{
		_vao.setColor( color );
	}

	void GLBasicProg::fillRect( int x, int y, int w, int h )
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

	void GLBasicProg::drawLines( const Vector2f* pts, size_t n, float width )
	{
		GLBuffer buf( GL_ARRAY_BUFFER );
		SIMD* simd = SIMD::instance();
		GLfloat* mbuf;

		buf.alloc( GL_STREAM_DRAW, sizeof( GLfloat ) * 2 * n );
		mbuf = ( GLfloat* ) buf.map( GL_WRITE_ONLY );
		simd->Memcpy( ( uint8_t* ) mbuf, ( const uint8_t* ) pts, sizeof( GLfloat ) * 2 * n );
		buf.unmap();

		glLineWidth( width );
		_vao.setVertexData( buf, 2, GL_FLOAT );
		_vao.draw( GL_LINES, 0, n );
	}

}
