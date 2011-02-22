#include <cvt/gl/GFXEngineGL.h>
#include <cvt/io/Resources.h>

#include <iostream>

namespace cvt {
			GFXEngineGL::GFXEngineGL( GLContext* ctx ) : _ctx( ctx )
			{
				Resources res;
				std::string path = res.find("default.glfont");
				_glfont.load( path.c_str() );
			}

			GFXEngineGL::~GFXEngineGL()
			{
			}

			void GFXEngineGL::begin()
			{
				_ctx->makeCurrent();
				glEnable( GL_BLEND );
				glEnable( GL_TEXTURE_2D );
				glEnable( GL_POINT_SPRITE );
				glEnable( GL_SCISSOR_TEST );
				glDisable( GL_DEPTH_TEST );
				if( _ctx->format().samples() )
					glEnable( GL_MULTISAMPLE );
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glViewport( 0, 0, _viewport.width, _viewport.height );
				glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
				setChildrect( _viewport );
			}

			void GFXEngineGL::end()
			{
				_ctx->swapBuffers();
			}

			void GFXEngineGL::setChildrect( const Recti& childrect )
			{
				_childrect = childrect;
				Recti clip;
				clip = _viewport;
				clip.intersect( _childrect );
				glScissor( clip.x, _viewport.height - ( clip.y + clip.height ), clip.width, clip.height );
			}

			void GFXEngineGL::drawLines( const Vector2f* pts, size_t n, float width, const Color& c )
			{
				Matrix4f proj;
				GL::orthoTranslation( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height, ( float ) _childrect.x, ( float ) _childrect.y );
				basicp.bind();
				basicp.setProjection( proj );
				basicp.setColor( c );
				basicp.drawLines( pts, n, width );
				basicp.unbind();
			}

			void GFXEngineGL::fillRect( const Recti& rect, const Color& c )
			{
				Matrix4f proj;
				GL::orthoTranslation( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height, ( float ) _childrect.x, ( float ) _childrect.y );
				basicp.bind();
				basicp.setProjection( proj );
				basicp.setColor( c );
				basicp.fillRect( rect.x, rect.y, rect.width, rect.height );
				basicp.unbind();
			}

			void GFXEngineGL::drawRect( const Recti& rect, float width, const Color& c )
			{}

			void GFXEngineGL::fillRoundRect( const Recti& rect, float radius, const Color& c )
			{
				Matrix4f proj;
				GL::orthoTranslation( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height, ( float ) _childrect.x, ( float ) _childrect.y );
				fillrrectp.bind();
				fillrrectp.setProjection( proj );
				fillrrectp.setColor( c );
				fillrrectp.fillRoundRect( rect.x, rect.y, rect.width, rect.height, radius );
				fillrrectp.unbind();
			}

			void GFXEngineGL::drawRoundRect( const Recti& rect, float width, float r, const Color& c )
			{}

			void GFXEngineGL::drawEllipse( const Vector2f& center, float rx, float ry, float width, const Color& c )
			{}

			void GFXEngineGL::fillEllipse( const Vector2f& center, float rx, float ry, const Color& c )
			{}

			void GFXEngineGL::strokePath( const Pathf& path, float width, const Color& c, GFX::StrokeStyle style )
			{}

			void GFXEngineGL::fillPath( const Pathf& path, const Color& c, GFX::PolygonFillRule rule )
			{}

			void GFXEngineGL::drawText( int x, int y, const char* text, const Color& c )
			{
				Matrix4f proj;
				GL::orthoTranslation( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height, ( float ) _childrect.x, ( float ) _childrect.y );
				drawtextp.bind();
				drawtextp.setProjection( proj );
				drawtextp.setColor( c );
				drawtextp.drawText( x, y, text, _glfont );
				drawtextp.unbind();
			}

			void GFXEngineGL::drawImage( int x, int y, const Image& img, float alpha )
			{
				Image* tmp = NULL;

				if( img.memType() != IALLOCATOR_GL ) {
					tmp = new Image( img, IALLOCATOR_GL );
				}

				Matrix4f proj;
				GL::orthoTranslation( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height, ( float ) _childrect.x, ( float ) _childrect.y );
				drawimgp.bind();
				drawimgp.setProjection( proj );
				drawimgp.setAlpha( alpha );
				drawimgp.drawImage( x, y, tmp?*tmp:img );
				drawimgp.unbind();

				if( tmp )
					delete tmp;
			}

			void GFXEngineGL::drawImage( const Recti& rect, const Image& img, float alpha )
			{
				Image* tmp = NULL;

				if( img.memType() != IALLOCATOR_GL ) {
					tmp = new Image( img, IALLOCATOR_GL );
				}

				Matrix4f proj;
				GL::orthoTranslation( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height, ( float ) _childrect.x, ( float ) _childrect.y );
				drawimgp.bind();
				drawimgp.setProjection( proj );
				drawimgp.setAlpha( alpha );
				drawimgp.drawImage( rect.x, rect.y, rect.width, rect.height, tmp?*tmp:img );
				drawimgp.unbind();

				if( tmp )
					delete tmp;
			}

			void GFXEngineGL::drawIcon( int x, int y, GFX::Icon i, const Color& c )
			{
				Matrix4f proj;
				GL::orthoTranslation( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height, ( float ) _childrect.x, ( float ) _childrect.y );
				drawiconp.bind();
				drawiconp.setProjection( proj );
				drawiconp.setColor( c );
				drawiconp.drawIcon( x, y, i );
				drawiconp.unbind();
			}

			void GFXEngineGL::drawIcons( const Vector2i* pts, size_t npts, GFX::Icon i, const Color& c )
			{
				Matrix4f proj;
				GL::orthoTranslation( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height, ( float ) _childrect.x, ( float ) _childrect.y );
				drawiconp.bind();
				drawiconp.setProjection( proj );
				drawiconp.setColor( c );
				drawiconp.drawIcons( pts, npts, i );
				drawiconp.unbind();
			}

			void GFXEngineGL::drawIcons( const Vector2f* pts, size_t npts, GFX::Icon i, const Color& c )
			{
				Matrix4f proj;
				GL::orthoTranslation( proj, 0, ( float ) _viewport.width, 0, ( float ) _viewport.height, ( float ) _childrect.x, ( float ) _childrect.y );
				drawiconp.bind();
				drawiconp.setProjection( proj );
				drawiconp.setColor( c );
				drawiconp.drawIcons( pts, npts, i );
				drawiconp.unbind();
			}

}
