#include <cvt/io/VideoReader.h>
#include <cvt/io/RawVideoReader.h>
#include <cvt/io/ImageSequence.h>
#include <cvt/io/Resources.h>
#include <cvt/util/Time.h>
#include <cvt/util/Exception.h>

#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/Label.h>
#include <cvt/gui/Button.h>
#include <cvt/gui/ImageView.h>
#include <cvt/gui/BasicTimer.h>
#include <cvt/gui/TimeoutHandler.h>
#include <cvt/util/Delegate.h>

using namespace cvt;

class VideoPlayer : public TimeoutHandler
{
	public:

		VideoPlayer( VideoInput* input, size_t playSpeedMs ) : 
			_window( "VideoPlayer" ), 
			_imageNumLabel( "#" ), 
			_pauseButton( "Pause" ), 
			_pause( false ),
			_video( input ), 
			_frames( 0 ),
			_seq( 0 )
		{
			// register a timer to retrieve the next video frame
			_timerId = Application::registerTimer( playSpeedMs, this );
			

			_window.setSize( 800, 600 );
			WidgetLayout wl;
			wl.setAnchoredTopBottom( 5, 30 );
			wl.setAnchoredLeftRight( 5, 5 );
			_window.addWidget( &_imView, wl );

			wl.setAnchoredBottom( 5, 20 );
			wl.setAnchoredLeft( 5, 120 );
			_window.addWidget( &_imageNumLabel, wl );

			wl.setAnchoredRight( 5, 150 );
			_window.addWidget( &_pauseButton, wl );
			
			Delegate<void ()> d( this, &VideoPlayer::togglePause );
			_pauseButton.clicked.add( d );
			
			_window.show();
			
			_time.reset();
			_video->nextFrame();
			_seq++;
			_imView.setImage( _video->frame() );

			String l;
			l.sprintf( "# %05d", _frames );
			_imageNumLabel.setLabel( l );
		}

		~VideoPlayer()
		{
			Application::unregisterTimer( _timerId );
		}

		void onTimeout()
		{ 
			if( !_pause ){
				_video->nextFrame();
				_seq++;
				_frames++;
				String l;
				l.sprintf( "# %05d", _seq );
				_imageNumLabel.setLabel( l );

				_imView.setImage( _video->frame() );
			}
		
			if( _time.elapsedSeconds() > 3.0f ){
				char buf[ 100 ];
				sprintf( buf, "VideoPlayer FPS: %0.1f", _frames / _time.elapsedSeconds() );            
				_frames = 0;
				_time.reset();           
				_window.setTitle( buf );
			}
		}

		void togglePause()
		{
			if( _pause ){
				_pauseButton.setLabel( "Pause" );
			} else {
				_pauseButton.setLabel( "Play" );
			}
			_pause = !_pause;
		}
		

	private:    
		Window			_window;
		Label			_imageNumLabel;
		Button			_pauseButton;
		bool			_pause;
		ImageView		_imView;
		VideoInput*		_video;
		uint32_t		_timerId;
		size_t			_frames;
		size_t			_seq;
		Time			_time;
};


int main( int argc, char** argv )
{
	if( argc < 3 ){
		std::cout << "Usage: " << argv[ 0 ] << "<basename> <extension>" << std::endl;
		return 0;
	}

	ImageSequence sequence( argv[ 1 ], argv[ 2 ] );
	
	try {
		VideoPlayer video( &sequence, 1000/30 );
        Application::run();
	} catch( cvt::Exception e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
