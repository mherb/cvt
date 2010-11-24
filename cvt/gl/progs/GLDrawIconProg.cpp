#include <cvt/gl/progs/GLDrawIconProg.h>
#include <cvt/gfx/Image.h>
#include <cvt/io/ImageIO.h>
#include <cvt/io/Resources.h>


#include <cvt/gl/shader/120/text_120_vert.h>
#include <cvt/gl/shader/120/text_120_frag.h>
#include <cvt/gl/shader/150/text_150_vert.h>
#include <cvt/gl/shader/150/text_150_frag.h>

namespace cvt {
	GLDrawIconProg::GLDrawIconProg() : _vbo( GL_ARRAY_BUFFER )
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

		Image icons;
		size_t stride;
		cvt::Resources resources;
		ImageIO::loadPNG( icons, resources.find( "Icons/Icons.png" ) );

		uint8_t* ptr = icons.map( &stride );
		_tex.alloc( GL_RED, 128, 128, GL_RED, GL_UNSIGNED_BYTE, ptr );
		icons.unmap( ptr );
		bind();
		glUniform1f( _texloc, 0 );
		glUniform1f( _scaleloc, 16.0f / 128.0f );
		unbind();
	}

	GLDrawIconProg::~GLDrawIconProg()
	{

	}

	void GLDrawIconProg::setProjection( const Matrix4f& projection )
	{
		glUniformMatrix4fv( _mvploc, 1, true , ( const GLfloat* ) projection.ptr() );
	}

	void GLDrawIconProg::setColor( const Color& color )
	{
		_vao.setColor( color );
	}

	void GLDrawIconProg::drawIcon( int x, int y, int icon )
	{
		GLint* buf;

		buf = ( GLint* ) _vbo.map( GL_WRITE_ONLY );
		buf[ 0 ] = x;
		buf[ 1 ] = y;
		buf[ 2 ] = icon;
		_vbo.unmap();

		_vao.setVertexData( _vbo, 3, GL_INT );
		_tex.bind();
		glPointSize( 16.0f );
		_vao.draw( GL_POINTS, 0, 1 );
		_tex.unbind();
	}

}
