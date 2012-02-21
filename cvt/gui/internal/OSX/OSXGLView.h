/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_OSXGLVIEW_H
#define CVT_OSXGLVIEW_H

#import <Cocoa/Cocoa.h>

namespace cvt {
	class WidgetImplWinGLOSX;
};

@interface OSXGLView : NSView
{
	NSOpenGLContext*		 _glcontext;
	cvt::WidgetImplWinGLOSX* _widgetimpl;
}
- (id) initWithFrame: (NSRect) frameRect CGLContextObj: ( void* ) ctx WidgetImpl: ( cvt::WidgetImplWinGLOSX* ) wimpl;
- ( void ) moveEvent: (NSNotification*)notification;
@end
#endif
