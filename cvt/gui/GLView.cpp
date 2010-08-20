#include <QtGui>
#include <QtOpenGL>
#include <iostream>

#include "GLView.h"

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
		glEnable( GL_SCISSOR_TEST );
		glEnable( GL_TEXTURE_2D );
		//    glEnable(GL_MULTISAMPLE);

		/*glGenTextures( 1, &tex );
		glBindTexture( GL_TEXTURE_2D, tex );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		QImage img("lena.tif");
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, img.bits() );
		*/
	}

	void GLView::paintGL()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glLoadIdentity();
		glTranslatef(0.0, 0.0, -1.0f );

		for (list<GLObject*>::iterator it=_objects.end(); it != _objects.begin(); --it) {
			const Rect& r;
			r = *it->rect();
			glScissor( r.x, r.y, r.width, r.height );
			*it->draw();
		}

		//    glColor3f( 1.0f, 0.0f, 0.0f );
		//glScalef( scale, scale, 1.0f );
		//glBegin( GL_QUADS );
		//glTexCoord2f( 0.0f, 0.0f );
		//glVertex3f( 0.0f, 0.0f, 0.0f );
		//glTexCoord2f( 1.0f, 0.0f );
		//glVertex3f( 512.0f, 0.0f, 0.0f );
		//glTexCoord2f( 1.0f, 1.0f );
		//glVertex3f( 512.0f, 512.0f, 0.0f );
		//glTexCoord2f( 0.0f, 1.0f );
		//glVertex3f( 0.0f, 512.0f, 0.0f );
		//glEnd();
	}

	void GLView::resizeGL(int width, int height)
	{
		glViewport( 0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(0, width, height, 0, 0.99f, 20.0f);
		glMatrixMode(GL_MODELVIEW);
	}

	void GLView::wheelEvent( QWheelEvent *event )
	{
//		scale = qMin( qMax( scale - event->delta() / 1000.0f , 0.25f ), 10.0f );
//		updateGL();
	}

	void GLView::mousePressEvent(QMouseEvent *event)
	{
//		std::cout << event->button() << std::endl;
	}

	void GLView::mouseMoveEvent(QMouseEvent *event)
	{

	}

}
