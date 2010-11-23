#include <cvt/gl/progs/GLDrawImageProg.h>

#include <cvt/gfx/ImageAllocatorGL.h>

#include <cvt/gl/shader/120/basictex_120_vert.h>
#include <cvt/gl/shader/120/basictex_120_frag.h>
#include <cvt/gl/shader/150/basictex_150_vert.h>
#include <cvt/gl/shader/150/basictex_150_frag.h>

namespace cvt {
	GLDrawImageProg::GLDrawImageProg() : _vbo( GL_ARRAY_BUFFER ), _tvbo( GL_ARRAY_BUFFER )
	{
		try {
			if( GL::isGLSLVersionSupported( 1, 50 ) ) {
				build( _basictex_150_vert_source, _basictex_150_frag_source );
			} else {
				build( _basictex_120_vert_source, _basictex_120_frag_source );
			}
		} catch( GLException e ) {
			std::cout << e.what() << e.log() << std::endl;
		}

		_vbo.alloc( GL_DYNAMIC_DRAW, sizeof( GLint ) * 8 );
		_tvbo.alloc( GL_STATIC_DRAW, sizeof( GLfloat ) * 8 );

		GLfloat* tv = ( GLfloat* ) _tvbo.map( GL_WRITE_ONLY );
		*tv++ = 0.0f; *tv++ = 1.0f;
		*tv++ = 0.0f; *tv++ = 0.0f;
		*tv++ = 1.0f; *tv++ = 1.0f;
		*tv++ = 1.0f; *tv++ = 0.0f;
		_tvbo.unmap();
		_vao.setTexCoordData( _tvbo, 2, GL_FLOAT );

		bind();
		_mvploc = uniformLocation( "MVP" );
		_texloc = uniformLocation( "Tex" );
		glUniform1i( _texloc, 0 );
		unbind();
	}

	GLDrawImageProg::~GLDrawImageProg()
	{

	}

	void GLDrawImageProg::setProjection( const Matrix4f& projection )
	{
		glUniformMatrix4fv( _mvploc, 1, true , ( const GLfloat* ) projection.ptr() );
	}

	void GLDrawImageProg::setAlpha( float alpha )
	{
		Color c( 1.0f, 1.0f, 1.0f, alpha );
		_vao.setColor( c );
	}

	void GLDrawImageProg::drawImage( int x, int y, const Image& img )
	{
		GLint w, h;

		if( img.memType() != IALLOCATOR_GL )
			return;

		w = ( GLint ) img.width();
		h = ( GLint ) img.height();

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
		glBindTexture( GL_TEXTURE_2D, ( ( ImageAllocatorGL* ) img._mem  )->_tex2d );
		_vao.draw( GL_TRIANGLE_STRIP, 0, 4 );

		glBindTexture( GL_TEXTURE_2D, 0 );
	}

	void GLDrawImageProg::drawImage( int x, int y, int width, int height, const Image& img )
	{
		GLint w, h;

		if( img.memType() != IALLOCATOR_GL )
			return;

		w = Math::max( width, 1 );
		h = Math::max( height, 1 );

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
		glBindTexture( GL_TEXTURE_2D, ( ( ImageAllocatorGL* ) img._mem  )->_tex2d );
		_vao.draw( GL_TRIANGLE_STRIP, 0, 4 );

		glBindTexture( GL_TEXTURE_2D, 0 );
	}


}
