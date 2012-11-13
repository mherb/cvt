#include <cvt/gui/internal/OSX/ApplicationOSX.h>
#include <cvt/gui/internal/OSX/WidgetImplWinGLOSX.h>
#include <Cocoa/Cocoa.h>

#include <cvt/cl/CLPlatform.h>

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

		// OpenCL init, try to share resources with GL
		if( !CL::defaultContext() ) {
			bool clinit = false;
			std::vector<CLPlatform> clplatforms;
			CLPlatform::get( clplatforms );
			if( clplatforms.size() ) {
				_clsupport = true;

				/* try to find platform/device with cl_APPLE_gl_sharing */
				for( size_t i = 0; i < clplatforms.size() && !clinit; i++ ) {
					//std::cout << clplatforms[ i ] << std::endl;
					std::vector<CLDevice> devs;
					clplatforms[ i ].devices( devs, CL_DEVICE_TYPE_GPU );
					for( size_t k = 0; k < devs.size() && !clinit; k++ ) {
						std::vector<String> exts;
						devs[ i ].extensions( exts );
						for( size_t l = 0; l < exts.size(); l++) {
							//std::cout << exts[ l ] << std::endl;
							if( exts[ l ] == "cl_APPLE_gl_sharing" ) {
								CGLShareGroupObj shareGroup = CGLGetShareGroup( _defaultctx->_cglctx );

								cl_context_properties props[ ] = {
									CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
									(cl_context_properties) shareGroup,
									0
								};

								//std::cout << "FOUND GL SHARING" << std::endl;
								if( CL::init( ( cl_device_id ) devs[ k ], props ) ) {
									CL::_glsharing = true;
									clinit = true;
								}
								break;
							}
						}
					}
				}

				if( !clinit ) {
					// Search for GPU
					for( size_t i = 0; i < clplatforms.size() && !clinit; i++ ) {
						std::vector<CLDevice> devs;
						clplatforms[ i ].devices( devs, CL_DEVICE_TYPE_GPU );
						if( devs.size() ) {
							CL::setDefaultDevice( devs[ 0 ] );
							clinit = true;
						}
					}

					// Search for CPU
					for( size_t i = 0; i < clplatforms.size() && !clinit; i++ ) {
						std::vector<CLDevice> devs;
						clplatforms[ i ].devices( devs, CL_DEVICE_TYPE_CPU );
						if( devs.size() ) {
							CL::setDefaultDevice( devs[ 0 ] );
							clinit = true;
						}

					}
					if( !clinit )
						_clsupport = false;
				}
			} else
				_clsupport = false;
		} else
			_clsupport = true;


		_defaultctx->resetCurrent();
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

	void ApplicationOSX::_unregisterWindow( WidgetImpl* )
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
