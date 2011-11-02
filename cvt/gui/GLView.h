#ifndef CVT_GLVIEW_H
#define CVT_GLVIEW_H

#include <cvt/gui/Widget.h>

namespace cvt
{
	class GLView : public Widget
	{
		public:
			GLView(){}
			virtual ~GLView(){}

		protected:
			void paintEvent( PaintEvent* ev, GFX* g );
			virtual void paintGLEvent( PaintEvent* ev ) = 0;
	};

	inline void GLView::paintEvent( PaintEvent* ev, GFX* g )
	{
		if( g->supportsGL() ){
			g->beginGL();
			paintGLEvent( ev );
			g->endGL();
		}
	}
} 

#endif
