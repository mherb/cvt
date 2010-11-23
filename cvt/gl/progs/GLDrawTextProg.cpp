#include <cvt/gl/progs/GLDrawTextProg.h>

#include <cvt/gl/shader/120/text_120_vert.h>
#include <cvt/gl/shader/120/text_120_frag.h>
#include <cvt/gl/shader/150/text_150_vert.h>
#include <cvt/gl/shader/150/text_150_frag.h>

#include <cvt/gui/internal/glfont.h>

namespace cvt {
	GLDrawTextProg::GLDrawTextProg() : _vbo( GL_ARRAY_BUFFER )
	{
		try {
			if( GL::isGLSLVersionSupported( 1, 50 ) ) {
				build( _text_150_vert_source, _text_150_frag_source );
			} else {
				build( _text_120_vert_source, _text_120_frag_source );
			}
		} catch( GLException e ) {
			std::cout << e.what() << e.log() << std::endl;
		}

		_vbo.alloc( GL_STATIC_DRAW, sizeof( GLint ) * 8 );
		_mvploc = uniformLocation( "MVP" );
		_texloc = uniformLocation( "TexFont" );
		_scaleloc = uniformLocation( "Scale" );

		_tex.alloc( GL_RED, _glfont.width, _glfont.height, GL_RED, GL_UNSIGNED_BYTE, _glfont.texdata );
		bind();
		glUniform1f( _texloc, 0 );
		glUniform1f( _scaleloc, _glfont.ptsize / ( float ) ( _glfont.width ) );
		unbind();
	}

	GLDrawTextProg::~GLDrawTextProg()
	{

	}

	void GLDrawTextProg::setProjection( const Matrix4f& projection )
	{
		glUniformMatrix4fv( _mvploc, 1, true , ( const GLfloat* ) projection.ptr() );
	}

	void GLDrawTextProg::setColor( const Color& color )
	{
		_vao.setColor( color );
	}

	void GLDrawTextProg::drawText( int x, int y, const char* text )
	{
		GLint* buf;

		int len = ( int ) strlen( text );

		if( _vbo.size() < sizeof( int ) * 3 * len )
			_vbo.alloc( GL_DYNAMIC_DRAW, sizeof( int ) * 3 * len );

		x += _glfont.offx;
		y += _glfont.offy;

		buf = ( GLint* ) _vbo.map( GL_WRITE_ONLY );
		for( int i = 0; i < len; i++ ) {
			buf[ i * 3 + 0 ] = x;
			buf[ i * 3 + 1 ] = y;
			buf[ i * 3 + 2 ] = text[ i ];
			x += _glfont.advance[ ( uint8_t ) text[ i ] ];
		}
		_vbo.unmap();

		_vao.setVertexData( _vbo, 3, GL_INT );
		_tex.bind();
		glPointSize( _glfont.ptsize );
		_vao.draw( GL_POINTS, 0, len );
		_tex.unbind();
	}

}
