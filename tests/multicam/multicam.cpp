#include <set>
#include <cvt/io/Camera.h>
#include <cvt/util/Time.h>
#include <cvt/util/Exception.h>

#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/BasicTimer.h>
#include <cvt/gui/TimeoutHandler.h>

using namespace cvt;

class MultiCamApp : public TimeoutHandler
{
	public:
		MultiCamApp( std::vector<Camera*>& cams ) :
			TimeoutHandler(),
			_cams( cams ),
			_frames( 0.0f ),
			_dump( false ),
			_dumpIter( 0 ),
			_window( "Multicam App" ),
			_quitButton( "Quit" ),
			_saveButton( "Save" )
	{
		_timer.reset();

		_window.setSize( 800, 600 );
		_window.setVisible( true );
		_window.setMinimumSize( 320, 240 );

		Delegate<void ()> dquit( &Application::exit );
		_quitButton.clicked.add( dquit );

		WidgetLayout wl;
		wl.setAnchoredRight( 10, 50 );
		wl.setAnchoredBottom( 10, 20 );
		_window.addWidget( &_quitButton, wl );
		
		Delegate<void ()> save( this, &MultiCamApp::setDump );
		_saveButton.clicked.add( save );
		wl.setAnchoredBottom( 40, 20 );
		_window.addWidget( &_saveButton, wl );

		_timerId = Application::registerTimer( 20, this );

		String movTitle;
		for( size_t i = 0; i < _cams.size(); i++ ){
			_views.push_back( new ImageView() );
			_moveables.push_back( new Moveable( _views.back() ) );
			movTitle.sprintf( "Cam %d: %s", i, _cams[ i ]->identifier().c_str() );
			_moveables.back()->setTitle( movTitle );
			_moveables.back()->setSize( 320, 240 );
			_window.addWidget( _moveables.back() );
		}
	}

		~MultiCamApp()
		{
			Application::unregisterTimer( _timerId );
			for( size_t i = 0; i < _cams.size(); i++ ){
				_cams[ i ]->stopCapture();
				delete _moveables[ i ];
				delete _views[ i ];
				delete _cams[ i ];
			}
		}

		void onTimeout()
		{
			for( size_t i = 0; i < _cams.size(); i++ ){
				_cams[ i ]->nextFrame();
			}

			for( size_t i = 0; i < _cams.size(); i++ ){
				_views[ i ]->setImage( _cams[ i ]->frame() );
			}
			
			_frames++;
			if( _timer.elapsedSeconds() > 2.0f ) {
				char buf[ 200 ];
				sprintf( buf,"Multicam App FPS: %.2f", _frames / _timer.elapsedSeconds() );
				_window.setTitle( buf );
				_frames = 0;
				_timer.reset();
			}

			if( _dump ){
				_dumpIter++;

				String name;
				Image img;

				for( size_t i = 0; i < _cams.size(); i++ ){
					name.sprintf( "camera_%02d_image_%03d.png", i, _dumpIter );
					_cams[ i ]->frame().convert( img, IFormat::GRAY_UINT8 );
					img.save( name );
				}
				_dump = false;
			}
		}

		void setDump(){ _dump = true; }

	private:
		std::vector<Camera*>&		_cams;
		Time						_timer;
		float						_frames;
		bool						_dump;
		size_t						_dumpIter;

		Window						_window;
		Button						_quitButton;
		Button						_saveButton;
		std::vector<Moveable*>		_moveables;
		std::vector<ImageView*>		_views;

		uint32_t					_timerId;

};

Camera* selectCamera( std::set<size_t>& alreadySelected )
{
	int numCams = Camera::count();
	for( int i = 0; i < numCams; i++ ){
		if( alreadySelected.find( i ) != alreadySelected.end() )
			continue;

		const CameraInfo & info = Camera::info( i );
		std::cout << "Camera " << i << ": " << info << std::endl;
	}

	int selection = -1;
	std::cout << "Select camera: ";
	std::cin >> selection;
	while ( selection >= numCams || alreadySelected.find( selection ) != alreadySelected.end() ){
		std::cout << "Index out of bounds -> select camera in Range:";
		std::cin >> selection;
	}
	
	try {	
		Camera* cam = Camera::get( selection, 640, 480, 60, IFormat::UYVY_UINT8 );
		cam->startCapture();
		alreadySelected.insert( selection );
		return cam;
	} catch( const cvt::Exception& ex ){
		std::cout << ex.what() << std::endl;
	}
	return NULL;
}

int main( int argc, char* argv[] )
{
	int camerasToSelect = 1;
	if( argc >= 2 ){
		camerasToSelect = atoi( argv[ 1 ] );
		if( camerasToSelect == 0 )
			camerasToSelect = 1;
	}

	Camera::updateInfo();
	int numCams = Camera::count();
	std::cout << "Overall number of Cameras: " << numCams << std::endl;
	if( numCams < camerasToSelect ){
		std::cout << "Please connect enough cameras!" << std::endl;
		return 0;
	}

	std::vector<Camera*> cameras;
	std::set<size_t> selectedIndices;
	for( int i = 0; i < camerasToSelect; i++ ){
		cameras.push_back( selectCamera( selectedIndices ) );
	}


	try {
		MultiCamApp camTimeOut( cameras );
		Application::run();

	} catch( cvt::Exception e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
