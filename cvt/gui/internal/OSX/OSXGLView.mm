#import <cvt/gui/internal/OSX/OSXGLView.h>
#include <cvt/gui/internal/OSX/WidgetImplWinGLOSX.h>
#include <cvt/gui/internal/OSX/OSXKeyMap.h>


@implementation OSXGLView

- ( id ) initWithFrame: ( NSRect ) frame CGLContextObj: ( void* )ctx WidgetImpl: ( cvt::WidgetImplWinGLOSX* ) wimpl
{
	if ((self = [super initWithFrame:frame]) != nil)
	{
		_widgetimpl = wimpl;
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

- (BOOL)isFlipped
{
	return YES;
}

- (BOOL)acceptsFirstResponder
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
	cvt::ResizeEvent re( rself.size.width, rself.size.height, oldFrameSize.width, oldFrameSize.height );
	_widgetimpl->resizeEvent( re );
	[_glcontext update];
}

- ( void ) moveEvent: (NSNotification*)notification
{
	int ox, oy;
	// FIXME: inverted ...
	_widgetimpl->position( ox, oy );
	NSRect rself = [ [ notification object ] frame];
	cvt::MoveEvent me( rself.origin.x, rself.origin.y, ox, oy );
	_widgetimpl->moveEvent( me );
}

- ( void ) drawRect:(NSRect) r
{
	// process the paint event
	cvt::PaintEvent pe( r.origin.x, r.origin.y, r.size.width, r.size.height );
	_widgetimpl->paintEvent( pe );
}

- (void)keyDown:(NSEvent *)e
{
	cvt::KeyEvent ke;
	cvt::OSXKeyMap::mapToKeyEvent( ke, e );
	std::cout << std::hex << "0x" << [ e keyCode ] << std::endl;
	std::cout << ke << std::endl;
	_widgetimpl->keyPressEvent( ke );
}

- (void)keyUp:(NSEvent *)e
{
	cvt::KeyEvent ke;
	cvt::OSXKeyMap::mapToKeyEvent( ke, e );
	std::cout << std::hex << "0x" << [ e keyCode ] << std::endl;
	std::cout << ke << std::endl;
	_widgetimpl->keyReleaseEvent( ke );
}


- (void)flagsChanged:(NSEvent *)e
{
	cvt::KeyEvent ke;
	std::cout << std::hex << "0x" << [ e keyCode ] << std::endl;
}

- (void)mouseDown:(NSEvent *)e
{
	NSPoint pt = [ self convertPoint: [ e locationInWindow ] fromView:nil ];
	cvt::MousePressEvent pe( pt.x, pt.y, [ e buttonNumber ] + 1 );
	_widgetimpl->mousePressEvent( pe );
}


- (void)mouseUp:(NSEvent *)e
{
	NSPoint pt = [ self convertPoint: [ e locationInWindow ] fromView:nil ];
	cvt::MouseReleaseEvent re( pt.x, pt.y, [ e buttonNumber ] + 1 );
	_widgetimpl->mouseReleaseEvent( re );
}

- (void)mouseDragged:(NSEvent *)e
{
	NSPoint pt = [ self convertPoint: [ e locationInWindow ] fromView:nil ];
	cvt::MouseMoveEvent me( pt.x, pt.y, [ e buttonNumber ] + 1 );
	_widgetimpl->mouseMoveEvent( me );
}

- (void)scrollWheel:(NSEvent *)e
{
	if( [ e deltaY ] == 0 )
		return;
	int button = ( [ e deltaY ] > 0 ) ? 4 : 5;
	
	NSPoint pt = [ self convertPoint: [ e locationInWindow ] fromView:nil ];
	cvt::MousePressEvent pe( pt.x, pt.y, button );
	_widgetimpl->mousePressEvent( pe );
	cvt::MouseReleaseEvent re( pt.x, pt.y, button );
	_widgetimpl->mouseReleaseEvent( re );
}

@end
