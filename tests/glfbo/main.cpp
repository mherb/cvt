#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>

#include <cvt/io/Resources.h>

#include <cvt/gl/GLFBO.h>
#include <cvt/gl/GLTexture.h>
#include <cvt/gfx/GFX.h>

using namespace cvt;

int main()
{
	// needed for opengl context
	Window dummy("");

	GLFBO fbo( 640, 480 );

	GLTexture tex;
	tex.alloc( GL_RGBA, 640, 480, GL_RGBA, GL_UNSIGNED_BYTE );

	Image bla;
	Resources r;
	bla.load( r.find( "lena.png" ) );

	{
		GFX g( &fbo );
		fbo.attach( GL_COLOR_ATTACHMENT0, tex );
		g.setColor( Color::BLACK );
		g.fillRect( 0, 0, fbo.width(), fbo.height() );
		g.drawImage( 50, 50, 200, 200, bla );
	}

/*	fbo.bind();

	fbo.attach( GL_COLOR_ATTACHMENT0, tex );
	glClearColor( 0.0f, 1.0f, 0.0f, 0.5f );
	glClear( GL_COLOR_BUFFER_BIT );

	fbo.unbind();

	// save image
*/
	Image out;
	tex.toImage( out );
	out.save( "glfbo.png" );

	return 0;
}
