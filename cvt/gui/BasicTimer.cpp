#include <cvt/gui/BasicTimer.h>
#include <cvt/gui/Application.h>

namespace cvt {
    void BasicTimer::start()
    {
	_id = Application::registerTimer( _interval, this );
    }

    void BasicTimer::stop()
    {
	if( _id ) {
	    Application::unregisterTimer( _id );
	    _id = 0;
	}
    }
}
