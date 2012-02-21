/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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
