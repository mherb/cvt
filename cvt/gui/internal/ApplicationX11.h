#ifndef CVT_APPLICATIONX11_H
#define CVT_APPLICATIONX11_H

#include <cvt/gui/Application.h>
#include <cvt/gui/internal/X.h>
#include <cvt/gui/internal/WidgetImplWinGLX11.h>
#include <cvt/gui/internal/GLXContext.h>
#include <cvt/gui/event/Event.h>
#include <cvt/gui/TimeoutHandler.h>
#include <cvt/gui/internal/TimerInfoList.h>
#include <cvt/gl/OpenGL.h>
#include <cvt/io/IOSelect.h>
#include <map>
#include <deque>

namespace cvt {
	class ApplicationX11 : public Application
	{
		public:
			ApplicationX11();
			~ApplicationX11();

			virtual void runApp();
			virtual void exitApp() { run = false; };

			virtual uint32_t _registerTimer( size_t interval, TimeoutHandler* t ) { return _timers.registerTimer( interval, t ); };
			virtual void _unregisterTimer( uint32_t id ) { _timers.unregisterTimer( id ); };


		private:
			ApplicationX11( const Application& );

			virtual WidgetImpl* _registerWindow( Widget* w );
			virtual void _unregisterWindow( WidgetImpl* w );

			::Display* dpy;
			GLXContext* _defaultctx;
			::Atom xatom_wmdelete;
			::Window _dummywin;
			bool run;
			std::map< ::Window, WidgetImplWinGLX11*> windows;
			std::deque< WidgetImplWinGLX11*> updates;
			IOSelect _ioselect;
			TimerInfoList _timers;
	};
}

#endif
