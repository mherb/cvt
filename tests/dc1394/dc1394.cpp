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

#define PG_STROBE_PRESENCE_BIT ( 1 << 31 )
#define PG_STROBE_ON_OFF_BIT ( 1 << 25 )
#define PG_SIG_POLARITY_BIT ( 1 << 24 )

using namespace cvt;

class StereoCamApp : public TimeoutHandler
{
	public:
		StereoCamApp( DC1394Camera* master, DC1394Camera* slave ) :
			TimeoutHandler(),
			_master( master ),
			_slave( slave ),
			_frames( 0.0f ),			
			_window( "Multicam App" ),
			_quitButton( "Quit" ),			
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
				

		Delegate<void ()> trigger( this, &StereoCamApp::trigger );
		_triggerButton.clicked.add( trigger );
		wl.setAnchoredBottom( 70, 20 );
		_window.addWidget( &_triggerButton, wl );

		_timerId = Application::registerTimer( 10, this );

		String movTitle;		
		_views.push_back( new ImageView() );
		_moveables.push_back( new Moveable( _views.back() ) );
		movTitle.sprintf( "Master: %s", _master->identifier().c_str() );
		_moveables.back()->setTitle( movTitle );
		_moveables.back()->setSize( 320, 240 );
		_window.addWidget( _moveables.back() );

		_views.push_back( new ImageView() );
		_moveables.push_back( new Moveable( _views.back() ) );
		movTitle.sprintf( "Slave: %s", _slave->identifier().c_str() );
		_moveables.back()->setTitle( movTitle );
		_moveables.back()->setSize( 320, 240 );
		_window.addWidget( _moveables.back() );

		configureMaster( 1 );
		configureSlave( 0 );
	}

		~StereoCamApp()
		{
			Application::unregisterTimer( _timerId );			

			_master->stopCapture();
			_slave->stopCapture();

			for( size_t i = 0; i < _moveables.size(); i++ )	{
				delete _moveables[ i ];
				delete _views[ i ];				
			}
		}

		void onTimeout()
		{
			bool newFrame[ 2 ];

			newFrame[ 0 ] = _master->nextFrame( 0 );
			newFrame[ 1 ] = _slave->nextFrame( 0 );

			if( newFrame[ 0 ] ){
				_views[ 0 ]->setImage( _master->frame() );
			}
			if( newFrame[ 1 ] ){
				_views[ 1 ]->setImage( _slave->frame() );
				_master->triggerFrame();
			}			

			_frames++;			
			if( _timer.elapsedSeconds() > 2.0f ) {
				char buf[ 200 ];
				sprintf( buf,"Multicam App FPS: %.2f", _frames / _timer.elapsedSeconds() );
				_window.setTitle( buf );
				_frames = 0;
				_timer.reset();
			}			
		}

		void trigger()
		{
			std::cout << "Trigger ... " << std::endl;
			_master->triggerFrame();
		}

	private:
		DC1394Camera*				_master;
		DC1394Camera*				_slave;
		Time						_timer;
		float						_frames;

		Window						_window;
		Button						_quitButton;		
		Button						_triggerButton;
		std::vector<Moveable*>		_moveables;
		std::vector<ImageView*>		_views;

		uint32_t					_timerId;

		void configureMaster( int strobePin )
		{
			// set to asynchronous triggering
			_master->startCapture();
			_master->setRunMode( cvt::DC1394Camera::RUNMODE_SW_TRIGGER );
			_master->enableExternalTrigger( true );
			_master->setExternalTriggerMode( cvt::DC1394Camera::EDGE_TRIGGERED_FIXED_EXPOSURE );
			// trigger from SW
			_master->setTriggerSource( cvt::DC1394Camera::TRIGGER_SOURCE_SOFTWARE );
			configureStrobe( strobePin );
		}

		void configureStrobe( int pin )
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
			//strobeReg &= ~PG_SIG_POLARITY_BIT;

			// no delay, strobe length = expose length
			strobeReg &= 0xFF000000;
			_master->setRegister( baseAddress + pinOffset, strobeReg );
		}

		void configureSlave( int triggerPin )
		{
			// slave shall trigger asynchronous
			_slave->startCapture();
			_slave->setRunMode( cvt::DC1394Camera::RUNMODE_HW_TRIGGER );
			_slave->enableExternalTrigger( true );
			_slave->setExternalTriggerMode( cvt::DC1394Camera::EDGE_TRIGGERED_EDGE_EXPOSURE );
			_slave->setExternalTriggerPolarity( cvt::DC1394Camera::TRIGGER_ON_RISING_EDGE );

			// configure to capture frame on signal change on triggerpin
			cvt::DC1394Camera::ExternalTriggerSource triggerSource = cvt::DC1394Camera::TRIGGER_SOURCE_0;
			switch( triggerPin ){
				case 0:	triggerSource = cvt::DC1394Camera::TRIGGER_SOURCE_0; break;
				case 1:	triggerSource = cvt::DC1394Camera::TRIGGER_SOURCE_1; break;
				case 2:	triggerSource = cvt::DC1394Camera::TRIGGER_SOURCE_2; break;
				case 3:	triggerSource = cvt::DC1394Camera::TRIGGER_SOURCE_3; break;
				default:
					throw CVTException( "unknown pin number for trigger source" );
			}
			_slave->setTriggerSource( triggerSource );
		}
};


int main( int argc, char* argv[] )
{

	int numCams = DC1394Camera::count();

	if( numCams < 2 ){
		std::cout << "Not enough cameras connected" << std::endl;
		return 0;
	}

	CameraInfo cinfo;
	int masterIdx = -1;
	int slaveIdx = -1;
	String masterId = "49712223533866357";
	String  slaveId = "49712223533866360";
	for( int i = 0; i < numCams; i++ ){
		DC1394Camera::cameraInfo( i, cinfo );
		std::cout << cinfo << std::endl;
		if( masterIdx == -1 && cinfo.identifier() == masterId ){
			std::cout << "master found with id:" << masterId << std::endl;
			masterIdx = i;
			continue;
		}
		if( slaveIdx == -1 && cinfo.identifier() == slaveId ){
			std::cout << "slave found with id:" << slaveId << std::endl;
			slaveIdx = i;
			continue;
		}

		if( masterIdx != -1 && slaveIdx != -1 )
			break;
	}

	if( masterIdx == -1 || slaveIdx == -1 ){
		std::cout << "Could not find cameras " << std::endl;
		return 0;
	}

	DC1394Camera master( masterIdx, cinfo.bestMatchingMode( IFormat::GRAY_UINT8, 640, 480, 30 ) );
	DC1394Camera slave( slaveIdx, cinfo.bestMatchingMode( IFormat::GRAY_UINT8, 640, 480, 30 ) );


	try {
		StereoCamApp camTimeOut( &master, &slave );
		Application::run();

	} catch( cvt::Exception e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
