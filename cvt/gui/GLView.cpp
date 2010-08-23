#include <iostream>
#include <QMouseEvent>

#include "gui/GLView.h"

#include "gui/GLImage.h"
#include "io/ImageIO.h"

namespace cvt {

	GLView::GLView(QWidget *parent)
		: QGLWidget(QGLFormat(QGL::SampleBuffers), parent), _selected( 0 )
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
			for ( std::list<GLObject*>::reverse_iterator it=_objects.rbegin(); it != _objects.rend(); ++it ) {
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


	GLObject* GLView::objectAt( int x, int y )
	{
		for ( std::list<GLObject*>::iterator it=_objects.begin(); it != _objects.end(); ++it ) {
			if( (*it)->rect().contains( x, y ) ) {
				return (*it);
			}
		}
		return NULL;
	}

	void GLView::wheelEvent( QWheelEvent *event )
	{
		GLObject* x = objectAt( event->x(), event->y() );
		if( x )  {
			size_t delta = event->delta() / 10;
			if( x->rect().width + delta > 20 && x->rect().height + delta > 20 ) {
				x->setSize( x->rect().width + delta, x->rect().height + delta );
				updateGL();
			}
		}
	}

	void GLView::mousePressEvent(QMouseEvent *event)
	{
		_selected = NULL;
		if( _objects.size() ) {
			for ( std::list<GLObject*>::iterator it=_objects.begin(); it != _objects.end(); ++it ) {
				if( (*it)->rect().contains( event->x(), event->y() ) ) {
					GLObject* x = (*it);
					_objects.erase( it );
					_objects.push_front( x );
					_selected = x;
					_lx = event->x();
					_ly = event->y();
					updateGL();
					break;
				}
			}
		}
	}

	void GLView::mouseMoveEvent(QMouseEvent *event)
	{
		if( _objects.size() && _selected ) {
//			if( _selected->rect().contains( event->x(), event->y() ) ) {
				int dx = event->x() - _lx;
				int dy = event->y() - _ly;
				_selected->setPosition( _selected->rect().x + dx, _selected->rect().y + dy );
				_lx = event->x();
				_ly = event->y();
				updateGL();
//			} else
//				_selected = NULL;
		}

	}

}
