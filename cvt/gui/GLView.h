#ifndef CVT_GLVIEW_H
#define CVT_GLVIEW_H

#include <cvt/gui/Widget.h>

namespace cvt
{
	class GLView : public Widget
	{
		public:
			GLView() :
				_viewport( rect() ),
				_allowViewportChange( false )
			{
			}

			virtual ~GLView(){}

			void setViewport( const Recti& rect );
			void setViewport( int x, int y, size_t w, size_t h );

		protected:
			void paintEvent( PaintEvent& ev, GFX& g );
			virtual void paintGLEvent( PaintEvent& ev ) = 0;

			Recti	_viewport;
			bool	_allowViewportChange;
	};

	inline void GLView::setViewport( int x, int y, size_t w, size_t h )
	{
		if( _allowViewportChange ) {
			Recti r = rect();
			Recti view( x + r.x, y + r.y, w, h );
			r.intersect( view );
			glViewport( r.x, _viewport.height - ( r.y + r.height ), r.width, r.height );
		}
	}

	inline void GLView::setViewport( const Recti& vrect )
	{
		if( _allowViewportChange ) {
			Recti r = rect();
			r.intersect( vrect );
			glViewport( r.x, _viewport.height - ( r.y + r.height ), r.width, r.height );
		}
	}
}

#endif
