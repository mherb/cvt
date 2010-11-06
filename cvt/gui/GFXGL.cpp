#include <cvt/gui/GFXGL.h>
#include <cvt/gl/OpenGL.h>

#include <cvt/gfx/IFilterVector.h>
#include <cvt/gfx/IFilterScalar.h>
#include <cvt/gfx/Image.h>
#include <cvt/io/ImageIO.h>
#include <cvt/gfx/ImageAllocatorGL.h>

#include <cstring>
#include <cvt/gui/internal/glfont.h>

/* Shaders */
#include <cvt/gl/shader/basic_vert.h>
#include <cvt/gl/shader/basic_frag.h>
#include <cvt/gl/shader/basictex_vert.h>
#include <cvt/gl/shader/basictex_frag.h>
#include <cvt/gl/shader/text_vert.h>
#include <cvt/gl/shader/text_frag.h>
#include <cvt/gl/shader/basicrrtex_frag.h>


namespace cvt {

	static void ortho2d( IFilterVector16& vec, float left, float right, float top, float bottom, float near, float far )
	{
		vec[ 0 ] = 2.0f / ( right - left );
		vec[ 1 ] = 0.0f;
		vec[ 2 ] = 0.0f;
		vec[ 3 ] = 0.0f;
		vec[ 0 + 4 ] = 0.0f;
		vec[ 1 + 4 ] = 2.0f / ( top - bottom );
		vec[ 2 + 4 ] = 0.0f;
		vec[ 3 + 4 ] = 0.0f;
		vec[ 0 + 8 ] = 0.0f;
		vec[ 1 + 8 ] = 0.0f;
		vec[ 2 + 8 ] = -2.0f / ( far - near );
		vec[ 3 + 8 ] = 0.0f;
		vec[ 0 + 12 ] = - ( right + left ) / ( right - left );
		vec[ 1 + 12 ] = - ( top + bottom ) / ( top - bottom );
		vec[ 2 + 12 ] = - ( far + near ) / ( far - near );
		vec[ 3 + 12 ] = 1.0f;
	}

	GFXGL::GFXGL() : vbo( GL_ARRAY_BUFFER )
	{
		try {
			progbasic.build( _basic_vert_source, _basic_frag_source );
			progbasictex.build( _basictex_vert_source, _basicrrtex_frag_source );
			progtext.build( _text_vert_source, _text_frag_source );
		} catch( GLException e ) {
			std::cout << e.what() << e.log() << std::endl;
		}

		updateState();

		glGenTextures( 1, &texfont );

		glBindTexture(GL_TEXTURE_2D, texfont );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, _glfont.width, _glfont.height, 0, GL_RED, GL_UNSIGNED_BYTE, _glfont.texdata );
		glBindTexture( GL_TEXTURE_2D, 0 );
	}

	GFXGL::~GFXGL()
	{
		/* FIXME */
	}

	void GFXGL::updateState()
	{
		glEnable( GL_BLEND );
		glEnable( GL_TEXTURE_2D );
		glEnable( GL_POINT_SPRITE );
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void GFXGL::fillRect( const Recti& rect )
	{
		fillRect( rect.x, rect.y, rect.width, rect.height );
	}

	void GFXGL::fillRect( int x, int y, int w, int h )
	{
		GLint vertices[ 8 ] = {
			x	 , y + h,
			x	 , y    ,
			x + w, y + h,
			x + w, y
		};

		progbasic.bind();
		IFilterVector16 vec;
		ortho2d( vec, 0.0f, ( float ) width, 0.0f, ( float ) height, -10.0f, 10.0f );
		progbasic.setArg( "MVP", &vec );

		vbo.alloc( GL_STATIC_DRAW, sizeof( GLint ) * 8, vertices );

		vao.setColor( color );
		vao.setVertexData( vbo, 2, GL_INT );
		vao.draw( GL_TRIANGLE_STRIP, 0, 4 );
	}

	void GFXGL::drawText( int x, int y, const char* text )
	{
		int len = ( int ) strlen( text );
		int* vertices = new int[ len * 3 ];
		x += _glfont.offx;
		y += _glfont.offy;
		for( int i = 0; i < len; i++ ) {
			vertices[ i * 3 + 0 ] = x;
			vertices[ i * 3 + 1 ] = y;
			vertices[ i * 3 + 2 ] = text[ i ];
			x += _glfont.advance[ ( uint8_t ) text[ i ] ];
		}

		progtext.bind();
		IFilterScalar scalar( 0 );
		progtext.setArg("TexFont", 0 );
		progtext.setArg("Scale", _glfont.ptsize / ( float ) ( _glfont.width ) );
		IFilterVector16 vec;
		ortho2d( vec, 0.0f, ( float ) width, 0.0f, ( float ) height, -10.0f, 10.0f );
		progtext.setArg( "MVP", &vec );

		glBindTexture( GL_TEXTURE_2D, texfont );

		GLBuffer tvbo( GL_ARRAY_BUFFER );
		tvbo.alloc( GL_STATIC_DRAW, sizeof( int ) * 3 * len, vertices );

		vao.setColor( color );
		vao.setVertexData( tvbo, 3, GL_INT );
		glPointSize( _glfont.ptsize );
		vao.draw( GL_POINTS, 0, len );

		glBindTexture( GL_TEXTURE_2D, 0 );
		delete[] vertices;
	}


	void GFXGL::drawImage( int x, int y, const Image& img )
	{
		GLint w, h;

		if( img.memType() != IALLOCATOR_GL )
			return;

		w = ( GLint ) img.width();
		h = ( GLint ) img.height();
		GLint vertices[ 8 ] = {
			x	 , y + h,
			x	 , y    ,
			x + w, y + h,
			x + w, y
		};
		GLfloat texcoords[ 8 ] = {
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,
			1.0f, 0.0f
		};

		progbasictex.bind();
		progbasictex.setArg("Tex", 0 );
		IFilterVector16 vec;
		ortho2d( vec, 0.0f, ( float ) width, 0.0f, ( float ) height, -10.0f, 10.0f );
		progbasictex.setArg( "MVP", &vec );
		progbasictex.setArg( "ImageSize", ( float ) w, ( float ) h );
		progbasictex.setArg( "Radius", 25.0f );

		ImageAllocatorGL* glmem = ( ImageAllocatorGL* ) img._mem;
		glBindTexture(GL_TEXTURE_2D, glmem->_tex2d );

		GLBuffer vbuf( GL_ARRAY_BUFFER );
		vbuf.alloc( GL_STATIC_DRAW, sizeof( int ) * 8, vertices );

		GLBuffer tbuf( GL_ARRAY_BUFFER );
		tbuf.alloc( GL_STATIC_DRAW, sizeof( int ) * 8, texcoords );

		vao.setVertexData( vbuf, 2, GL_INT );
		vao.setTexCoordData( tbuf, 2, GL_FLOAT );
		vao.setColor( color );
		vao.draw( GL_TRIANGLE_STRIP, 0, 4 );

		vao.resetTexCoord();

		glBindTexture( GL_TEXTURE_2D, 0 );
	}
}
