#include <cvt/gui/Window.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Application.h>

#include <cvt/geom/scene/Scene.h>

#include <cvt/util/Time.h>

#include "GLSceneView.h"

using namespace cvt;

int main( int argc, char** argv )
{
	if( argc != 2 ){
		std::cout << "Usage: " << argv[ 0 ] << " <scenefile>" << std::endl;
		return 0;
	}

	Time t;
	Scene* s = new Scene();
	s->load( argv[ 1 ] );
	std::cout << "Loading took: " << t.elapsedMilliSeconds() << " ms" << std::endl;
	std::cout << ( const Scene&)( *s ) << std::endl;

/*	size_t index = String( argv[ 2 ] ).toInteger();
	std::cout << index << std::endl;
	if( index >= s.geometrySize() ) {
		std::cout << "No such mesh" << std::endl;
		return 0;
	} */


	Window w( "meshView" );
	w.setSize( 640, 480 );

	GLScene glscene( *s );
	delete s;
	GLSceneView view( glscene );

	WidgetLayout wl;
	wl.setAnchoredTopBottom( 0, 0 );
	wl.setAnchoredLeftRight( 0, 0 );
	w.addWidget( &view, wl );
	w.setVisible( true );

	w.update();

	Application::run();

	return 0;

}
