#include <cvt/gui/GFXGL.h>
#include <cvt/gl/OpenGL.h>

#include <cvt/gfx/IFilterVector.h>
#include <cvt/gfx/IFilterScalar.h>
#include <cvt/gfx/Image.h>
#include <cvt/io/ImageIO.h>

/* Shaders */
#include <cvt/gl/shader/basic_vert.h>
#include <cvt/gl/shader/basic_frag.h>
#include <cvt/gl/shader/text_vert.h>
#include <cvt/gl/shader/text_frag.h>


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

	GFXGL::GFXGL()
	{
		try {
			progbasic.build( _basic_vert_source, _basic_frag_source );
			progtext.build( _text_vert_source, _text_frag_source );
		} catch( GLException e ) {
			std::cout << e.what() << e.log() << std::endl;
		}
		glGenBuffers( 1, &vbuffer );
		glGenVertexArrays( 1, &varray );
		glGenTextures( 1, &texfont );

		{
			Image fontpng;
			uint8_t* data;
			size_t stride;

			ImageIO::loadPNG( fontpng, "/home/heise/Pictures/font.png" );
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texfont );
			data = fontpng.map( &stride );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, fontpng.width(), fontpng.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
			fontpng.unmap( data );
		}

		updateState();
	}

	void GFXGL::updateState()
	{
		glEnable( GL_BLEND );
		glEnable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void GFXGL::fillRect( const Recti& rect )
	{
		fillRect( rect.x, rect.y, rect.width, rect.height );
	}

	void GFXGL::fillRect( int x, int y, int w, int h )
	{
		int vertices[ 8 ] = {
			x	 , y + h,
			x	 , y    ,
			x + w, y + h,
			x + w, y
		};


		progbasic.bind();
		progbasic.bindAttribLocation( "Vertex", 0 );
		progbasic.bindAttribLocation( "Color", 1 );
		IFilterVector16 vec;
		ortho2d( vec, 0.0f, ( float ) width, 0.0f, ( float ) height, -10.0f, 10.0f );
		progbasic.setArg( "MVP", &vec );

		glBindBuffer( GL_ARRAY_BUFFER, vbuffer );
		glBufferData( GL_ARRAY_BUFFER, sizeof( int ) * 8, vertices, GL_DYNAMIC_DRAW );

		// Attrib 0 is Vertex data
		glBindVertexArray( varray );
		glVertexAttribPointer( 0 , 2, GL_INT, GL_FALSE, 0, 0 );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		// Attrib 1 is Color - constant here
		glVertexAttrib4fv( 1, color.data() );

		glEnableVertexAttribArray( 0 );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		glDisableVertexAttribArray( 0 );

		glBindVertexArray( 0 );
	}

	void GFXGL::drawText( int x, int y, const char* text )
	{
#define TW 32
			int vertices[ 8 ] = {
			x + 0 * TW , y,
			x + 1 * TW , y,
			x + 2 * TW , y,
			x + 3 * TW , y
		};

		progtext.bind();
		progtext.bindAttribLocation( "Vertex", 0 );
		progtext.bindAttribLocation( "Color", 1 );
		IFilterScalar scalar( 0 );
		progtext.setArg("TexFont", 0 );
		IFilterVector16 vec;
		ortho2d( vec, 0.0f, ( float ) width, 0.0f, ( float ) height, -10.0f, 10.0f );
		progtext.setArg( "MVP", &vec );

		glBindBuffer( GL_ARRAY_BUFFER, vbuffer );
		glBufferData( GL_ARRAY_BUFFER, sizeof( int ) * 8, vertices, GL_DYNAMIC_DRAW );

		// Attrib 0 is Vertex data
		glBindVertexArray( varray );
		glVertexAttribPointer( 0 , 2, GL_INT, GL_FALSE, 0, 0 );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		// Attrib 1 is Color - constant here
		glVertexAttrib4fv( 1, color.data() );

		glPointSize( 32.0f );
		glEnableVertexAttribArray( 0 );
		glDrawArrays( GL_POINTS, 0, 4 );
		glDisableVertexAttribArray( 0 );

		glBindVertexArray( 0 );
	}
}
