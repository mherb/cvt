#include <cvt/gui/GFXGL.h>
#include <cvt/gl/OpenGL.h>

namespace cvt {

	GFXGL::GFXGL()
	{
		updateState();
	}

	void GFXGL::updateState()
	{
		glEnable( GL_BLEND );
		glEnable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_COLOR_ARRAY );
	}

	void GFXGL::fillRect( const Recti& rect )
	{
		fillRect( rect.x, rect.y, rect.width, rect.height );
	}

	void GFXGL::fillRect( int x, int y, int width, int height )
	{
		float vertices[ 8 ] = {
			( float ) x + 0.5f, ( float ) ( y + height ) + 0.5f,
			( float ) x + 0.5f, ( float ) y + 0.5f,
			( float ) ( x + width ) + 0.5f, ( float ) ( y + height ) + 0.5f,
			( float ) ( x + width ) + 0.5f, ( float ) y + 0.5f,
		};
		glVertexPointer( 2, GL_FLOAT, 0, vertices );
		glColor4f( color.red(), color.green(), color.blue(), color.alpha() );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4);
	}

}
