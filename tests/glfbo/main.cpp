#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>

#include <cvt/io/Resources.h>

#include <cvt/gl/GLFBO.h>
#include <cvt/gl/GLTexture.h>
#include <cvt/gfx/GFX.h>
#include <cvt/gl/GLTeapot.h>

#include <cvt/gl/progs/GLDrawModelProg.h>
#include <cvt/gl/progs/GLDrawImageProg.h>

using namespace cvt;

int main()
{
	// needed for opengl context
	Window dummy("");

	GLFBO fbo( 640, 480 );

	GLTexture tex;
	tex.alloc( GL_RGBA, 640, 480, GL_RGBA, GL_UNSIGNED_BYTE );
	GLTexture texdepth;
	texdepth.alloc( GL_DEPTH_COMPONENT16, 640, 480, GL_DEPTH_COMPONENT, GL_FLOAT );
	texdepth.bind();

	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE );
	texdepth.unbind();

	GLDrawModelProg glprog;

	Image bla;
	Resources r;
	bla.load( r.find( "lena.png" ) );

	{
		GFX g( &fbo );
		fbo.attach( GL_COLOR_ATTACHMENT0, tex );
		GLRBO rbo( GL_DEPTH_COMPONENT, 640, 480 );
		fbo.attach( GL_DEPTH_ATTACHMENT, texdepth );

		g.setColor( Color::BLACK );
		g.fillRect( 0, 0, fbo.width(), fbo.height() );
		g.drawImage( 50, 50, 200, 200, bla );

		glClear( GL_DEPTH_BUFFER_BIT );
		glEnable( GL_DEPTH_TEST );
		glprog.bind();
		Matrix4f mdl,proj;
		mdl.setIdentity();
		mdl[ 2 ][ 3 ] = -20.0f;
		GL::perspective( proj, 60.0f, 0.75f, 0.01f, 100.0f, true );
		glprog.setLightPosition( Vector3f( 0, 0, -1 ) );
		glprog.setProjection( proj, mdl );

		GLTeapot teapot;
		teapot.setColor( Color::GREEN );
		teapot.draw();
		glprog.unbind();

		glDisable( GL_DEPTH_TEST );
	}

	{
		GFX g( &fbo );

		glDisable( GL_DEPTH_TEST );
	texdepth.bind();
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		texdepth.unbind();
		GLDrawImageProg iprog;

		Matrix4f proj;
		GL::orthoTranslation( proj, 0, ( float ) 640, 0, ( float ) 480, ( float ) 0, ( float ) 0, -1.0f, 1.0f, true );
		iprog.bind();
		iprog.setProjection( proj );
		iprog.setAlpha( 1.0 );
		iprog.drawImage( 0,0, 640, 480, texdepth );
		iprog.unbind();

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

//	texdepth.toImage( out );
//	out.save( "glfbodepth.png" );

	return 0;
}
