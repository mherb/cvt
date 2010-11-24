#include <cvt/gui/GFXGL.h>
#include <cvt/gl/OpenGL.h>

#include <cvt/gfx/IFilterVector.h>
#include <cvt/gfx/IFilterScalar.h>
#include <cvt/gfx/Image.h>
#include <cvt/io/ImageIO.h>

#include <cstring>

namespace cvt {

	GFXGL::GFXGL()
	{
		updateState();

		glEnable( GL_BLEND );
		glEnable( GL_TEXTURE_2D );
		glEnable( GL_POINT_SPRITE );
		glEnable( GL_SCISSOR_TEST );
	}

	GFXGL::~GFXGL()
	{
		/* FIXME */
	}

	void GFXGL::updateState()
	{
		Recti clip;
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		clip = _viewport;
		clip.intersect( _childrect );
		glScissor( clip.x, _viewport.height - ( clip.y + clip.height ), clip.width, clip.height );
	}

	void GFXGL::fillRect( const Recti& rect )
	{
		fillRect( rect.x, rect.y, rect.width, rect.height );
	}

	void GFXGL::fillRect( int x, int y, int width, int height )
	{
		x += _childrect.x;
		y += _childrect.y;

		Matrix4f proj;
		GL::ortho2d( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height, -10.0f, 10.0f );
		fillrectp.bind();
		fillrectp.setProjection( proj );
		fillrectp.setColor( _color );
		fillrectp.fillRect( x, y, width, height );
		fillrectp.unbind();
	}

	void GFXGL::fillRoundRect( const Recti& rect, float radius )
	{
		fillRoundRect( rect.x, rect.y, rect.width, rect.height, radius );
	}

	void GFXGL::fillRoundRect( int x, int y, int width, int height, float radius )
	{
		x += _childrect.x;
		y += _childrect.y;

		Matrix4f proj;
		GL::ortho2d( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height, -10.0f, 10.0f );
		fillrrectp.bind();
		fillrrectp.setProjection( proj );
		fillrrectp.setColor( _color );
		fillrrectp.fillRoundRect( x, y, width, height, radius );
		fillrrectp.unbind();
	}


	void GFXGL::drawText( int x, int y, const char* text )
	{
		x += _childrect.x;
		y += _childrect.y;

		Matrix4f proj;
		GL::ortho2d( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height, -10.0f, 10.0f );
		drawtextp.bind();
		drawtextp.setProjection( proj );
		drawtextp.setColor( _color );
		drawtextp.drawText( x, y, text );
		drawtextp.unbind();
	}


	void GFXGL::drawImage( int x, int y, const Image& img )
	{
		Image* tmp = NULL;

		if( img.memType() != IALLOCATOR_GL ) {
			tmp = new Image( img, IALLOCATOR_GL );
		}

		x += _childrect.x;
		y += _childrect.y;

		Matrix4f proj;
		GL::ortho2d( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height, -10.0f, 10.0f );
		drawimgp.bind();
		drawimgp.setProjection( proj );
		drawimgp.setAlpha( 1.0f );
		drawimgp.drawImage( x, y, tmp?*tmp:img );
		drawimgp.unbind();

		if( tmp )
			delete tmp;
	}


	void GFXGL::drawImage( int x, int y, int width, int height, const Image& img )
	{
		Image* tmp = NULL;

		if( img.memType() != IALLOCATOR_GL ) {
			tmp = new Image( img, IALLOCATOR_GL );
		}

		x += _childrect.x;
		y += _childrect.y;

		Matrix4f proj;
		GL::ortho2d( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height, -10.0f, 10.0f );
		drawimgp.bind();
		drawimgp.setProjection( proj );
		drawimgp.setAlpha( 1.0f );
		drawimgp.drawImage( x, y, width, height, tmp?*tmp:img );
		drawimgp.unbind();

		if( tmp )
			delete tmp;
	}

	void GFXGL::drawIcon( int x, int y, Icon i )
	{
		x += _childrect.x;
		y += _childrect.y;

		Matrix4f proj;
		GL::ortho2d( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height, -10.0f, 10.0f );
		drawiconp.bind();
		drawiconp.setProjection( proj );
		drawiconp.setColor( _color );
		drawiconp.drawIcon( x, y, i );
		drawiconp.unbind();
	}
}
