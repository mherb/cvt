#ifndef CVT_OSXTIMEOUTHANDLER_H
#define CVT_OSXTIMEOUTHANDLER_H

#import <Cocoa/Cocoa.h>
#include <cvt/gui/TimeoutHandler.h>

@interface OSXTimerHandler : NSObject
{
	cvt::TimeoutHandler* _handler;
}
-( id ) initWithTimeoutHandler: ( cvt::TimeoutHandler* ) handler;
-( void ) timeout: ( id ) sender;
@end

#endif
