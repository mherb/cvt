#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/Moveable.h>
#include <cvt/util/PluginManager.h>

#include "IFilterView.h"
#include "IFilterWindow.h"

using namespace cvt;

int main()
{
	IFilterWindow w( "IFilterView - Test" );
    w.setSize( 640, 480 );
    w.setVisible( true );
    w.setMinimumSize( 320, 240 );

    Button button( "Quit" );
	Delegate<void ()> dquit( &Application::exit );
	button.clicked.add( dquit );

    WidgetLayout wl;
    wl.setAnchoredRight( 10, 50 );
    wl.setAnchoredBottom( 10, 20 );
    w.addWidget( &button, wl );

	IFilterView view( PluginManager::instance().getIFilter("Canny") );
	Moveable m( &view );
	w.addWidget( &m );
	m.setSize( 200, 200 );
	m.setPosition( 10, 10 );
	m.setTitle( view.name() );
	m.show();

	IFilterView view2( PluginManager::instance().getIFilter("BCS") );
	Moveable m2( &view2 );
	w.addWidget( &m2 );
	m2.setSize( 200, 200 );
	m2.setPosition( 10, 10 );
	m2.setTitle( view2.name() );
	m2.show();

    Application::run();
    return 0;
}
