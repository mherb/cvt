#ifndef CVT_VIEW3D_H
#define CVT_VIEW3D_H

#include <cvt/gui/Widget.h>
#include <cvt/geom/ArcBall.h>

namespace cvt {

	class View3D : public Widget
	{
		public:
			View3D();
			~View3D();
			void setModel( GLModel* mdl );

		protected:
			virtual void paintEvent( PaintEvent* , GFX* );
			virtual void resizeEvent( ResizeEvent* e );
			virtual void mousePressEvent( MousePressEvent* e );
			virtual void mouseMoveEvent( MouseMoveEvent* e );
			virtual void mouseReleaseEvent( MouseReleaseEvent* e );

		private:
			GLModel* _mdl;
			int		 _x1, _y1;
			Matrix4f _rotation;
			float	 _trans;
			ArcBall  _arcball;
	};

	inline void View3D::setModel( GLModel* mdl )
	{
		_mdl = mdl;
	}

	View3D::View3D() : Widget(), _x1( 0 ), _y1( 0 )
	{
		int w, h;

		_rotation.identity();
		_trans = -5.0f;
		size( w, h );
		_arcball.setViewportSize( w, h );
	}

	View3D::~View3D()
	{

	}

	void View3D::paintEvent( PaintEvent* , GFX* g )
	{
		int w, h;

		size( w, h );
		g->color().set( 1.0f, 1.0f, 1.0f, 1.0f );
		g->fillRect( 0, 0, w, h );

		g->color().set( 0.6f, 0.6f, 0.6f, 1.0f );

		Matrix4f trans;
		trans.identity();
		trans[ 2 ][ 3 ] = _trans;
		trans *= _rotation;
		g->drawModel( *_mdl, trans );
	}

	void View3D::mousePressEvent( MousePressEvent* e )
	{
		if( e->button() == 1 ) {
			_x1 = e->x;
			_y1 = e->y;
		}
	}


	void View3D::mouseReleaseEvent( MouseReleaseEvent* e )
	{
		if( e->button() == 4 ) {
			_trans += 0.25f;
			update();
		} else if( e->button() == 5 ) {
			_trans -= 0.25f;
			update();
		}
	}

	void View3D::mouseMoveEvent( MouseMoveEvent* e )
	{
		Matrix4f rot;
		if( e->buttonMask() & 1 ) {
			_arcball.getRotation( rot, _x1, _y1, e->x, e->y );
			_rotation = rot * _rotation;
			update();
			_x1 = e->x;
			_y1 = e->y;
		}
	}


	void View3D::resizeEvent( ResizeEvent* e )
	{
		_arcball.setViewportSize( e->width(), e->height() );
	}

}

#endif
