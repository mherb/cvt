/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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
