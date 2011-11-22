#include <cvt/gui/Application.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/Window.h>
#include "PlotView.h"

#include "PlotDataSamples.h"

using namespace cvt;

class MyWindow : public Window
{
	public:
		MyWindow( PlotView& view  ) :
			Window( "Plotview" ),
			_mov( &view )
	{
		_mov.setSize( 320, 240 );
		this->addWidget( &_mov );
	}

	private:
		Moveable			_mov;

};

int main()
{
	PlotView pview;
	MyWindow w( pview );
	w.setSize( 800, 600 );
	w.setVisible( true );
	w.setMinimumSize( 320, 240 );

	PlotDataSamples pds;
	for( float x = -3.0f; x <= 3.0f; x += 0.25f )
		pds.addSample( Point2f( x, x * x ) );
	pview.addPlotData( &pds );

	Application::run();
	return 0;
}
