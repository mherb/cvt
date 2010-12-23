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
		GLModel* _mdl;
};

inline void View3d::setModel( GLModel* mdl )
{
	_mdl = mdl;
}

View3d::View3d() : Widget()
{
}

View3d::~View3d()
{

}

void View3d::paintEvent( PaintEvent* , GFX* g )
{
	static int R = 0;
	int w, h, x, y;
	position( x , y );
	size( w, h );
	g->color().set( 1.0f, 1.0f, 1.0f, 1.0f );
	g->fillRect( 0, 0, w, h );

	g->color().set( 1.0f, 0.0f, 0.0f, 1.0f );

	Matrix4f trans;
	trans.identity();
	trans[ 2 ][ 3 ] = -3.0f;

	R++;
	if( R > 360 )
		R = 0;
	Matrix4f rot;
	float alpha = ( float ) R;
	rot.identity();
	rot[ 1 ][ 1 ] = Math::cos( Math::deg2Rad( alpha ) );
	rot[ 1 ][ 2 ] = -Math::sin( Math::deg2Rad( alpha ) );
	rot[ 2 ][ 1 ] = Math::sin( Math::deg2Rad( alpha ) );
	rot[ 2 ][ 2 ] = Math::cos( Math::deg2Rad( alpha ) );
	alpha = 30.0f;
	Matrix4f rot2;
	rot2.identity();
	rot2[ 0 ][ 0 ] = Math::cos( Math::deg2Rad( alpha ) );
	rot2[ 0 ][ 2 ] = Math::sin( Math::deg2Rad( alpha ) );
	rot2[ 2 ][ 0 ] = -Math::sin( Math::deg2Rad( alpha ) );
	rot2[ 2 ][ 2 ] = Math::cos( Math::deg2Rad( alpha ) );

	trans *= rot;
	g->drawModel( *_mdl, trans );
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
