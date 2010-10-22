#include <cvt/gui/GFXGL.h>
#include <cvt/gl/OpenGL.h>

#include <cvt/gfx/IFilterVector.h>
#include <cvt/gfx/IFilterScalar.h>
#include <cvt/gfx/Image.h>
#include <cvt/io/ImageIO.h>

#include <cstring>
#include <cvt/gui/internal/glfont.h>

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

		updateState();

		glGenBuffers( 1, &vbuffer );
		glGenVertexArrays( 1, &varray );
		glGenTextures( 1, &texfont );

		glBindTexture(GL_TEXTURE_2D, texfont );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, _glfont.width, _glfont.height, 0, GL_RED, GL_UNSIGNED_BYTE, _glfont.texdata );
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
		int vertices[ 8 ] = {
			x	 , y + h,
			x	 , y    ,
			x + w, y + h,
			x + w, y
		};


		progbasic.bind();
		IFilterVector16 vec;
		ortho2d( vec, 0.0f, ( float ) width, 0.0f, ( float ) height, -10.0f, 10.0f );
		progbasic.setArg( "MVP", &vec );
		unsigned int vtx = progbasic.getAttribLocation( "Vertex" );
		unsigned int col = progbasic.getAttribLocation( "Color" );

		glBindBuffer( GL_ARRAY_BUFFER, vbuffer );
		glBufferData( GL_ARRAY_BUFFER, sizeof( int ) * 8, vertices, GL_DYNAMIC_DRAW );

		// Attrib 0 is Vertex data
		glBindVertexArray( varray );
		glVertexAttribPointer( vtx , 2, GL_INT, GL_FALSE, 0, 0 );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		// Attrib 1 is Color - constant here
		glVertexAttrib4fv( col, color.data() );

		glEnableVertexAttribArray( vtx );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		glDisableVertexAttribArray( vtx );

		glBindVertexArray( 0 );
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
		unsigned int vtx = progbasic.getAttribLocation( "Vertex" );
		unsigned int col = progbasic.getAttribLocation( "Color" );


		glBindBuffer( GL_ARRAY_BUFFER, vbuffer );
		glBufferData( GL_ARRAY_BUFFER, sizeof( int ) * 3 * len, vertices, GL_DYNAMIC_DRAW );
		delete[] vertices;

		// Attrib 0 is Vertex data
		glBindVertexArray( varray );
		glVertexAttribPointer( vtx , 3, GL_INT, GL_FALSE, 0, 0 );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		// Attrib 1 is Color - constant here
		glVertexAttrib4fv( col, color.data() );

		glPointSize( _glfont.ptsize );
		glPointParameteri( GL_POINT_SPRITE_COORD_ORIGIN, GL_UPPER_LEFT );
		glEnableVertexAttribArray( vtx );
		glDrawArrays( GL_POINTS, 0, len );
		glDisableVertexAttribArray( vtx );

		glBindVertexArray( 0 );
	}
}
