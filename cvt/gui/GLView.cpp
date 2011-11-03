#include <cvt/gui/GLView.h>

#include <cvt/gl/GFXEngineGL.h>

namespace cvt {

	void GLView::paintEvent( PaintEvent* ev, GFX* g )
	{
		if( g->supportsGL() ){
			_viewport = ( ( GFXEngineGL* )g->_engine )->viewport();
			g->beginGL();
			_allowViewportChange = true;
			paintGLEvent( ev );
			_allowViewportChange = false;
			g->endGL();
		}
	}

}
