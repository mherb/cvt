#ifndef CVT_GLSCENEVIEW_H
#define CVT_GLSCENEVIEW_H

#include <cvt/gui/GLView.h>

namespace cvt {
	class GLSceneView : public GLView
	{
		public:
			GLSceneView( GLScene* scene );
			~GLSceneView();

			GLScene* scene();

		private:
			GLScene* _scene;
	};

	inline GLSceneView::GLSceneView( GLScene* scene ) : _scene( scene )
	{
	}

	inline GLSceneView::~GLSceneView()
	{
	}

	inline GLScene* GLSceneView::scene()
	{
		return _scene;
	}

	inline void GLSceneView::paintGLEvent( PaintEvent* )
	{
		setViewport( rect() );
		_scene->draw();
	}
}

#endif
