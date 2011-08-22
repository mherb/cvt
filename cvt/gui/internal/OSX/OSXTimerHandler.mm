#import <cvt/gui/internal/OSX/OSXTimerHandler.h>

@implementation OSXTimerHandler

-( id )initWithTimeoutHandler:(cvt::TimeoutHandler *) handler
{
    if (self) {
        _handler = handler;
    }
    return self;
}

-( void ) timeout: ( id ) sender
{
	_handler->onTimeout();
}

@end
