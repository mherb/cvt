/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef GLVIEW_H
#define GLVIEW_H

#include <QGLWidget>
#include <list>

#include "gui/GLObject.h"

namespace cvt {

	class GLView : public QGLWidget
	{
		Q_OBJECT

		public:
			GLView(QWidget *parent = 0);
			~GLView();

			QSize minimumSizeHint() const;
			QSize sizeHint() const;
			void addGLObject( GLObject* obj, bool front = true );
			void removeGLObject( GLObject* obj );

		protected:
			void initializeGL();
			void paintGL();
			void resizeGL(int width, int height);
			void mousePressEvent( QMouseEvent *event );
			void mouseMoveEvent( QMouseEvent *event );
			void wheelEvent( QWheelEvent *event );
			GLObject* objectAt( int x, int y );

		private:
			std::list<GLObject*> _objects;
			GLObject* _selected;
			int _lx, _ly;

	};
}

#endif
