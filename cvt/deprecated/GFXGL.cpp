#include <cvt/gui/GFXGL.h>
#include <cvt/gl/OpenGL.h>
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
		glDisable( GL_DEPTH_TEST );
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
		GL::ortho( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height );
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
		GL::ortho( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height );
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
		GL::ortho( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height );
		drawtextp.bind();
		drawtextp.setProjection( proj );
		drawtextp.setColor( _color );
		drawtextp.drawText( x, y, text );
		drawtextp.unbind();
	}

	int GFXGL::textWidth( const char* text )
	{
		return drawtextp.textWidth( text );
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
		GL::ortho( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height );
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
		GL::ortho( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height );
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
		GL::ortho( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height );
		drawiconp.bind();
		drawiconp.setProjection( proj );
		drawiconp.setColor( _color );
		drawiconp.drawIcon( x, y, i );
		drawiconp.unbind();
	}

	void GFXGL::drawIcons( const Vector2i* pts, size_t npts, Icon i )
	{
		Matrix4f proj;
		GL::ortho( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height );
		drawiconp.bind();
		drawiconp.setProjection( proj );
		drawiconp.setColor( _color );
		drawiconp.drawIcons( pts, npts, i, _childrect.x, _childrect.y );
		drawiconp.unbind();
	}


	void GFXGL::drawModel( GLModel& mdl, const Matrix4f& modelview, float near, float far )
	{
		Matrix4f proj, tmp;
		GL::subviewport( proj, _childrect.x, _childrect.y, _childrect.width, _childrect.height, _viewport.width, _viewport.height );
		GL::perspective( tmp, 60.0f, ( float ) _viewport.width / ( float ) _viewport.height, near, far );

		proj *= tmp;

		glEnable( GL_DEPTH_TEST );
		modelp.bind();
		mdl.setColor( _color );
		modelp.setLightPosition( Vector3f( 0.0f, 0.0f, -1.0f ) );
		modelp.setProjection( proj, modelview );
		modelp.drawModel( mdl );
		modelp.unbind();
		glDisable( GL_DEPTH_TEST );
	}
}
