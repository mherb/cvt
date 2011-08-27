#include <cvt/gui/internal/OSX/TimerInfoOSX.h>
#include <cvt/gui/internal/OSX/OSXTimerHandler.h>

namespace cvt {
	IDGenerator TimerInfoOSX::_idgen;

	TimerInfoOSX::TimerInfoOSX( size_t msinterval, TimeoutHandler* th )
	{
		_id = _idgen.allocateID();
		OSXTimerHandler* handler = [ [ OSXTimerHandler alloc ] initWithTimeoutHandler: th ];
		_nstimerhandler = ( void* ) handler;
		_nstimer = [NSTimer scheduledTimerWithTimeInterval: 0.001 * ( ( double ) ( msinterval ) )
                            target: handler
                            selector: @selector(timeout:)
                            userInfo: 0
                            repeats: YES];
	}

	TimerInfoOSX::~TimerInfoOSX()
	{
		[ ( NSTimer* ) _nstimer invalidate ];
		[ ( OSXTimerHandler* ) _nstimerhandler release ];
		_idgen.releaseID( _id );
	}


}

