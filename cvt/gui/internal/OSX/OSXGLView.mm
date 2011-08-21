#import <cvt/gui/internal/OSX/OSXGLView.h>
#include <cvt/gui/internal/OSX/WidgetImplWinGLOSX.h>


@implementation OSXGLView

- ( id ) initWithFrame: ( NSRect ) frame CGLContextObj: ( void* )ctx WidgetImpl: ( cvt::WidgetImplWinGLOSX* ) wimpl
{
	if ((self = [super initWithFrame:frame]) != nil)
	{
		_widgetimpl = wimpl;
		_height = frame.size.height;
		[self setAutoresizingMask: NSViewHeightSizable | NSViewWidthSizable ];
		[self allocateGState];
		_glcontext = [ [ NSOpenGLContext alloc ] initWithCGLContextObj: ctx ];
	}
	return self;
}

- ( void ) dealloc
{
    [ _glcontext release ];
    [ super dealloc ];
}

- (BOOL)isOpaque
{
    return YES;
}

- (void)lockFocus
{
    [super lockFocus];
    if( [ _glcontext view ] != self ) {
        [_glcontext setView:self];
    }
}

- (void)resizeWithOldSuperviewSize: (NSSize)oldFrameSize
{
	[super resizeWithOldSuperviewSize: oldFrameSize];

	NSRect rself = [self bounds];
	//NSLog(@"frameRect = %@", NSStringFromRect( rself ));
	_height = rself.size.height;
	cvt::ResizeEvent re( rself.size.width, rself.size.height, oldFrameSize.width, oldFrameSize.height );
	_widgetimpl->resizeEvent( &re );
	[_glcontext update];
}

- ( void ) drawRect:(NSRect) r
{
	// process the paint event
	cvt::PaintEvent pe( r.origin.x, r.origin.y, r.size.width, r.size.height );
	_widgetimpl->paintEvent( &pe );
}

- (void)mouseDown:(NSEvent *)e
{
	NSPoint pt = [ self convertPoint: [ e locationInWindow ] fromView:nil ];
	cvt::MousePressEvent pe( pt.x, _height - pt.y, [ e buttonNumber ] + 1 );
	_widgetimpl->mousePressEvent( &pe );
}

- (void)mouseUp:(NSEvent *)e
{
	NSPoint pt = [ self convertPoint: [ e locationInWindow ] fromView:nil ];
	cvt::MouseReleaseEvent re( pt.x, _height - pt.y, [ e buttonNumber ] + 1 );
	_widgetimpl->mousePressEvent( &re );
}

- (void)mouseDragged:(NSEvent *)e
{
	NSPoint pt = [ self convertPoint: [ e locationInWindow ] fromView:nil ];
	cvt::MouseMoveEvent me( pt.x, _height - pt.y, [ e buttonNumber ] + 1 );
	_widgetimpl->mouseMoveEvent( &me );
}

/*- (void)scrollWheel:(NSEvent *)e
{
	//std::cout << [e deltaY] << std::endl;
}*/

@end
