#include <set>
#include <cvt/util/Time.h>
#include <cvt/util/ConfigFile.h>
#include <cvt/util/Exception.h>

#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/WidgetLayout.h>
#include <cvt/gui/Moveable.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/BasicTimer.h>
#include <cvt/gui/TimeoutHandler.h>

#include <cvt/io/UEyeStereo.h>

using namespace cvt;

class MultiCamApp : public TimeoutHandler
{
	public:
		MultiCamApp( UEyeStereo& stereo ) :
			TimeoutHandler(),
			_stereo( stereo ),
			_frames( 0.0f ),
			_dump( false ),
			_dumpIter( 0 ),
			_window( "Synced UEye Cams App" ),
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
		_views.push_back( new ImageView() );
		_moveables.push_back( new Moveable( _views.back() ) );
		movTitle.sprintf( "Master" );
		_moveables.back()->setTitle( movTitle );
		_moveables.back()->setSize( 320, 240 );
		_window.addWidget( _moveables.back() );
		
		_views.push_back( new ImageView() );
		_moveables.push_back( new Moveable( _views.back() ) );
		movTitle.sprintf( "Slave" );
		_moveables.back()->setTitle( movTitle );
		_moveables.back()->setSize( 320, 240 );
		_window.addWidget( _moveables.back() );
	}

		~MultiCamApp()
		{
			Application::unregisterTimer( _timerId );
			for( size_t i = 0; i < _views.size(); i++ ){
				delete _moveables[ i ];
				delete _views[ i ];
			}
		}

		void onTimeout()
		{
            if( _stereo.nextFrame() ){
                _views[ 0 ]->setImage( _stereo.masterFrame() );
                _views[ 1 ]->setImage( _stereo.slaveFrame() );
                _frames++;
            }

			if( _timer.elapsedSeconds() > 2.0f ) {
				char buf[ 200 ];
				sprintf( buf,"UEye synced  FPS: %.2f", _frames / _timer.elapsedSeconds() );
				_window.setTitle( buf );
				_frames = 0;
				_timer.reset();
			}

			if( _dump ){
				_dumpIter++;

				String name;
				Image img;
				name.sprintf( "camera_%s_image_%03d.png", 
							 _stereo.masterId().c_str(), _dumpIter );
				_stereo.masterFrame().convert( img, IFormat::RGBA_UINT8 );
				img.save( name );
				name.sprintf( "camera_%s_image_%03d.png", 
							 _stereo.slaveId().c_str(), _dumpIter );
				_stereo.slaveFrame().convert( img, IFormat::RGBA_UINT8 );
				img.save( name );
				
				std::cout << "dumped" << std::endl;
				_dump = false;
			}
		}

		void setDump(){ _dump = true; }

	private:
		UEyeStereo&					_stereo;
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


int main( int, char** )
{

	ConfigFile	config( "ueye_config.cfg" );
    String masterId = config.valueForName<String>( "master", "4002738790" );
    String slaveId  = config.valueForName<String>( "slave",  "4002738788" );

    config.save( "ueye_config.cfg" );

	try {
        UEyeStereo stereo( masterId, slaveId );
		MultiCamApp camTimeOut( stereo );
		Application::run();

	} catch( cvt::Exception e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
