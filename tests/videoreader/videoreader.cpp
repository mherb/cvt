#include <cvt/io/VideoReader.h>
#include <cvt/io/RawVideoReader.h>
#include <cvt/io/Resources.h>
#include <cvt/util/Time.h>
#include <cvt/util/Exception.h>

#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gui/BasicTimer.h>
#include <cvt/util/Delegate.h>

using namespace cvt;

class VideoPlayer : public Window
{
public:
	VideoPlayer( const String& videoFile ) : 
        Window( "VideoPlayer" ), _video( 0 ), _timer( 10 ), _frames( 0 )
	{
		//_video = new VideoReader( videoFile, true );
		_video = new RawVideoReader( videoFile, true );
        // register a timer to retrieve the next video frame
        Delegate<void ( BasicTimer* )> capturedelegate( this, &VideoPlayer::capture );
        _timer.timeout.add( capturedelegate );
        _timer.start();
        
        _video->nextFrame();
            
        this->setSize( 800, 600 );
        this->show();
        _time.reset();
    }
    
    ~VideoPlayer()
    {
        _timer.stop();
		delete _video;
    }
    
    void capture( BasicTimer* )
    { 
        _video->nextFrame(); 
        _frames++;
        if( _time.elapsedSeconds() > 3.0f ){
            char buf[ 100 ];
            sprintf( buf, "VideoPlayer FPS: %0.1f", _frames / _time.elapsedSeconds() );            
            _frames = 0;
            _time.reset();           
            this->setTitle( buf );
        }
        update();        
    }
        
	void paintEvent( PaintEvent*, GFX* gfx )
	{
		int w, h;
		size( w, h );
		gfx->color().set( 0.6f, 0.6f, 0.6f, 1.0f );
		gfx->fillRect( 0, 0, w, h );        
		gfx->drawImage( 0, 0, w, h, _video->frame() );
	}
    
private:    
    VideoInput*		_video;
    BasicTimer  _timer;
    Delegate< void ( BasicTimer * ) >* _captureDelegate;
    size_t      _frames;
    Time        _time;
};

int main( int argc, char** argv )
{
	
	
	if( argc < 2 ){
		std::cout << "Usage: " << argv[ 0 ] << " <filename>" << std::endl;
		return 0;
	}
	
	String xxx( argv[ 1 ] );

	try {
		VideoPlayer video( xxx );
        Application::run();
	} catch( cvt::Exception e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
