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
	int						 _height;
}
- (id) initWithFrame: (NSRect) frameRect CGLContextObj: ( void* ) ctx WidgetImpl: ( cvt::WidgetImplWinGLOSX* ) wimpl;
@end

#endif
