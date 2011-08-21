#include <cvt/gui/internal/OSX/ApplicationOSX.h>
#include <cvt/gui/internal/OSX/WidgetImplWinGLOSX.h>
#include <Cocoa/Cocoa.h>

namespace cvt {
	struct OSXData {
		NSAutoreleasePool* _pool;
	};

	ApplicationOSX::ApplicationOSX()
	{
		/* transform to foreground app */
		ProcessSerialNumber psn;
		GetCurrentProcess(&psn);
		TransformProcessType(&psn, kProcessTransformToForegroundApplication);
		SetFrontProcess(&psn);

		/* setup cocoa stuff inside OSXData struct */
		_osx = new OSXData();
		_osx->_pool = [[ NSAutoreleasePool alloc ] init ];
		[ NSApplication sharedApplication ];

		/* replace default menu bar */
		id menubar = [[NSMenu new] autorelease];
		id appMenuItem = [[NSMenuItem new] autorelease];
		[menubar addItem:appMenuItem];
		[NSApp setMainMenu:menubar];
		id appMenu = [ [NSMenu new ] autorelease];
		id quitMenuItem = [[[NSMenuItem alloc] initWithTitle: @"Quit" action:@selector(terminate:) keyEquivalent:@"q"] autorelease ];
		[appMenu addItem:quitMenuItem];
		[appMenuItem setSubmenu:appMenu];

		/* create default gl context */
		GLFormat format;
		_defaultctx = new CGLContext( format );
		_defaultctx->makeCurrent();
		GL::init();
	}

	WidgetImpl* ApplicationOSX::_registerWindow( Widget* w )
	{
		WidgetImpl* ret;
		if( w->isToplevel() ) {
			WidgetImplWinGLOSX* impl = new WidgetImplWinGLOSX( _defaultctx, w );
			ret = impl;
		} else {
			ret = NULL;
		}
		return ret;
	}

	void ApplicationOSX::_unregisterWindow( WidgetImpl* impl )
	{
	}

	ApplicationOSX::~ApplicationOSX()
	{
		[ _osx->_pool release ];
		delete _osx;
	}

	void ApplicationOSX::runApp()
	{
		[ NSApp run ];
	}

	void ApplicationOSX::exitApp()
	{
		[ NSApp terminate: nil ];
	}
}
