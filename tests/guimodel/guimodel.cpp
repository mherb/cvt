#include <iostream>
#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/View3D.h>
#include <cvt/gui/BasicTimer.h>
#include <cvt/io/ImageIO.h>
#include <cvt/io/PlyModel.h>
#include <cvt/geom/ArcBall.h>
#include <cvt/util/Time.h>

using namespace cvt;

void timeout( BasicTimer* )
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

	View3D v3d;

	Model mdl;
	PlyModel::load( mdl, argv[ 1 ] );
	mdl.center();
	mdl.calcNormals();
	Boxf box;
	Vector3f tsize;
	mdl.calcAABB( box );
	box.getSize( tsize );
	float scale = 1.0f / ( Math::max( tsize.x, Math::max( tsize.y, tsize.z ) ) );
	mdl.scale( scale );
	GLModel glmdl( mdl );
	v3d.setModel( &glmdl );

	Moveable m2( &v3d );
	m2.setSize( 160, 120 );
	m2.setPosition( 240, 180 );
	w.addWidget( &m2 );


	Application::run();
	return 0;
}
