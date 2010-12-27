#include <iostream>
#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/BasicTimer.h>
#include <cvt/io/ImageIO.h>
#include <cvt/io/PlyModel.h>
#include <cvt/geom/ArcBall.h>
#include <cvt/util/Time.h>

using namespace cvt;

class View3d : public Widget
{
	public:
		View3d();
		~View3d();
		void setModel( GLModel* mdl );
	protected:
		virtual void paintEvent( PaintEvent* , GFX* );
		virtual void resizeEvent( ResizeEvent* e );
		virtual void mousePressEvent( MousePressEvent* e );
		virtual void mouseMoveEvent( MouseMoveEvent* e );
		virtual void mouseReleaseEvent( MouseReleaseEvent* e );

		GLModel* _mdl;
		int		 _x1, _y1;
		Matrix4f _rotation;
		float	 _trans;
		ArcBall  _arcball;
};

inline void View3d::setModel( GLModel* mdl )
{
	_mdl = mdl;
}

View3d::View3d() : Widget(), _x1( 0 ), _y1( 0 )
{
	int w, h;

	_rotation.identity();
	_trans = -5.0f;
	size( w, h );
	_arcball.setViewportSize( w, h );
}

View3d::~View3d()
{

}

void View3d::paintEvent( PaintEvent* , GFX* g )
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

void View3d::mousePressEvent( MousePressEvent* e )
{
	if( e->button() == 1 ) {
		_x1 = e->x;
		_y1 = e->y;
	}
}


void View3d::mouseReleaseEvent( MouseReleaseEvent* e )
{
	if( e->button() == 4 ) {
		_trans += 0.25f;
		update();
	} else if( e->button() == 5 ) {
		_trans -= 0.25f;
		update();
	}
}

void View3d::mouseMoveEvent( MouseMoveEvent* e )
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


void View3d::resizeEvent( ResizeEvent* e )
{
	_arcball.setViewportSize( e->width(), e->height() );
}


void timeout( BasicTimer* timer )
{
    static cvt::Time t;

    std::cout << "Timeout: " << t.elapsedMilliSeconds() << " ms" << std::endl;
    t.reset();
}

int main(int argc, char* argv[])
{
	if( argc != 2 ) {
		std::cout << argv[ 0 ] << " ply-file" << std::endl;
		return 1;
	}


	cvt::Window w( "Test" );
	w.setSize( 640, 480 );
	w.setVisible( true );
	w.setMinimumSize( 320, 240 );

	Button button( "Quit" );
	Delegate<void ()> dquit( &Application::exit );
	button.clicked.add( &dquit );

	WidgetLayout wl;
	wl.setAnchoredRight( 10, 50 );
	wl.setAnchoredBottom( 10, 20 );
	w.addWidget( &button, wl );

	View3d v3d;

	Model mdl;
	PlyModel::load( mdl, argv[ 1 ] );
	mdl.calcNormals();
	GLModel glmdl( mdl );
	v3d.setModel( &glmdl );

	Moveable m2( &v3d );
	m2.setSize( 160, 120 );
	m2.setPosition( 240, 180 );
	w.addWidget( &m2 );


	Application::run();
	return 0;
}
