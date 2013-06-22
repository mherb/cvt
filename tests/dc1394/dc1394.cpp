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

#include <cvt/io/ChameleonStereo.h>

using namespace cvt;

class StereoCamApp : public TimeoutHandler
{
	public:
		StereoCamApp( ChameleonStereo& stereo ) :
			TimeoutHandler(),
			_stereo( stereo ),
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

		movTitle.sprintf( "Master: %s", _stereo.leftId().c_str() );
		_moveables.back()->setTitle( movTitle );
		_moveables.back()->setSize( 320, 240 );
		_window.addWidget( _moveables.back() );

		_views.push_back( new ImageView() );
		_moveables.push_back( new Moveable( _views.back() ) );
		movTitle.sprintf( "Slave: %s", _stereo.rightId().c_str() );
		_moveables.back()->setTitle( movTitle );
		_moveables.back()->setSize( 320, 240 );
		_window.addWidget( _moveables.back() );		
	}

		~StereoCamApp()
		{
			Application::unregisterTimer( _timerId );

			for( size_t i = 0; i < _moveables.size(); i++ )	{
				delete _moveables[ i ];
				delete _views[ i ];				
			}
		}

		void onTimeout()
		{			
			if( _stereo.nextFrame( 40 ) ){
				_frames++;
				if( _timer.elapsedSeconds() > 2.0f ) {
					char buf[ 200 ];
					sprintf( buf,"Multicam App FPS: %.2f", _frames / _timer.elapsedSeconds() );
					_window.setTitle( buf );
					_frames = 0;
					_timer.reset();
				}
			}
			_views[ 0 ]->setImage( _stereo.left() );
			_views[ 1 ]->setImage( _stereo.right() );
		}

		void trigger()
		{
			std::cout << "Trigger ... " << std::endl;
			_stereo.trigger();
		}

	private:
		ChameleonStereo&			_stereo;
		Time						_timer;
		float						_frames;

		Window						_window;
		Button						_quitButton;		
		Button						_triggerButton;
		std::vector<Moveable*>		_moveables;
		std::vector<ImageView*>		_views;

		uint32_t					_timerId;
};


int main( void )
{
	int numCams = DC1394Camera::count();

	if( numCams < 2 ){
		std::cout << "Not enough cameras connected" << std::endl;
		return 0;
	}

	ChameleonStereo::Parameters params;
	params.leftId =  "49712223533866357";
	params.rightId = "49712223533866360";
	params.leftStrobePin = 1;
	params.rightTriggerPin = 0;

	try {
		ChameleonStereo stereo( params );
		StereoCamApp camTimeOut( stereo );
		Application::run();
	} catch( cvt::Exception e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
