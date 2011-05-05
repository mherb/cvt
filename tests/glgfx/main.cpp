#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/util/String.h>


using namespace cvt;

class View : public Window
{
	public:
	View( ) : Window("GL-GFX")
	{
		_path.moveTo( 200.0f, 10.0f );
		_path.lineTo( 300.0f, 410.0f );
		_path.lineTo( 10.0f, 200.0f );
		_path.lineTo( 410.0f, 200.0f );
		_path.lineTo( 100.0f, 410.0f);
/*		_path.moveTo( 10.0f, 10.0f );
		_path.lineTo( 110.0f, 10.0f );
		_path.lineTo( 110.0f, 110.0f );
		_path.lineTo( 10.0f, 110.0f );*/
		_path.close();
	}

	~View()
	{
	}

	void paintEvent( PaintEvent* e, GFX* g )
	{
		Window::paintEvent( e, g );
		g->color().set( 0.0f, 1.0f, 0.0f, 0.5f );
		g->fillPath( _path, GFX::WINDING_EVEN_ODD );
	}

	private:
		Pathf _path;
};

int main( int argc, char** argv )
{
	View ui;
	ui.setSize( 640, 480 );
	ui.show();
	Application::run();
	return 0;
}
