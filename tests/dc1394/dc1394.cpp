#include <set>
#include <sstream>
#include <cvt/io/DC1394Camera.h>
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
		MultiCamApp( std::vector<DC1394Camera*>& cams ) :
			TimeoutHandler(),
			_cams( cams ),
			_frames( 0.0f ),
			_dump( false ),
			_dumpIter( 0 ),
			_window( "Multicam App" ),
			_quitButton( "Quit" ),
			_saveButton( "Save" ),
			_triggerButton( "Trigger" )
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

		Delegate<void ()> trigger( this, &MultiCamApp::trigger );
		_triggerButton.clicked.add( trigger );
		wl.setAnchoredBottom( 70, 20 );
		_window.addWidget( &_triggerButton, wl );

		_timerId = Application::registerTimer( 10, this );

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
			std::vector<bool> newFrame( _cams.size() );
			for( size_t i = 0; i < _cams.size(); i++ ){
				newFrame[ i ] = _cams[ i ]->nextFrame( 10 );
			}

			for( size_t i = 0; i < _cams.size(); i++ ){
				if( newFrame[ i ] ){
					_views[ i ]->setImage( _cams[ i ]->frame() );
					_cams[ i ]->triggerFrame();
				}
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
					name.sprintf( "camera_%s_image_%03d.png", _cams[ i ]->identifier().c_str(), _dumpIter );
					_cams[ i ]->frame().convert( img, IFormat::RGBA_UINT8 );
					img.save( name );
				}
				std::cout << "dumped" << std::endl;
				_dump = false;
			}
		}

		void setDump(){ _dump = true; }

		void trigger()
		{
			std::cout << "Trigger ... " << std::endl;

#define CHAMELEON_SW_TRIGGER ( 0x62C )
			uint32_t addrBase = _cams[ 0 ]->commandRegistersBase();
			uint32_t value = 0;

			value = _cams[ 0 ]->getRegister( addrBase + CHAMELEON_SW_TRIGGER );
			std::cout << "Software Trigger: " << value << std::endl;

			//value = 0x80000000;
			//_cams[ 0 ]->setRegister( addrBase + CHAMELEON_SW_TRIGGER, value );
			//value = _cams[ 0 ]->getRegister( addrBase + CHAMELEON_SW_TRIGGER );

			//std::cout << "Software Trigger: " << value << std::endl;
			_cams[ 0 ]->triggerFrame();
		}

	private:
		std::vector<DC1394Camera*>&		_cams;
		Time						_timer;
		float						_frames;
		bool						_dump;
		size_t						_dumpIter;

		Window						_window;
		Button						_quitButton;
		Button						_saveButton;
		Button						_triggerButton;
		std::vector<Moveable*>		_moveables;
		std::vector<ImageView*>		_views;

		uint32_t					_timerId;

};

#define PG_STROBE_PRESENCE_BIT ( 1 << 31 )
#define PG_STROBE_ON_OFF_BIT ( 1 << 25 )
#define PG_SIG_POLARITY_BIT ( 1 << 24 )

void configureStrobe( DC1394Camera* _master, int pin )
{
	// configure strobePin to output strobe, as long as exposure time
	uint64_t baseAddress = _master->commandRegistersBase();
	static const uint64_t StrobeOutputInq = 0x48C;
	uint32_t strobeAddress = _master->getRegister( baseAddress + StrobeOutputInq );

	strobeAddress = ( strobeAddress << 2 ) & 0xFFFFF;

	uint32_t strobeCtrlInq = _master->getRegister( baseAddress + strobeAddress );

	uint32_t pinOffset = strobeAddress + 0x200 + 4 * pin;

	if( pin > 3 || pin < 0 ){
		throw CVTException( "unknown pin!" );
	}

	if( !( strobeCtrlInq & ( 1 << ( 31 - pin ) ) ) ){
		throw CVTException( "Strobe not present for requested pin" );
	}

	uint32_t strobeReg = _master->getRegister( baseAddress + pinOffset );

	// stop strobe when streaming stops
	strobeReg |= PG_STROBE_ON_OFF_BIT;

	// trigger on rising edge
	strobeReg |= PG_SIG_POLARITY_BIT;

	// no delay, strobe length = expose length
	strobeReg &= 0xFF000000;
	_master->setRegister( baseAddress + pinOffset, strobeReg );
}

int main( int argc, char* argv[] )
{
	uint32_t offset = 0x1110; 
	int numCams = DC1394Camera::count();

	if( argc == 2 ){
		std::stringstream ss;
		ss << std::hex << argv[ 1 ];
		ss >> offset;
	}

	std::cout << "Overall number of Cameras: " << numCams << std::endl;
	if( numCams == 0 ){
		std::cout << "No DC1394 Camera found" << std::endl;
		return 1;
	}

	CameraInfo cinfo;
	DC1394Camera::cameraInfo( 0, cinfo );
	DC1394Camera cam( 0, cinfo.bestMatchingMode( IFormat::GRAY_UINT8, 640, 480, 30 ) );

	std::cout << "TRIGGER REG: " << cam.getRegister( cam.commandRegistersBase() + 0x62C ) << std::endl;
	cam.startCapture();
	cam.setRunMode( DC1394Camera::RUNMODE_SW_TRIGGER );
	cam.enableExternalTrigger( true );
	cam.setExternalTriggerMode( DC1394Camera::EDGE_TRIGGERED_FIXED_EXPOSURE );
	cam.setTriggerSource( DC1394Camera::TRIGGER_SOURCE_SOFTWARE );

	configureStrobe( &cam, 1 );

	cam.triggerFrame();
	//std::cout << "TRIGGER REG: " << std::hex << cam.getRegister( cam.commandRegistersBase() + 0x62C ) << std::endl;
	//cam.setRegister( cam.commandRegistersBase() + 0x62C, 0x80000000 );

	std::vector<DC1394Camera*> cameras;
	cameras.push_back( &cam );

	try {
		MultiCamApp camTimeOut( cameras );
		Application::run();

	} catch( cvt::Exception e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
