#ifndef GLVIEW_H
#define GLVIEW_H

#include <list>

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

		private:
			std::list<GLObject*> _objects;

	};
}

#endif
