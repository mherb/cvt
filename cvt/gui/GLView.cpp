#include <iostream>

#include "gui/GLView.h"

#include "gui/GLImage.h"
#include "io/ImageIO.h"

namespace cvt {

	GLView::GLView(QWidget *parent)
		: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
	{

	}

	GLView::~GLView()
	{
	}

	void GLView::addGLObject( GLObject* obj, bool front )
	{
		if( front )
			_objects.push_front( obj );
		else
			_objects.push_back( obj );
		updateGL();
	}

	void GLView::removeGLObject( GLObject* obj )
	{
		_objects.remove( obj );
		updateGL();
	}


	QSize GLView::minimumSizeHint() const
	{
		return QSize(300, 200);
	}

	QSize GLView::sizeHint() const
	{
		return QSize(640, 480);
	}


	void GLView::initializeGL()
	{
		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

		glShadeModel( GL_SMOOTH );
		glEnable( GL_TEXTURE_2D );
		//    glEnable(GL_MULTISAMPLE);
	}

	void GLView::paintGL()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glLoadIdentity();
		glTranslatef(0.0, 0.0, -1.0f );

		if( _objects.size() ) {
			for ( std::list<GLObject*>::iterator it=_objects.begin(); it != _objects.end(); ++it ) {
				(*it)->draw();
			}
		}
	}

	void GLView::resizeGL(int width, int height)
	{
		glViewport( 0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(0.0f, width, height, 0.0f, 0.99f, 20.0f);
		glMatrixMode(GL_MODELVIEW);
	}

	void GLView::wheelEvent( QWheelEvent *event )
	{
		//		scale = qMin( qMax( scale - event->delta() / 1000.0f , 0.25f ), 10.0f );
		//		updateGL();

		}

	void GLView::mousePressEvent(QMouseEvent *event)
	{
	}

	void GLView::mouseMoveEvent(QMouseEvent *event)
	{

	}

}
